#ifndef QTREE_H
#define QTREE_H

#include <QRect>
#include <QPoint>
#include <vector>

class QTree {
public:
    QTree(const QPoint& hook, size_t size);

    inline const QRect getBounds() const;

    bool insert(const QPoint& p);
    bool remove(const QPoint& p);
    std::vector<QPoint> queryRange(const QRect& range) const;
    bool test(const QPoint& p) const;
    inline void setNW(QTree* t) { subquads[0] = t; }
    inline void setNE(QTree* t) { subquads[1] = t; }
    inline void setSW(QTree* t) { subquads[2] = t; }
    inline void setSE(QTree* t) { subquads[3] = t; }
    int count();

    bool contains(int col, int row) const;
    bool contains(const QPoint& p) const;
    bool intersects(int l, int t, int wd, int ht) const;
    bool intersects(const QRect& r) const;

private:
    QTree** subquads;
    QPoint lt;
    size_t size;
    qint64 index;

    void subdivide();
};

#endif // QTREE_H
