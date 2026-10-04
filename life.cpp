#include "life.h"
#include <algorithm>
#include <vector>

namespace {

inline uint64_t chunkKey(int32_t cx, int32_t cy) {
    return ((uint64_t)(uint32_t)cx << 32) | (uint32_t)cy;
}

inline int32_t keyX(uint64_t k) { return (int32_t)(k >> 32); }
inline int32_t keyY(uint64_t k) { return (int32_t)(uint32_t)k; }

// floor(v / 64) and v mod 64 for negative v too
inline int32_t chunkOf(long v) { return (int32_t)(v >> 6); }
inline int inChunk(long v) { return (int)(v & 63); }

bool isEmpty(const LifeChunk& c) {
    uint64_t any = 0;
    for(int i = 0; i < 64; i++) any |= c.rows[i];
    return any == 0;
}

const LifeChunk emptyChunk = {};

// Bit-sliced sums for one row: horizontal neighbours of every cell
struct RowSums {
    uint64_t s0, s1; // L + C + R as a 2-bit number
    uint64_t h0, h1; // L + R as a 2-bit number
};

inline RowSums rowSums(uint64_t west, uint64_t center, uint64_t east) {
    uint64_t left = (center << 1) | (west >> 63);
    uint64_t right = (center >> 1) | (east << 63);
    RowSums s;
    s.h0 = left ^ right;
    s.h1 = left & right;
    s.s0 = s.h0 ^ center;
    s.s1 = s.h1 | (s.h0 & center);
    return s;
}

// Computes the next generation of chunk (cx, cy)
LifeChunk nextChunk(const std::unordered_map<uint64_t, LifeChunk>& map, int32_t cx, int32_t cy) {
    const LifeChunk* n[3][3];
    for(int dy = -1; dy <= 1; dy++) {
        for(int dx = -1; dx <= 1; dx++) {
            auto it = map.find(chunkKey(cx + dx, cy + dy));
            n[dy + 1][dx + 1] = it == map.end() ? &emptyChunk : &it->second;
        }
    }

    // rows -1..64 are stored at indices 0..65
    RowSums rs[66];
    rs[0]  = rowSums(n[0][0]->rows[63], n[0][1]->rows[63], n[0][2]->rows[63]);
    rs[65] = rowSums(n[2][0]->rows[0],  n[2][1]->rows[0],  n[2][2]->rows[0]);
    for(int y = 0; y < 64; y++)
        rs[y + 1] = rowSums(n[1][0]->rows[y], n[1][1]->rows[y], n[1][2]->rows[y]);

    LifeChunk res;
    for(int y = 0; y < 64; y++) {
        const RowSums& a = rs[y];
        const RowSums& m = rs[y + 1];
        const RowSums& b = rs[y + 2];

        // a.s + b.s
        uint64_t t0 = a.s0 ^ b.s0, c0 = a.s0 & b.s0;
        uint64_t x1 = a.s1 ^ b.s1;
        uint64_t t1 = x1 ^ c0;
        uint64_t t2 = (a.s1 & b.s1) | (c0 & x1);
        // + m.h
        uint64_t u0 = t0 ^ m.h0, c1 = t0 & m.h0;
        uint64_t x2 = t1 ^ m.h1;
        uint64_t u1 = x2 ^ c1;
        uint64_t c2 = (t1 & m.h1) | (c1 & x2);
        uint64_t u2 = t2 | c2;
        // count = u0 + 2*u1 + 4*u2; alive next if count == 3, or count == 2 and alive
        res.rows[y] = u1 & ~u2 & (u0 | n[1][1]->rows[y]);
    }
    return res;
}

} // namespace

Life::Life() : populationCached(-1L) {}

bool Life::test(long col, long row) {
    auto it = chunks.find(chunkKey(chunkOf(col), chunkOf(row)));
    if(it == chunks.end()) return false;
    return (it->second.rows[inChunk(row)] >> inChunk(col)) & 1;
}

