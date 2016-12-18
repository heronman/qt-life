#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include "LifeWidget.h"
#include "ruler.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    // app settings
    static QColor colorGrid, colorLive, colorDead;
    static QColor colorTrace, colorTraceKill, colorSelected;
    static QCursor curSelect, curMoveReady, curMove, curCopyReady, curCopy, curBurn, curKill;

    static int gridMinSize;
    static int cellSize;
    static int timeGap;

protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void lifeMoved(int left, int top);
    void lifeScaled(int left, int top, int cellSize);
    void activeCellChanged(int col, int row);
    void lifeLeaved();
    void lifeStarted();
    void lifeStopped();
    void lifeChanged(int population, int generation);
    void btnMoveTrigger(bool);
    void btnEditTrigger(bool);
    inline void readOnlySwitched(bool readOnly) {
        editAct->setChecked(!readOnly);
        moveAct->setChecked(readOnly);
    }

    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();

private:
    void createActions();
    void readSettings();
    void writeSettings();

    void createStatusBar();
    void createMenus();
    void createToolBars();

    LifeWidget *lifeWidget;
    QLabel* labelCell, *labelMouse, *labelPos;
    QLabel *labelCellSize, *labelGeneration, *labelPopulation;
    QString curFile;
    Ruler* leftRuler, *topRuler;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QToolBar *toolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;

    QAction *startAct;
    QAction *stopAct;
    QAction *stepAct;
    QAction *moveAct;
    QAction *editAct;

    QAction *aboutAct;
    QAction *aboutQtAct;

};

#endif // MAINWINDOW_H
