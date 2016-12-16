#ifndef QTLIFE_H
#define QTLIFE_H

#include "qtree.h"

class QTLife {
public:
    QTLife();
    bool burn(int col, int row);
    bool kill(int col, int row);
    bool test(int col, int row);
    std::vector<QPoint> getCells(const QRect& range);

private:
    QTree* tree;

    void expand(const QPoint& p);
};

#endif // QTLIFE_H
