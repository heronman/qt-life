#include "qtlife.h"

int nextPow2(unsigned int v) {
    if(!v) return 2;
    v--;
    for(unsigned i = 1;i < sizeof(unsigned int);i *= 2) {
        v |= (v >> i);
    }
    v++;
    return v;
}

QTLife::QTLife() {
    tree = new QTree(QRect(0, 0, 2, 2));

}

bool QTLife::burn(int col, int row) {
    QPoint p(col, row);
    expand(p);
    return tree->insert(p);
}

bool QTLife::kill(int col, int row) {
    return tree->remove(QPoint(col, row));
}

std::vector<QPoint> QTLife::getCells(const QRect &range) {
    return tree->queryRange(range);
}

void QTLife::expand(const QPoint &p) {
    const QRect& bounds = tree->getBounds();
    if(bounds.contains(p))
        return;

    QTree *newTree;
    if(p.x() >= bounds.left()) {
        if(p.y() >= bounds.top()) {
            newTree = new QTree(QRect(bounds.left(), bounds.top(), bounds.width() * 2, bounds.height() * 2));
            newTree->setNW(tree);
        } else {
            newTree = new QTree(QRect(bounds.left(), bounds.top() - bounds.height(), bounds.width() * 2, bounds.height() * 2));
            newTree->setSW(tree);
        }
    } else {
        if(p.y() >= bounds.top()) {
            newTree = new QTree(QRect(bounds.left() - bounds.width(), bounds.top(), bounds.width() * 2, bounds.height() * 2));
            newTree->setNE(tree);
        } else {
            newTree = new QTree(QRect(bounds.left() - bounds.width(), bounds.top() - bounds.height(), bounds.width() * 2, bounds.height() * 2));
            newTree->setSE(tree);
        }
    }
    tree = newTree;
    expand(p);
}

