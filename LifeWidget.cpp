#include <QtDebug>
#include <typeinfo>
#include "life.h"
#include "LifeWidget.h"
#include "mainwindow.h"

LifeWidget::LifeWidget(int cellSize, long timeGap, QWidget *parent)
    : QWidget(parent)

{    
    mouseOut = true;
//    drag = burn = kill = false;
    mode = M_NONE;

    timer = NULL;
    generation = 0l;
//    life = new MLife();
    life = new Life();
    grid = cellSize < 2;

    life->burn(35, 25);
    life->burn(36, 25);
    life->burn(37, 25);
    life->burn(35, 26);
    life->burn(35, 27);
    life->burn(37, 26);
    life->burn(37, 27);

    timegap = timeGap;
    this->cellSize = cellSize;
    left = top = 0;

    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);
    setFocus();
    adjustCursor();
}

void LifeWidget::adjustCursor() {
    QCursor cursor = MainWindow::curBurn;
    switch(mode) {
        case M_NONE:
        if(keyPressed == Qt::Key_Space)
            cursor = MainWindow::curMoveReady;
        else if(keyPressed == Qt::Key_Control)
            cursor = MainWindow::curSelect;
        break;

        case M_DRAG: // case M_MOVE_SELECTED:
        cursor = MainWindow::curMove;
        break;

//        case M_COPY_SELECTED:
//        cursor = MainWindow::curCopy;
//        break;

        case M_KILL:
        cursor = MainWindow::curKill;
        break;

    }

    setCursor(cursor);
}

void LifeWidget::keyPressEvent(QKeyEvent *event) {
    if(!event->isAutoRepeat() && keyPressed == 0 && mode == M_NONE) {
//        qDebug("Key pressed: %u", event->key());
        keyPressed = event->key();
        repaintCurrentCell();
        adjustCursor();
        event->accept();
    } else event->ignore();
}

void LifeWidget::keyReleaseEvent(QKeyEvent *event) {
    if(!event->isAutoRepeat() && keyPressed == event->key()) {
//        qDebug("Key released: %u", event->key());
        repaintCurrentCell();
        keyPressed = 0;
        if(mode == M_NONE)
            adjustCursor();
        event->accept();
    } else event->ignore();
}

void LifeWidget::paintEvent(QPaintEvent *) {
//    qint64 tstart = QDateTime::currentMSecsSinceEpoch();
    QPainter painter(this);

    QRegion clipRegion = painter.clipRegion();
    QRect bounds(0, 0, width(), height());
    if(clipRegion.isEmpty()) {
        painter.setClipRegion(QRegion(bounds));
//        qDebug() << "Clip region is empty. Drawing a whole region\n";
    } else {
        bounds = clipRegion.boundingRect();
//        qDebug() << "Clip region: " << bounds.left() << ", " << bounds.top() << "; " << bounds.width() << "x" << bounds.height() << "\n";
    }

    painter.fillRect(bounds, MainWindow::colorDead);
    if(cellSize >= MainWindow::gridMinSize)
        drawGrid(&painter);

    int topRow = getRow(bounds.top());
    int bottomRow = getRow(bounds.bottom());
    int leftCol = getCol(bounds.left());
    int rightCol = getCol(bounds.right());

    drawCells(&painter, leftCol, topRow, rightCol, bottomRow);
    if(!mouseOut)
        drawCell(&painter, cellCurrent);

    for(int row = topRow; row <= bottomRow;row++) {
        drawCell(&painter, QPoint(0, row));
    }
    for(int col = leftCol; col <= rightCol;col++) {
        drawCell(&painter, QPoint(col, 0));
    }
//    qint64 tend = QDateTime::currentMSecsSinceEpoch();
//    qDebug("Paint time: %llu msec", tend-tstart);
}

void LifeWidget::mousePressEvent(QMouseEvent* event) {
    if(event->buttons() == Qt::LeftButton) {
        switch(keyPressed) {
            case Qt::Key_Shift: break;

            case Qt::Key_Alt: case Qt::Key_AltGr:
                mode = M_INVERSE;
                if(life->test(cellCurrent.x(), cellCurrent.y()))
                    life->kill(cellCurrent.x(), cellCurrent.y());
                else
                    life->burn(cellCurrent.x(), cellCurrent.y());
                repaintCurrentCell();
                emit lifeChanged(life->population(), generation);
            break;

            case Qt::Key_Space:
                mode = M_DRAG;
                grabMouse();
            break;

            default:
                mode = M_BURN;
                life->burn(cellCurrent.x(), cellCurrent.y());
                repaintCurrentCell();
                emit lifeChanged(life->population(), generation);

        }
    } else if(event->buttons() == Qt::RightButton) {
        mode = M_KILL;
        life->kill(cellCurrent.x(), cellCurrent.y());
        repaintCurrentCell();
        emit lifeChanged(life->population(), generation);
    }
    adjustCursor();
    event->accept();
}

