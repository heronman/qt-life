#include <QCoreApplication>

#include <iostream>
#include <../lifecached.h>

using namespace std;

int main(int, char **) {
//    QCoreApplication a(argc, argv);

    LifeCached life;
    BMatrix *active = life.getActive();//, *sparse = life.getSparse();

    QSize s = active->bounds();

//    QRect r = active->validRect();

    cout << "Bounds: " << s.width() << "x" << s.height() << endl;
    int left, top, right, bottom;

    life.burn(-10, -5);
    life.burn(10, 5);
    QRect br = active->validRect();
    br.getCoords(&left, &top, &right, &bottom);
    cout << "Blocks rectangle: " << left << ":" << top << " - " << right << ":" << bottom << endl;

    return 0;
//    return a.exec();
}
