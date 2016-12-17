/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

#include <QWidget>
#include <QtGui>
#include "LifeBase.h"

class LifeWidget : public QWidget {
    Q_OBJECT

public slots:
    void step();
    void turn();
    void start();
    void stop();
    void clear();
    void delayTimerChange(int value);

signals:
//    void nextGeneration(int geneation, int population);
//    void populationChanged(int population);
//    void currentCellChanged(QPoint cell);
    void copyAvailable(bool av);
    void activeCellChanged(int col, int row);
    void lifeLeaved();
    void lifeMoved(int left, int top);
    void lifeScaled(int left, int top, int cellSize);
    void lifeStarted();
    void lifeStopped();
    void lifeChanged(int population, int generation);

public:
    enum Mode { M_NONE, M_DRAG, M_BURN, M_KILL, M_INVERSE, M_MOVE };
    LifeWidget(int cellSize, long timegap, QWidget *parent = 0);
    static QColor colorFusion(const QColor& bg, const QColor& overlay);

    inline long getTimeGap(void) { return timegap; }
    inline void setTimeGap(long t) { timegap = t; }

    inline int getPopulation() { return life->population(); }
    inline int getGeneration() { return generation; }
    inline const QPoint& getCurrentCell() { return cellCurrent; }
    inline int getCellSize() { return cellSize; }
    inline bool hasMouse() { return mouseOut; }
    inline Mode getMode() { return mode; }
    inline void setMode(Mode m) { mode = m; }

    void openGif(QString fname);
    int getCellX(int col);
    int getCellY(int row);
    int getCol(int x);
    int getRow(int y);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    virtual void enterEvent(QEvent* event);
    virtual void leaveEvent(QEvent* event);
    virtual void wheelEvent(QWheelEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

    QColor getCellColor(const QPoint& cell);
    QColor getCellColor(long x, long y);
    void drawGrid(QPainter* painter);
    void drawCells(QPainter* painter, long l, long t, long r, long b);
    void drawCell(QPainter* painter, const QPoint& cell);
    void drawCell(QPainter* painter, const QPoint& cell, const QColor& color);
    void drawCell(QPainter* painter, long x, long y);
    void drawCell(QPainter* painter, long x, long y, const QColor& color);

private:
    Mode mode;
    QPoint mouseCurrent;//, mouseOld;
    QPoint cellCurrent;//, cellOld;
    bool mouseOut;//, mouseOldOut;
//    bool drag, burn, kill;

    QTimer* timer;
    LifeBase *life;
    int cellSize;
    int left, top;
    long timegap;
    long generation;
    bool grid;
    int keyPressed = 0;

    void adjustCursor();

    inline void repaintCurrentCell() { update(getCellX(cellCurrent.x()), getCellY(cellCurrent.y()), cellSize, cellSize); }

friend class DrawCellConsumer;
};

#endif