void LifeWidget::mouseReleaseEvent(QMouseEvent* event) {
    mode = M_NONE;
    adjustCursor();
    event->accept();
    releaseMouse();
}

void LifeWidget::mouseMoveEvent(QMouseEvent* event) {
    event->accept();

    if(mouseCurrent.x() == event->x()
            && mouseCurrent.y() == event->y()
    ) return;

    QPoint oldMouse(mouseCurrent);
    mouseCurrent = event->pos();

    if(mode == M_DRAG) {
        left += oldMouse.x() - event->x();
        top += oldMouse.y() - event->y();
        emit lifeMoved(left, top);
        update();
        return;
    }

    QPoint oldCell(cellCurrent);
    cellCurrent.setX(getCol(event->x()));
    cellCurrent.setY(getRow(event->y()));
    if(cellCurrent.x() == oldCell.x()
            && cellCurrent.y() == oldCell.y())
        return;

    if(mode == M_BURN) {
        life->burn(cellCurrent.x(), cellCurrent.y());
        emit lifeChanged(life->population(), generation);
    } else if(mode == M_KILL) {
        life->kill(cellCurrent.x(), cellCurrent.y());
        emit lifeChanged(life->population(), generation);
    } else if(mode == M_INVERSE) {
        if(life->test(cellCurrent.x(), cellCurrent.y()))
            life->kill(cellCurrent.x(), cellCurrent.y());
        else
            life->burn(cellCurrent.x(), cellCurrent.y());
    }

    update(getCellX(oldCell.x()), getCellY(oldCell.y()), cellSize, cellSize);
    repaintCurrentCell();
    emit activeCellChanged(cellCurrent.x(), cellCurrent.y());
}

void LifeWidget::enterEvent(QEvent* event) {
//    qDebug() << "Mouse enter";
    mouseOut = false;
    event->accept();
//    adjustCursor();
}

void LifeWidget::leaveEvent(QEvent* event) {
//    qDebug() << "Mouse leave";
    mouseOut = true;
    repaintCurrentCell();
//    adjustCursor();
    if(mode != M_DRAG) {
        emit lifeLeaved();
    }
    event->accept();
}

void LifeWidget::wheelEvent(QWheelEvent *event) {
    int newSize = cellSize;
    if(event->delta() < 0) {
        if(newSize > 1) newSize--;
    } else if(event->delta() > 0) {
        if(newSize < 30) newSize++;
    }
    if(newSize == cellSize) return;
//    (double)(event->x() + left) / (double)cellSize

    double q = (double)newSize / (double)cellSize;
    left = round(((double)event->x() + (double)left) * q - (double)event->x());
    top = round(((double)event->y() + (double)top) * q - (double)event->y());
    cellSize = newSize;
    update();
    emit lifeScaled(left, top, cellSize);
    event->accept();
}

void LifeWidget::step() {
    if(timer) {
        stop();
        return;
    }
    turn();
}

void LifeWidget::turn() {
    life->step();
    generation++;
    update();
    emit lifeChanged(life->population(), generation);
}

void LifeWidget::start() {
    if(timer) return;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(turn()));
    timer->start(timegap);
    emit lifeStarted();
}

void LifeWidget::stop() {
    if(!timer) return;
    timer->stop();
    delete timer;
    timer = NULL;
    emit lifeStopped();
}

void LifeWidget::clear() {
    stop();
    life->clear();
    update();
}

void LifeWidget::delayTimerChange(int value) {
    timegap = value;
    if(timer != NULL)
        timer->setInterval(value);
}

QColor LifeWidget::colorFusion(const QColor& bg, const QColor& overlay) {
    return QColor(
        (int)round((overlay.alphaF() * overlay.redF() + bg.redF() * (1.0f - overlay.alphaF())) * 255.0),
        (int)round((overlay.alphaF() * overlay.greenF() + bg.greenF() * (1.0f - overlay.alphaF())) * 255.0),
        (int)round((overlay.alphaF() * overlay.blueF() + bg.blueF() * (1.0f - overlay.alphaF())) * 255.0)
    );
}

