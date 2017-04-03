#ifndef LIFE_H
#define LIFE_H

#define LIFE_CAP_DELTA 65536
#define LIFE_FORMULA_BURN 3
#define LIFE_FORMULA_SURVIVE_MIN 2
#define LIFE_FORMULA_SURVIVE_MAX 3

#include <QReadWriteLock>
#include <QPoint>
#include <QMap>
#include <vector>
#include "LifeBase.h"

typedef QMap<int, bool> LifeRow;
typedef QMap<int, LifeRow*> LifeMap;

class Life : public LifeBase {
private:
    LifeMap *cells;
    QReadWriteLock lock;
    long populationCached;

    void setFormula(int burn, int surviveMin, int surviveMax, bool lock);

public:
    Life();
    Life(int burn, int surviveMin, int surviveMax);

    std::vector<int> getFormula(void);
    void setFormula(int burn, int surviveMin, int surviveMax);

    bool test(long col, long row);
    void burn(long col, long row);
    void kill(long col, long row);
    unsigned long population();
    void clear();
    void step();

    void iterate(LifeCellConsumer *it);
    void iterate(LifeCellConsumer* it, long l, long t, long r, long b);

    void rdlock();
    void unlock();
    LifeMap::const_iterator begin();
    LifeMap::const_iterator end();
    LifeMap* copy(LifeMap* ret = NULL);
};

#endif // LIFE_H
