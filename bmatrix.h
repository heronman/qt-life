#ifndef BMATRIX_H
#define BMATRIX_H

#include <QtGlobal>
#include <QPoint>
#include <QRect>
#include <QSize>

#ifndef BM64
#ifndef BM16
#define BM16
#endif
#endif

#ifdef BM64
#define BM_BITSIZE 64
#define BM_BYTESIZE 8
#define BM_TYPE quint64
#define BM_SIZE 8
#elif defined(BM16)
#define BM_BITSIZE 16
#define BM_BYTESIZE 2
#define BM_TYPE quint16
#define BM_SIZE 4
#endif

class BMatrix {
public:
    BMatrix();
    BMatrix(int w, int h);
    ~BMatrix();

    inline int getBlocksWidth() const { return hblocks; }
    inline int getBlocksHeight() const { return vblocks; }
    inline int getWidth() const { return width; }
    inline int getHeight() const { return height; }
    void pad(int left, int top, int right, int bottom);
    void padBlocks(int left, int top, int right, int bottom);
    void resizeBlocks(int wd, int ht);

    bool get(int x, int y) const;
    BM_TYPE getBlock(int x, int y) const;
    bool set(int x, int y, bool v);
    bool setBlock(int x, int y, BM_TYPE v);

    QRect validRect() const;
    QRect validBlocksRect() const;
    inline QSize bounds() const { return QSize(width, height); }

    void clear();
    void zero();
    unsigned long count() const;

    inline BM_TYPE* getBuffer() { return buf; }

private:
    BM_TYPE *buf;
    int width, height; // width and height in bits
    int hblocks, vblocks; // width and height in blocks
};

#endif // BMATRIX_H