void Life::burn(long col, long row) {
    populationCached = -1L;
    auto it = chunks.find(chunkKey(chunkOf(col), chunkOf(row)));
    if(it == chunks.end())
        it = chunks.emplace(chunkKey(chunkOf(col), chunkOf(row)), emptyChunk).first;
    it->second.rows[inChunk(row)] |= (uint64_t)1 << inChunk(col);
}

void Life::kill(long col, long row) {
    auto it = chunks.find(chunkKey(chunkOf(col), chunkOf(row)));
    if(it == chunks.end()) return;
    populationCached = -1L;
    it->second.rows[inChunk(row)] &= ~((uint64_t)1 << inChunk(col));
    if(isEmpty(it->second)) chunks.erase(it);
}

void Life::clear() {
    populationCached = -1L;
    chunks.clear();
}

void Life::step() {
    // candidates: live chunks plus the neighbours that their edge cells can spill into
    std::vector<uint64_t> cand;
    cand.reserve(chunks.size() * 2);
    for(const auto& kv : chunks) {
        const LifeChunk& c = kv.second;
        int32_t cx = keyX(kv.first), cy = keyY(kv.first);

        uint64_t left = 0, right = 0;
        for(int i = 0; i < 64; i++) {
            left |= c.rows[i] & 1;
            right |= c.rows[i] >> 63;
        }
        bool top = c.rows[0] != 0, bottom = c.rows[63] != 0;
        bool l = left, r = right;
        bool any = top || bottom || l || r;

        cand.push_back(kv.first);
        if(!any) continue;
        // a pattern at an edge reaches the adjacent chunk; at a corner, the diagonal one
        if(top)    cand.push_back(chunkKey(cx, cy - 1));
        if(bottom) cand.push_back(chunkKey(cx, cy + 1));
        if(l)      cand.push_back(chunkKey(cx - 1, cy));
        if(r)      cand.push_back(chunkKey(cx + 1, cy));
        if((c.rows[0] & 1) )               cand.push_back(chunkKey(cx - 1, cy - 1));
        if((c.rows[0] >> 63) )             cand.push_back(chunkKey(cx + 1, cy - 1));
        if((c.rows[63] & 1) )              cand.push_back(chunkKey(cx - 1, cy + 1));
        if((c.rows[63] >> 63) )            cand.push_back(chunkKey(cx + 1, cy + 1));
    }
    std::sort(cand.begin(), cand.end());
    cand.erase(std::unique(cand.begin(), cand.end()), cand.end());

    ChunkMap next;
    next.reserve(cand.size());
    for(uint64_t k : cand) {
        LifeChunk c = nextChunk(chunks, keyX(k), keyY(k));
        if(!isEmpty(c)) next.emplace(k, c);
    }
    chunks.swap(next);
    populationCached = -1L;
}

unsigned long Life::population() {
    if(populationCached < 0L) {
        long total = 0;
        for(const auto& kv : chunks)
            for(int i = 0; i < 64; i++)
                total += __builtin_popcountll(kv.second.rows[i]);
        populationCached = total;
    }
    return populationCached;
}

void Life::iterate(LifeCellConsumer *li) {
    for(const auto& kv : chunks) {
        long ox = (long)keyX(kv.first) * 64, oy = (long)keyY(kv.first) * 64;
        for(int y = 0; y < 64; y++) {
            for(uint64_t bits = kv.second.rows[y]; bits; bits &= bits - 1)
                li->run(ox + __builtin_ctzll(bits), oy + y, true);
        }
    }
}

void Life::iterate(LifeCellConsumer* li, long left, long top, long right, long bottom) {
    for(const auto& kv : chunks) {
        long ox = (long)keyX(kv.first) * 64, oy = (long)keyY(kv.first) * 64;
        if(ox > right || ox + 63 < left || oy > bottom || oy + 63 < top) continue;
        for(int y = 0; y < 64; y++) {
            long yy = oy + y;
            if(yy < top || yy > bottom) continue;
            for(uint64_t bits = kv.second.rows[y]; bits; bits &= bits - 1) {
                long xx = ox + __builtin_ctzll(bits);
                if(xx >= left && xx <= right)
                    li->run(xx, yy, true);
            }
        }
    }
}
