#include <QtGui>
#include <QGridLayout>
#include <QtWidgets/QMessageBox>
#include "mainwindow.h"
//#include "flowlayout.h"

QColor MainWindow::colorGrid, MainWindow::colorLive, MainWindow::colorDead;
QColor MainWindow::colorTrace, MainWindow::colorTraceKill, MainWindow::colorSelected;
QCursor MainWindow::curSelect,
    MainWindow::curMoveReady,
    MainWindow::curMove,
    MainWindow::curCopyReady,
    MainWindow::curCopy,
    MainWindow::curBurn,
    MainWindow::curKill;

int MainWindow::gridMinSize;
int MainWindow::cellSize;
int MainWindow::timeGap;

MainWindow::MainWindow() {
    readSettings();

    installEventFilter(this);

    QWidget *contentPane = new QWidget;
    QGridLayout* gl = new QGridLayout;
    contentPane->setLayout(gl);
    setCentralWidget(contentPane);

    lifeWidget = new LifeWidget(cellSize, timeGap, this);
    lifeWidget->setFocusPolicy(Qt::StrongFocus);
    lifeWidget->setMouseTracking(true);

    //setCentralWidget(lifeWidget);

    leftRuler = new Ruler(Qt::Vertical, cellSize);
    topRuler = new Ruler(Qt::Horizontal, cellSize);

    gl->setMargin(0);
    gl->setSpacing(0);
    gl->addWidget(leftRuler, 1, 0);
    gl->addWidget(topRuler, 0, 1);
    gl->addWidget(lifeWidget, 1, 1);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    setWindowTitle("AGL Life");

    connect(lifeWidget, SIGNAL(activeCellChanged(int,int)),
            this, SLOT(activeCellChanged(int,int)));
    connect(lifeWidget, SIGNAL(lifeLeaved()),
            this, SLOT(lifeLeaved()));
    connect(lifeWidget, SIGNAL(lifeMoved(int,int)),
            this, SLOT(lifeMoved(int,int)));
    connect(lifeWidget, SIGNAL(lifeScaled(int,int,int)),
            this, SLOT(lifeScaled(int,int,int)));
    connect(lifeWidget, SIGNAL(lifeStarted()),
            this, SLOT(lifeStarted()));
    connect(lifeWidget, SIGNAL(lifeStopped()),
            this, SLOT(lifeStopped()));
    connect(lifeWidget, SIGNAL(lifeChanged(int,int)),
            this, SLOT(lifeChanged(int,int)));
}

void MainWindow::lifeChanged(int population, int generation) {
    labelGeneration->setText(QString().sprintf("%d", generation));
    labelPopulation->setText(QString().sprintf("%d", population));
}

void MainWindow::activeCellChanged(int col, int row) {
    QString s;
    labelCell->setText(s.sprintf("%d:%d", col, row));
}

void MainWindow::lifeMoved(int left, int top) {
    QString s;
    //labelPos->setText(s.sprintf("%d:%d", left, top));
    topRuler->setStart(left);
    topRuler->update();
    leftRuler->setStart(top);
    leftRuler->update();
}

void MainWindow::lifeScaled(int left, int top, int cellSize) {
    //labelPos->setText(s.sprintf("%d:%d", left, top));
    labelCellSize->setText(QString().sprintf("%d", cellSize));
    topRuler->setUnitSize(cellSize);
    topRuler->setStart(left);
    topRuler->update();
    leftRuler->setUnitSize(cellSize);
    leftRuler->setStart(top);
    leftRuler->update();
}

void MainWindow::lifeLeaved() {
    labelCell->setText("");
}

void MainWindow::lifeStarted() {
    stopAct->setEnabled(true);
    stopAct->setVisible(true);
    startAct->setEnabled(false);
    startAct->setVisible(false);
}

void MainWindow::lifeStopped() {
    stopAct->setEnabled(false);
    stopAct->setVisible(false);
    startAct->setEnabled(true);
    startAct->setVisible(true);
}

