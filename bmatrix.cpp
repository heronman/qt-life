#include <string.h>
#include <math.h>
#include "bmatrix.h"

#include <iostream>
using namespace std;

#ifdef BM64
#define _V55 0x5555555555555555
#define _V33 0x3333333333333333
#define _V01 0x0101010101010101
#define _V0f 0x0f0f0f0f0f0f0f0f
#define _RSHIFT 56
#elif defined(BM16)
#define _V55 0x5555
#define _V33 0x3333
#define _V01 0x0101
#define _V0f 0x0f0f
#define _RSHIFT 8
#endif

int bpop(BM_TYPE v) {
    v = v - ((v >> 1) & _V55);
    v = (v & _V33) + ((v >> 2) & _V33);
    v = (v + (v >> 4)) & _V0f;
    return (BM_TYPE)(v * _V01) >> _RSHIFT;
}

BMatrix::BMatrix() {
    width = height = BM_SIZE;
    hblocks = vblocks = 1;
    buf = new BM_TYPE[1];
    *buf = (BM_TYPE)0;
}

BMatrix::BMatrix(int w, int h) {
    hblocks = ceil((double)w / BM_SIZE);
    vblocks = ceil((double)h / BM_SIZE);
    width = hblocks * BM_SIZE;
    height = vblocks * BM_SIZE;
    buf = new BM_TYPE[hblocks * vblocks];
    memset(buf, 0, hblocks * vblocks * sizeof(BM_TYPE));
}

BMatrix::~BMatrix() {
    delete buf;
}

BM_TYPE BMatrix::getBlock(int x, int y) const {
    if(x < 0 || y < 0 || x >= hblocks || y >= vblocks) return 0;
    return buf[hblocks * y + x];
}

bool BMatrix::get(int x, int y) const {
    int bitIndex = (y % BM_SIZE) * BM_SIZE + (x % BM_SIZE);
    return getBlock(x / BM_SIZE, y / BM_SIZE)
            & (((BM_TYPE)1) << bitIndex);
//    if(x < 0 || y < 0 || x >= width || y >= height) return false;
//    int blockIndex = y / BM_SIZE * hblocks + x / BM_SIZE;
//    int bitIndex = (y % BM_SIZE) * BM_SIZE + (x % BM_SIZE);
//    return buf[blockIndex] & (((BM_TYPE)1) << bitIndex);
}

bool BMatrix::set(int x, int y, bool v) {
    if(x < 0 || y < 0 || x >= width || y >= height) return false;
    int bx = x / BM_SIZE, by = y / BM_SIZE;
    int blockIndex = by * hblocks + bx;
    int bitIndex = (y % BM_SIZE) * BM_SIZE + (x % BM_SIZE);

    if(v) {
        buf[blockIndex] |= (((BM_TYPE)1) << bitIndex);
    } else {
        buf[blockIndex] &= ~(((BM_TYPE)1) << bitIndex);
    }
    return true;
}

bool BMatrix::setBlock(int x, int y, BM_TYPE v) {
    if(x < 0 || y < 0 || x >= hblocks || y >= vblocks) return false;
    buf[hblocks * y + x] = v;
    return true;
}

QRect BMatrix::validRect() const {
    bool t = false;
    QRect r(0, 0, 0, 0); // init an empty rect

    for(int y = 0;y < vblocks;y++) {
        for(int x = 0;x < hblocks;x++) {
            quint16 v = buf[y * hblocks + x];
            if(v) {
                for(int i = 0;i < 4;i++) {
                    if((v >> (i * 4)) & 0x000f) {
                        r.setTop(y * 4 + i);
                        r.setBottom(y * 4 + i);
                        break;
                    }
                }
                t = true;
                break;
            }
        }
        if(t) break;
    }
    if(!t) {
        return r;
    }
    t = false;
    for(int y = vblocks-1;y >= 0;y--) {
        for(int x = 0;x < hblocks;x++) {
            quint16 v = buf[y * hblocks + x];
            if(v) {
                for(int i = 3;i >= 0;i--) {
                    if((v >> (i * 4)) & 0x000f) {
                        r.setBottom(y * 4 + i);
                        break;
                    }
                }
                t = true;
                break;
            }
        }
        if(t) break;
    }
    t = false;
    for(int x = 0;x < hblocks;x++) {
        for(int y = 0;y < vblocks;y++) {
            quint16 v = buf[y * hblocks + x];
            if(v) {
                for(int i = 0;i < 4;i++) {
                    if(v & (0x1111 << i)) {
                        r.setLeft(x * 4 + i);
                        r.setRight(x * 4 + i);
                        break;
                    }
                }
                t = true;
                break;
            }
        }
        if(t) break;
    }
    t = false;
    for(int x = hblocks - 1;x >= 0;x--) {
        for(int y = 0;y < vblocks;y++) {
            quint16 v = buf[y * hblocks + x];
            if(v) {
                for(int i = 3;i >= 0;i--) {
                    if(v & (0x1111 << i)) {
                        r.setRight(x * 4 + i);
                        break;
                    }
                }
                t = true;
                break;
            }
        }
        if(t) break;
    }
    return r;
}

