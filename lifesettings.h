#ifndef LIFESETTINGS_H
#define LIFESETTINGS_H

#include <QColor>
#include <QPoint>
#include <QSize>

class LifeSettings {
public:
    static QColor colorGrid, colorLive, colorDead;
    static QColor colorTrace, colorTraceKill, colorSelected;

    static int cellSize;
    static long timeGap;

    static QPoint position;
    static QSize size;

    static void readSettings();
};

#endif // LIFESETTINGS_H
