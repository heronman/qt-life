#ifndef RULER_H
#define RULER_H

#include <QWidget>
#include <QtGui>

class Ruler : public QWidget {
    Q_OBJECT

public:
    Ruler(Qt::Orientation orientation, int unitSize, QWidget* parent = NULL);
    void setUnitSize(int newSize);
    void setStart(int newStart);
    int getUnitSize() { return unitSize; }
    int getStart() { return start; }

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);
    int rulerSize();
    void drawMark(QPainter* painter, int pos, int size);
    void drawLabel(QPainter* painter, int unit, const QString& text, int pe, int ph);

private:
    Qt::Orientation orientation;
    int start;
    int unitSize;
    QColor colorBackground;
    QColor colorLabel;
    QColor colorMark;
};

#endif // RULER_H