void MainWindow::closeEvent(QCloseEvent *) {
    writeSettings();
}

QLabel* addFramedLabel(QStatusBar* parent, const QString& title, const QString& initValue, int width = 0) {
    QFrame* frame = new QFrame;
    QHBoxLayout* layout = new QHBoxLayout;
    QLabel* label = new QLabel(initValue, frame);
    frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    layout->addWidget(new QLabel(title, frame));
    layout->addWidget(label);
    frame->setLayout(layout);
    parent->addWidget(frame);
    if(width > 0) label->setMinimumWidth(width);
    else label->adjustSize();
    return label;
}

void MainWindow::createStatusBar() {
    QFontMetrics fm = fontMetrics();

    labelCell = addFramedLabel(statusBar(), "Current cell:", "", 80);
    labelCellSize = addFramedLabel(statusBar(), "Cell size:", "10", 20);
//    labelPos = createFramedLabel(statusBar(), "FramePos:", "0:0", fm.width(QString('9', 9)));
    labelGeneration = addFramedLabel(statusBar(), "Generation:", "0", 50);
    labelPopulation = addFramedLabel(statusBar(), "Population:", "0", 50);

//    labelCell->setFixedWidth(fm.width(QString('9', 9)));
//    labelCellSize->setFixedWidth(fm.width(QString('9', 2)));
//    labelGeneration->setFixedWidth(fm.width(QString('9', 5)));
//    labelPopulation->setFixedWidth(fm.width(QString('9', 4)));
}

