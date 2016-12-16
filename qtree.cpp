#include "qtree.h"
#include <string.h>

QTree::QTree(const QPoint& hook, size_t s) : bounds(b) {
    lt = hook;
    size = s;
    points.reserve(4);
    memchr(subquads, 0, sizeof(void*) * 4);
}

bool QTree::insert(const QPoint& p) {
    if(!bounds.contains(p))
        return false;
    std::vector<QPoint>::const_iterator it = points.begin();
    while(it != points.end()) {
        if((*it).x() == p.x() && (*it).y() == p.y())
            return false;
        it++;
    }
    if(points.size() < 4) {
        points.push_back(p);
        return true;
    }
    if(!*subquads) subdivide();
    for(int i = 0;i < 4;i++)
        if(subquads[i]->insert(p))
            return true;

    return false;
}

bool QTree::remove(const QPoint &p) {
    if(!bounds.contains(p))
        return false;

    for(std::vector<QPoint>::iterator it = points.begin();it != points.end();it++) {
        if((*it).x() == p.x() && (*it).y() == p.y()) {
            points.erase(it);
            return true;
        }
    }

    if(!*subquads) return false;
    for(int i = 0;i < 4;i++) {
        if(subquads[i]->remove(p)) {
            return true;
        }
    }

    return false;
}

void QTree::subdivide() {
    if(*subquads) return;

    for(int i = 0;i < 4;i++) {
        QRect(bounds.left() + (i % 2 ? bounds.width() / 2 : 0),
              bounds.top() + (i < 2 ? 0 : bounds.height() / 2),
              bounds.width() / 2,
              bounds.height() / 2
        );
    }
}

std::vector<QPoint> QTree::queryRange(const QRect& range) const {
    std::vector<QPoint> pa;
    if(!bounds.intersects(range))
        return pa;
    std::vector<QPoint>::const_iterator it = points.begin();
    while(it != points.end()) {
        if(range.contains(*it))
            pa.push_back(*it);
    }

    if(!*subquads)
        return pa;

    for(int i = 0;i < 4;i++) {
        std::vector<QPoint> ppa = subquads[i]->queryRange(range);
        pa.insert(pa.end(), ppa.begin(), ppa.end());
    }

    return pa;
}

bool QTree::test(const QPoint &p) const {
    if(!bounds.contains(p))
        return false;
    for(std::vector<QPoint>::const_iterator it = points.begin();it != points.end();it++) {
        if((*it).x() == p.x() && (*it).y() == p.y())
            return true;
    }
    if(!*subquads) return false;
    for(int i = 0;i < 4;i++) {
        if(subquads[i]->test(p)) {
            return true;
        }
    }
    return false;
}

int QTree::count() {
    int n = 0;
    if(*subquads) {
        for(int i = 0;i < 4;i++)
            n += subquads[i]->count();
        return n;
    } else return points.size();
}

bool QTree::contains(int col, int row) const {
    return
            col >= lt.x() && col < lt.x() + size
            && row >= lt.y() && row < lt.y() + size;
}

bool QTree::contains(const QPoint& p) const {
    return contains(p.x(), p.y());
}

bool QTree::intersects(int l, int t, int s) const {
    return std::min(lt.x()+size, l+s) - std::max(lt.x(), l) >= 0
        && std::min(lt.y()+size, t+s) - std::max(lt.y(), t) >= 0;
}
