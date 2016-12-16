#include "ruler.h"
#include <QPainter>

Ruler::Ruler(Qt::Orientation orientation, int unitSize, QWidget* parent) :
    QWidget(parent)
{
    this->unitSize = unitSize;
    this->orientation = orientation;
    if(orientation == Qt::Horizontal)
        setFixedHeight(25);
    else
        setFixedWidth(30);
    start = 0;
}

int Ruler::rulerSize() {
    if(orientation == Qt::Horizontal)
        return width();
    else if(orientation == Qt::Vertical)
        return height();
    return 0;
}

void Ruler::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    int startUnit = start / unitSize;
    int endUnit = (rulerSize() + start) / unitSize;

    painter.save();
    painter.setBrush(Qt::gray);
    painter.fillRect(0, 0, width(), height(), Qt::gray);
    painter.setPen(Qt::darkGray);
    if(orientation == Qt::Horizontal) {
        painter.drawLine(0, 0, width()-1, 0);
    } else if(orientation == Qt::Vertical) {
        painter.drawLine(0, 0, 0, height()-1);
    }
    painter.restore();

    QFont font("Arial", 6);
    QFontMetrics fm(font);
    painter.setFont(font);
    int ph = fm.height();
    int freqV = (ph / unitSize) + 1;

    for(int i = startUnit;i <= endUnit;i++) {
        int size = 5;
        if(i % 10 == 0) size = 10;
        else if(i % 5 == 0) size = 7;

        if(unitSize > 1 || i % 2 == 0)
            drawMark(&painter, i * unitSize + unitSize/2 - start, size);

        QString text;
        text.sprintf("%d", i);
        QString s2(text.size()+1, '9');
        int pw = fm.width(s2);

        int freq;
        if(orientation == Qt::Horizontal) {
            freq = (pw / unitSize) + 1;
        } else if(orientation == Qt::Vertical) {
            freq = freqV;
        }
        if(freq > 20) freq = 30;
        else if(freq > 10) freq = 20;
        else if(freq > 5) freq = 10;
        else if(freq > 1) freq = 5;
        if(i % freq != 0) continue;
        drawLabel(&painter, i, text, pw, ph);
    }
}

void Ruler::drawLabel(QPainter* painter, int unit, const QString& text, int pw, int ph) {
    QRect r;
    int opt;
    if(orientation == Qt::Horizontal) {
        r = QRect(unit * unitSize - start + unitSize/2 - pw/2, 2, pw, height() - 12);
        opt = Qt::AlignCenter | Qt::AlignBottom;
    } else if(orientation == Qt::Vertical) {
        r = QRect(2, unit * unitSize - start + unitSize/2 - ph/2, width() - 15, ph);
        opt = Qt::AlignRight | Qt::AlignVCenter;
    } else return;
    painter->drawText(r, opt, text);
}

void Ruler::drawMark(QPainter* painter, int pos, int size) {
    if(orientation == Qt::Horizontal)
        painter->drawLine(pos, height()-1, pos, height()-1-size);
    else if(orientation == Qt::Vertical)
        painter->drawLine(width()-1, pos, width()-1-size, pos);
}

void Ruler::setUnitSize(int newSize) {
    unitSize = newSize;
}

void Ruler::setStart(int newStart) {
    start = newStart;
}