QRect BMatrix::validBlocksRect() const {
    bool t = false;
    QRect r(0, 0, 0, 0); // init an empty rect

    for(int y = 0;y < vblocks;y++) {
        for(int x = 0;x < hblocks;x++) {
            if(buf[y * hblocks + x]) {
                t = true;
                r.setTop(y);
                r.setBottom(y);
                break;
            }
        }
        if(t) break;
    }
    if(!t) {
        return r;
    }
    t = false;
    for(int y = vblocks-1;y > r.bottom();y--) {
        for(int x = 0;x < hblocks;x++) {
            if(buf[y * hblocks + x]) {
                t = true;
                r.setBottom(y);
                break;
            }
        }
        if(t) break;
    }
    t = false;
    for(int x = 0;x < hblocks;x++) {
        for(int y = 0;y < vblocks;y++) {
            if(buf[y * hblocks + x]) {
                t = true;
                r.setLeft(x);
                r.setRight(x);
                break;
            }
        }
        if(t) break;
    }
    t = false;
    for(int x = hblocks - 1;x > r.right();x--) {
        for(int y = 0;y < vblocks;y++) {
            if(buf[y * hblocks + x - 1]) {
                t = true;
                r.setRight(x);
                break;
            }
        }
        if(t) break;
    }
    return r;
}

void BMatrix::pad(int left, int top, int right, int bottom) {
    padBlocks((int)ceil((double)left / BM_SIZE),
                 (int)ceil((double)top / BM_SIZE),
                 (int)ceil((double)right / BM_SIZE),
                 (int)ceil((double)bottom / BM_SIZE));
}

void BMatrix::padBlocks(int left, int top, int right, int bottom) {
    if(!(left | top | right | bottom)) return;

    int wd = hblocks + left + right;
    int ht = vblocks + top + bottom;

    BM_TYPE *newbuf = new BM_TYPE[wd * ht];
    memset(newbuf, 0, wd * ht * sizeof(BM_TYPE));

    int yStart = top < 0 ? -top : 0;
    int yEnd = vblocks + (bottom < 0 ? bottom : 0);
    int xStart = left < 0 ? -left : 0;
    int xEnd = hblocks + (right < 0 ? right : 0);
    int cpwd = sizeof(BM_TYPE) * (xEnd - xStart);

    for(int y = yStart;y < yEnd;y++) {
        memcpy(newbuf + wd * (y + top) + (left > 0 ? left : 0),
               buf + y * hblocks + xStart,
               cpwd);
    }

    hblocks = wd;
    vblocks = ht;
    width = hblocks * BM_SIZE;
    height = vblocks * BM_SIZE;
    delete buf;
    buf = newbuf;
}

void BMatrix::resizeBlocks(int wd, int ht) {
    if(!wd || !ht) return;
    int newsize = wd * ht;
    int oldsize = hblocks * vblocks;
    if(newsize != oldsize) {
        delete buf;
        buf = new BM_TYPE[newsize];
    }
    hblocks = wd;
    vblocks = ht;
    width = hblocks * BM_SIZE;
    height = vblocks * BM_SIZE;
    zero();
}

void BMatrix::clear() {
    resizeBlocks(1, 1);
}

void BMatrix::zero() {
    memset(buf, 0, hblocks * vblocks * sizeof(BM_TYPE));
}

unsigned long BMatrix::count() const {
    unsigned long cnt = 0;
    BM_TYPE *end = buf + hblocks * vblocks;
    for(BM_TYPE *p = buf;p != end;p++)
        cnt += bpop(*p);
    return cnt;
}