void MainWindow::createActions() {
    newAct = new QAction(QIcon(":/images/document-new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
//    newAct->setStatusTip(tr("Create a new life"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/document-open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
//    openAct->setStatusTip(tr("Open an existing life"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/document-save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
//    saveAct->setStatusTip(tr("Save the life to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
//    saveAsAct->setStatusTip(tr("Save the life under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
//    exitAct->setStatusTip(tr("Exit Life... Oops..."));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    cutAct = new QAction(QIcon(":/images/edit-cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
//    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
//                            "clipboard"));
    //connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/edit-copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
//    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
//                             "clipboard"));
    //connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/edit-paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
//    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
//                              "selection"));
    //connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

    startAct = new QAction(QIcon(":/images/media-playback-start.png"), tr("&Play"), this);
    startAct->setShortcut(QKeySequence("Ctrl+P"));
//    startAct->setStatusTip(tr("Start life cycle"));
    connect(startAct, SIGNAL(triggered()), lifeWidget, SLOT(start()));

    stopAct = new QAction(QIcon(":/images/media-playback-stop.png"), tr("S&top"), this);
    stopAct->setShortcut(QKeySequence("Alt+T"));
//    stopAct->setStatusTip(tr("Stop life cycle"));
    connect(stopAct, SIGNAL(triggered()), lifeWidget, SLOT(stop()));

    stepAct = new QAction(QIcon(":/images/media-skip-forward.png"), tr("St&ep"), this);
    stepAct->setShortcut(QKeySequence("Alt+E"));
//    stepAct->setStatusTip(tr("Make one life step"));
    connect(stepAct, SIGNAL(triggered()), lifeWidget, SLOT(step()));

    aboutAct = new QAction(QIcon(":/images/help-about.png"), tr("&About"), this);
//    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
//    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    moveAct = new QAction(QIcon(":/images/transform-move.png"), tr("Move the Life"), this);
    moveAct->setCheckable(true);
    moveAct->setChecked(false);
    connect(moveAct, SIGNAL(triggered(bool)), this, SLOT(btnMoveTrigger(bool)));
    editAct = new QAction(QIcon(":/images/draw-freehand.png"), tr("Edit the Life"), this);
    editAct->setCheckable(true);
    editAct->setChecked(true);
    connect(editAct, SIGNAL(triggered(bool)), this, SLOT(btnEditTrigger(bool)));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    pasteAct->setEnabled(false);
    stopAct->setEnabled(false);
    stopAct->setVisible(false);
}

void MainWindow::btnEditTrigger(bool t) {
    if(t) {
        lifeWidget->setMode(LifeWidget::M_NONE);
        moveAct->setChecked(false);
    } else {
        lifeWidget->setMode(LifeWidget::M_MOVE);
        moveAct->setChecked(true);
    }
}

void MainWindow::btnMoveTrigger(bool t) {
    if(t) {
        lifeWidget->setMode(LifeWidget::M_MOVE);
        editAct->setChecked(false);
    } else {
        lifeWidget->setMode(LifeWidget::M_NONE);
        editAct->setChecked(true);
    }
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars() {
    toolBar = addToolBar(tr("Life"));
    toolBar->addAction(newAct);
    toolBar->addAction(openAct);
    toolBar->addAction(saveAct);

    toolBar->addSeparator();
    toolBar->addAction(cutAct);
    toolBar->addAction(copyAct);
    toolBar->addAction(pasteAct);

    toolBar->addSeparator();
    toolBar->addAction(moveAct);
    toolBar->addAction(editAct);
    toolBar->setMovable(false);

    toolBar->addSeparator();
    toolBar->addAction(startAct);
    toolBar->addAction(stopAct);
    toolBar->addAction(stepAct);

//    fileToolBar->setStyleSheet("QToolBar { border-bottom: 1px solid darkgrey; }");
//    editToolBar->setStyleSheet("QToolBar { border-bottom: 1px solid darkgrey; }");
}

void MainWindow::newFile() {
    lifeWidget->clear();
}

void MainWindow::open() {
}

bool MainWindow::save() {
    return false;
}

bool MainWindow::saveAs() {
    return false;
}

void MainWindow::about() {
   QMessageBox::about(this, tr("About AG-L Life"),
                      tr("The <b>AG-L Life</b> is an implementation of Convey's Game of Life "
               "written using Qt, with powerfull tools to investigate secrets of Life."));
}

void MainWindow::readSettings() {
    QSettings settings("AG-L", "Life");

    colorGrid = QColor(settings.value("colorGrid", QString("#001E3C")).toString());
    colorLive = QColor(settings.value("colorLive", QString("#0064B4")).toString());
    colorDead = QColor(settings.value("colorDead", QString("#000")).toString());

    colorTrace = QColor(settings.value("colorTrace", QString("#FFFF00")).toString());
    colorTrace.setAlpha(settings.value("colorTraceAlpha", 128).toInt());
    colorTraceKill = QColor(settings.value("colorTraceKill", QString("#FF0000")).toString());
    colorTraceKill.setAlpha(settings.value("colorTraceKillAlpha", 128).toInt());
    colorSelected = QColor(settings.value("colorSelected", QString("#008080")).toString());
    colorSelected.setAlpha(settings.value("colorSelectedAlpha", 85).toInt());

    gridMinSize = settings.value("gridMinSize", 3).toInt();
    cellSize = settings.value("cellSize", 10).toInt();
    timeGap = settings.value("timeGap", 100).toInt();

//    curSelect = Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR);
    curMoveReady = *new QCursor(*new QPixmap(":/cursors/openhand.gif"), 6, 3);
    curMove = *new QCursor(*new QPixmap(":/cursors/closedhand.gif"), 8, 2);
    curCopyReady = *new QCursor(*new QPixmap(":/cursors/openhand-plus.gif"), 6, 3);
    curCopy = *new QCursor(*new QPixmap(":/cursors/closedhand-plus.gif"), 8, 2);
    curBurn = *new QCursor(*new QPixmap(":/cursors/burn.gif"), 8, 8);
    curKill = *new QCursor(*new QPixmap(":/cursors/kill.gif"), 8, 8);

    QPoint position = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();

    resize(size);
    QMainWindow::move(position);
}

void MainWindow::writeSettings() {
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
    settings.setValue("pos", pos());
    settings.setValue("size", size());

}
