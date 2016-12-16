#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QAction>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QStatusBar>
#include "LifeWidget.h"
#include "ruler.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
//class MainWindow : public QWidget {
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

    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
//    void documentWasModified();

private:
    void createActions();
    void readSettings();
    void writeSettings();

    void createStatusBar();
    void createMenus();
    void createToolBars();
    /*
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
*/
//    LifeWidget::Mode mode;
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
