#ifndef LIFE_H
#define LIFE_H

#include <stdint.h>
#include <unordered_map>
#include "LifeBase.h"

// Infinite Conway's Life (B3/S23) stored as sparse 64x64-bit chunks.
// A step processes 64 cells at once with bit-sliced neighbour counting.

struct LifeChunk {
    uint64_t rows[64]; // bit x of rows[y] is the cell (x, y) inside the chunk
};

class Life : public LifeBase {
private:
    typedef std::unordered_map<uint64_t, LifeChunk> ChunkMap;
    ChunkMap chunks;
    long populationCached;

public:
    Life();

    bool test(long col, long row);
    void burn(long col, long row);
    void kill(long col, long row);
    unsigned long population();
    void clear();
    void step();

    void iterate(LifeCellConsumer *it);
    void iterate(LifeCellConsumer* it, long l, long t, long r, long b);
};

#endif // LIFE_H