void LifeWidget::openGif(QString fname) {
    life->clear();

    int shiftX = 0, shiftY = 1;
    int scaleX = 2, scaleY = 2;

    QMovie *movie = new QMovie(fname);
    movie->jumpToFrame(0);
    QRgb bc = movie->backgroundColor().rgb();

    const QImage img = movie->currentImage();
    for(int y = 0; y < img.height(); y++) {
        for(int x = 0; x < img.width(); x++) {
            QRgb rgb = img.pixel(x, y);
            if(rgb != bc)
                life->burn((x - shiftX) / scaleX, (y - shiftY) / scaleY);
        }
    }

    delete movie;
}

int LifeWidget::getCellX(int col) {
    return cellSize * col - left;
}

int LifeWidget::getCellY(int row) {
    return cellSize * row - top;
}

int LifeWidget::getCol(int x) {
    return (x + left - (x+left < 0 ? cellSize : 0)) / cellSize;
}

int LifeWidget::getRow(int y) {
    return (y + top - (y+top < 0 ? cellSize : 0))/ cellSize;
}

void LifeWidget::drawGrid(QPainter* painter) {
    painter->save();
    painter->setBrush(Qt::NoBrush);
    painter->setPen(MainWindow::colorGrid);

    QRect clipRect = painter->clipRegion().boundingRect();

    int topRow = getRow(clipRect.top());
    int bottomRow = getRow(clipRect.bottom());
    int leftCol = getCol(clipRect.left());
    int rightCol = getCol(clipRect.right());

    for(int row = topRow; row <= bottomRow;row++) {
        int y = getCellY(row + 1) - 1;
//        if(row == -1) painter->setPen(MainWindow::colorGrid.lighter(200));
        painter->drawLine(clipRect.left(), y, clipRect.right(), y);
//        if(row == -1) painter->setPen(MainWindow::colorGrid.darker(200));
    }
    for(int col = leftCol; col <= rightCol;col++) {
        int x = getCellX(col + 1) - 1;
//        if(col == -1) painter->setPen(MainWindow::colorGrid.lighter(200));
        painter->drawLine(x, clipRect.top(), x, clipRect.bottom());
//        if(col == -1) painter->setPen(MainWindow::colorGrid.darker(200));
    }

    painter->restore();
}

QColor LifeWidget::getCellColor(const QPoint& cell) {
    return getCellColor(cell.x(), cell.y());
}

QColor LifeWidget::getCellColor(long x, long y) {
    bool alive = life->test(x, y);
    QColor color = alive
            ? MainWindow::colorLive
            : MainWindow::colorDead;
    if(!mouseOut
            && x == cellCurrent.x()
            && y == cellCurrent.y()
    ) color = colorFusion(color, MainWindow::colorTrace);
    if(x == 0 || y == 0)
        color = colorFusion(color, QColor(60, 60, 60, 128));
    return color;
}

void LifeWidget::drawCell(QPainter* painter, const QPoint& cell) {
    drawCell(painter, cell.x(), cell.y(), getCellColor(cell));
}

void LifeWidget::drawCell(QPainter* painter, long x, long y) {
    drawCell(painter, x, y, getCellColor(x, y));
}

void LifeWidget::drawCell(QPainter* painter, const QPoint& cell, const QColor& color) {
    drawCell(painter, cell.x(), cell.y(), color);
}

void LifeWidget::drawCell(QPainter* painter, long x, long y, const QColor& color) {
    int cs = cellSize;
    if(cs > 2) cs--;
    QRect cellRect(getCellX(x), getCellY(y), cs, cs);
    QRect clipRect = painter->clipRegion().boundingRect();
    if(clipRect.intersects(cellRect)) {
        painter->setBrush(color);
        painter->setPen(Qt::NoPen);
        painter->fillRect(cellRect, color);
    }
}

class DrawCellConsumer : public LifeCellConsumer {
private:
    LifeWidget *widget;
    QPainter *painter;
//    long left, top, right, bottom;
public:
    DrawCellConsumer(LifeWidget *w, QPainter *p);
    void run(long x, long y, bool alive);
};

DrawCellConsumer::DrawCellConsumer(LifeWidget *w, QPainter *p) {
    widget = w;
    painter = p;
}

void DrawCellConsumer::run(long x, long y, bool alive) {
        widget->drawCell(painter, x, y);
}

void LifeWidget::drawCells(QPainter* painter, long left, long top, long right, long bottom) {
    DrawCellConsumer it(this, painter);
    life->iterate(&it, left, top, right, bottom);
}
