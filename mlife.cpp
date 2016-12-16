#include "mlife.h"
#include <stddef.h>
#include <math.h>
#include <QIntegerForSize>
#include <QDateTime>

#include <iostream>
using namespace std;

/*
int bpop(u_int64_t x) {
    x = x - ((x >> 1) & 0x5555555555555555);
    x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f;
    x = x + (x >> 8);
    x = x + (x >> 16);
    x = x + (x >> 32);
    return x & 0x3f;
}
*/

int bpop(u_int64_t v) {
    v = v - ((v >> 1) & 0x5555555555555555);
    v = (v & 0x3333333333333333) + ((v >> 2) & 0x3333333333333333);
    v = (v + (v >> 4)) & 0x0f0f0f0f0f0f0f0f;
    return (u_int64_t)(v * 0x0101010101010101) >> 56;
}

int neighbors(const BMatrix *m, size_t x, size_t y) {
    int n = 0;
    for(int _y = -1;_y < 2;_y++) {
        for(int _x = -1;_x < 2;_x++) {
            if(!(_x|_y)) continue;
            if(m->get(x+_x, y+_y)) n++;
        }
    }
    return n;
}

// constructors, destructor

MLife::MLife() : m1(), m2() {
    left = top = 0;
    fBurn = 3;
    fSurviveMin = 2;
    fSurviveMax = 3;
    alive = 0;
    active = &m1;
    sparse = &m2;
}

MLife::~MLife() {

}

// public interfaces

bool MLife::test(long x, long y) const {
    if(x < -left || y < -top) return false; // out of bounds
    return active->get(x + left, y + top);
}

void MLife::burn(long X, long Y) {
    long mleft = 0, mright = 0,
            mtop = 0, mbottom = 0;

    if(X + left < 0) {
        mleft = ceil((double)(-X) / BM_SIZE);
        left += mleft * BM_SIZE;
//        x += mleft * 8;
    } else if(X + left >= active->getWidth())
        mright = ceil((double)(X - active->getWidth() + 1) / BM_SIZE);
    if(Y + top < 0) {
        mtop = ceil((double)(-Y) / BM_SIZE);
        top += mtop * BM_SIZE;
//        y += mtop * 8;
    } else if(Y + top >= active->getHeight())
        mbottom = ceil((double)(Y - active->getHeight() + 1) / BM_SIZE);

    if(mleft || mright || mtop || mbottom)
        active->padBlocks(mleft, mtop, mright, mbottom);
    active->set(X + left, Y + top, true);
}

void MLife::kill(long x, long y) {
    x += left;
    y += top;

    active->set(x, y, false);
}

unsigned long MLife::population() const {
    return active->count();
}

void MLife::clear() {
    active->clear();
    sparse->clear();
    alive = 0;
}

void MLife::step() {
    // prepare matrix size

    qint64 tstart = QDateTime::currentMSecsSinceEpoch();

    prepareTurn();
    qint64 tend = QDateTime::currentMSecsSinceEpoch();
//    cout << "Prepare turn takes " << (tend - tstart) << " ms" << endl;

    // do the job
    size_t pop = 0;
    size_t bottom = active->getHeight();
    size_t right = active->getWidth();
    for(size_t y = 0;y < bottom;y++) {
        for(size_t x = 0;x < right;x++) {
            int n = neighbors(active, x, y);
            if(active->get(x, y)) {
                if(n >= fSurviveMin && n <= fSurviveMax)
                    sparse->set(x, y, true);
                pop++;
            } else {
                if(n == fBurn)
                    sparse->set(x, y, true);
                pop++;
            }
        }
    }
    qint64 tend2 = QDateTime::currentMSecsSinceEpoch();

//    cout << "Make turn takes " << (tend2 - tend) << " ms" << endl;

    // swap active and sparse matrices
    if(active == &m1) {
        active = &m2;
        sparse = &m1;
    } else {
        active = &m1;
        sparse = &m2;
    }
}

void MLife::iterate(LifeCellConsumer* it, long l, long t, long r, long b) {
//    rdlock();
//    size_t wd = active->getWidth();
//    size_t ht = active->getHeight();
    for(long y = t;y <= b;y++) {
        for(long x = l;x <= r;x++) {
            if(active->get(x, y))
                it->run(x-left, y-top, true);
        }
    }
//    unlock();
}

void MLife::iterate(LifeCellConsumer* it) {
//    rdlock();
    iterate(it, 0, 0, active->getWidth()-1, active->getHeight()-1);
//    unlock();
}

// private service methods

void MLife::prepareTurn() {
    size_t hblocks = active->getBlocksWidth();
    size_t vblocks = active->getBlocksHeight();
    int modleft = 0,
        modright = 0,
        modtop = 0,
        modbottom = 0;

    // test top and bottom edges
    for(size_t x = 0;x < hblocks;x++) {
        if(active->getBlock(x, 0) & 0xff)
            modtop = 1;
        if(active->getBlock(x, vblocks - 1) & 0xff00000000000000)
            modbottom = 1;
        if(modtop && modbottom)
            break;
    }
    // test left and right edges
    for(size_t y = 0;y < vblocks;y++) {
        if(active->getBlock(0, y) & 0x0101010101010101)
                modleft = 1;
        if(active->getBlock(hblocks-1, y) & 0x8080808080808080)
                modright = 1;
        if(modleft && modright)
            break;
    }

    if(modleft || modright || modtop || modbottom) {
        active->padBlocks(modleft, modtop, modright, modbottom);
        if(modleft) left += 8;
        if(modtop) top += 8;
    }
    sparse->resizeBlocks(active->getBlocksWidth(), active->getBlocksHeight());
}
