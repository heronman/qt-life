#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(QTLife);
    QApplication app(argc, argv);
    app.setOrganizationName("AG-L.NET");
    app.setApplicationName("Life");
    app.setApplicationVersion("0.001");

    MainWindow mainWin;
    mainWin.setMinimumSize(200, 100);
    mainWin.setWindowIcon(QIcon(":/images/app-icon.png"));
    mainWin.show();

    return app.exec();
}
