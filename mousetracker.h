#ifndef MOUSETRACKER_H
#define MOUSETRACKER_H

#include <QPoint>

class MouseTracker {
public:
    MouseTracker();
    QPoint mouse, oldMouse;
    QPoint cell, oldCell;

    int mouseDiffX();
    int mouseDiffY();
    int colDiff();
    int rowDiff();
};

#endif // MOUSETRACKER_H
