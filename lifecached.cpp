#include "lifecached.h"
#include <math.h>

void LifeCached::init(int fBurn, int fSurviveMin, int fSurviveMax) {
    cache = NULL;
    setFormula(fBurn, fSurviveMin, fSurviveMax);
    left = top = 0;
    active = &m1;
    sparse = &m2;
}

LifeCached::LifeCached(int fBurn, int fSurviveMin, int fSurviveMax) {
    init(fBurn, fSurviveMin, fSurviveMax);
}

void LifeCached::turn() {
    QRect bounds = active->validBlocksRect();
    if(sparse->getBlocksWidth() < bounds.width() + 2
            || sparse->getBlocksHeight() < bounds.height() + 2)
        sparse->resizeBlocks(bounds.width() + 2, bounds.height() + 2);
}

LifeCached::LifeCached() {
    cache = NULL;
    setFormula(3, 2, 3);
    left = top = 0;
    active = &m1;
    sparse = &m2;
}

LifeCached::~LifeCached() {
    delete cache;
}

quint16 LifeCached::uniteH(quint16 left, quint16 right) {
    return ((left & 0xcccc) >> 2) | ((right & 0x3333) << 2);
}

quint16 LifeCached::uniteV(quint16 top, quint16 bottom) {
    return (bottom << 8) | (top >> 8);
}

quint16 LifeCached::unite(quint16 lt, quint16 rt, quint16 lb, quint16 rb) {
    return ((lt & 0xcc00) >> 10)
            | ((rt & 0x3300) >> 6)
            | ((lb & 0x00cc) << 6)
            | ((rb & 0x0033) << 10);
}

void LifeCached::makePrecache() {
    if(!cache) cache = new unsigned char[65536];
    for(int i = 0;i < 65536;i++) {
        cache[i] = computeLife(i);
    }
}

unsigned char LifeCached::computeLife(u_int16_t v) const {
    unsigned char r = 0;
    for(int n = 5;n < 11;n++) {
        if(n == 7) n += 2;
        int neighbors = 0;
        for(int y = -1;y < 2;y++) {
            for(int x = -1;x < 2;x++) {
                if(!x && !y) continue;
                if(v & ((u_int16_t)1 << (n + y * 4 + x)))
                    neighbors++;
            }
        }
        if(v & (((u_int16_t)1) << n)) {
            if(neighbors >= fSurviveMin && neighbors <= fSurviveMax)
                r |= (unsigned char)1 << (n - 5 - (n >= 9 ? 2 : 0));
        } else {
            if(neighbors == fBurn)
                r |= (unsigned char)1 << (n - 5 - (n >= 9 ? 2 : 0));
        }
    }
    return r;
}

const int *LifeCached::getFormula() {
    return &fBurn;
}

void LifeCached::setFormula(int fBurn, int fSurviveMin, int fSurviveMax) {
    this->fBurn = fBurn;
    this->fSurviveMin = fSurviveMin;
    this->fSurviveMax = fSurviveMax;
    makePrecache();
}

bool LifeCached::test(int x, int y) const {
    if(x < -left || y < -top) return false; // out of bounds
    return active->get(x + left, y + top);
}

void LifeCached::burn(int x, int y) {
    int mleft = 0, mright = 0,
            mtop = 0, mbottom = 0;

    if(x + left < 0) {
        mleft = ceil((double)(-x - left) / BM_SIZE);
//        x += mleft * 8;
    } else if(x + left >= active->getWidth())
        mright = ceil((double)(x + left - active->getWidth() + 1) / BM_SIZE);
    if(y + top < 0) {
        mtop = ceil((double)(-y - top) / BM_SIZE);
//        y += mtop * 8;
    } else if(y + top >= active->getHeight())
        mbottom = ceil((double)(y + top - active->getHeight() + 1) / BM_SIZE);

    if(mleft || mright || mtop || mbottom) {
        active->padBlocks(mleft, mtop, mright, mbottom);
        left += mleft * BM_SIZE;
        top += mtop * BM_SIZE;
    }
    active->set(x + left, y + top, true);
}

void LifeCached::kill(int x, int y) {
    x += left;
    y += top;

    active->set(x, y, false);
}

unsigned long LifeCached::population() const {
    return active->count();
}

void LifeCached::clear() {
    active->clear();
    sparse->clear();
}

void LifeCached::step() {

}

void LifeCached::iterate(LifeCellConsumer* it) {
    iterate(it, 0, 0, active->getWidth()-1, active->getHeight()-1);
}

void LifeCached::iterate(LifeCellConsumer* it, int l, int t, int r, int b) {
    for(int y = t;y <= b;y++) {
        for(int x = l;x <= r;x++) {
            if(active->get(x, y))
                it->run(x-left, y-top, true);
        }
    }
}
