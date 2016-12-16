#include <QSettings>
#include "lifesettings.h"

QColor LifeSettings::colorGrid(0, 60, 0);
QColor LifeSettings::colorLive(0, 180, 0);
QColor LifeSettings::colorDead(0, 0, 0);

QColor LifeSettings::colorTrace(255, 255, 0, 128);
QColor LifeSettings::colorTraceKill(255, 0, 0, 128);
QColor LifeSettings::colorSelected(0, 180, 180, 128);
//QColor LifeSettings::colorSelection(0, 128, 128, 85);
//QColor LifeSettings::colorSelectionFrame(0, 180, 180, 128);

int LifeSettings::cellSize = 10;
long LifeSettings::timeGap = 100l;
QPoint LifeSettings::position(200, 200);
QSize LifeSettings::size(400, 400);

void LifeSettings::readSettings() {
    QSettings settings("AG-L", "Life");

    colorGrid = QColor(settings.value("colorGrid", QString("#003C00")).toString());
    colorLive = QColor(settings.value("colorLive", QString("#00B400")).toString());
    colorDead = QColor(settings.value("colorDead", QString("#000")).toString());

    colorTrace = QColor(settings.value("colorTrace", QString("#FFFF00")).toString());
    colorTrace.setAlpha(settings.value("colorTraceAlpha", 128).toInt());
    colorTraceKill = QColor(settings.value("colorTraceKill", QString("#FF0000")).toString());
    colorTraceKill.setAlpha(settings.value("colorTraceKillAlpha", 128).toInt());
    colorSelected = QColor(settings.value("colorSelected", QString("#008080")).toString());
    colorSelected.setAlpha(settings.value("colorSelectedAlpha", 85).toInt());

    cellSize = settings.value("cellSize", 10).toInt();
    timeGap = settings.value("timeGap", 100).toInt();

    position = settings.value("pos", QPoint(200, 200)).toPoint();
    size = settings.value("size", QSize(400, 400)).toSize();
}

void LifeSettings::writeSettings() {
    QSettings settings("AG-L", "Life");

    settings.setValue("colorGrid", colorGrid.name());
    settings.setValue("colorLive", colorLive.name());
    settings.setValue("colorDead", colorDead.name());

    settings.setValue("colorTrace", colorTrace.name());
    settings.setValue("colorTraceAlpha", colorTrace.alpha());
    settings.setValue("colorTraceKill", colorTraceKill.name());
    settings.setValue("colorTraceKillAlpha", colorTraceKill.alpha());
    settings.setValue("colorSelected", colorSelected.name());
    settings.setValue("colorSelectedAlpha", colorSelected.alpha());

    settings.setValue("cellSize", cellSize);
    settings.setValue("timeGap", timeGap);
    settings.setValue("pos", );
    settings.setValue("size", );

}
