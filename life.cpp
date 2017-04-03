#include "life.h"
#include <QDateTime>

// internal utils

void clearMap(LifeMap* map) {
    for(LifeMap::iterator it = map->begin(); it != map->end(); it++) {
        delete it.value();
    }
    map->clear();
}

bool test(LifeMap *cells, long col, long row) {
    LifeRow* r = cells->value(row);
    if(r == NULL) return false;
    else return r->contains(col);
}

void burn(LifeMap *cells, long col, long row) {
    LifeRow *r = cells->value(row);
    if(r == NULL) {
        r = new LifeRow();
        cells->insert(row, r);
    }
    r->insert(col, true);
}

void kill(LifeMap *cells, long col, long row) {
    LifeRow* r = cells->value(row);
    if(r != NULL) {
        if(r->size() == 1) {
            if(r->firstKey() == col) {
                cells->remove(row);
                delete r;
            }
        } else {
            r->remove(col);
        }
    }
}

// end of internal utils

Life::Life() : Life(LIFE_FORMULA_BURN, LIFE_FORMULA_SURVIVE_MIN, LIFE_FORMULA_SURVIVE_MAX) {}

Life::Life(int burn, int surviveMin, int surviveMax) {
    setFormula(burn, surviveMin, surviveMax, false);
    cells = new LifeMap();
    populationCached = -1L;
}

std::vector<int> Life::getFormula() {
    std::vector<int> r(3);
    r.push_back(fBurn);
    r.push_back(fSurviveMin);
    r.push_back(fSurviveMax);
    return r;
}

void Life::setFormula(int burn, int surviveMin, int surviveMax, bool doLock) {
    if(doLock) lock.lockForWrite();
    fBurn = burn;
    fSurviveMin = surviveMin;
    fSurviveMax = surviveMax;
    if(doLock) lock.unlock();
}

void Life::setFormula(int burn, int surviveMin, int surviveMax) {
    setFormula(burn, surviveMin, surviveMax, true);
}

bool Life::test(long col, long row) {
    bool ret = false;
    lock.lockForRead();
    ret = ::test(cells, col, row);
    lock.unlock();
    return ret;
}

void Life::burn(long col, long row) {
    lock.lockForWrite();
    populationCached = -1L;
    ::burn(cells, col, row);
    lock.unlock();
}

void Life::kill(long col, long row) {
    lock.lockForWrite();
    populationCached = -1L;
    ::kill(cells, col, row);
    lock.unlock();
}

void Life::clear(void) {
    lock.lockForWrite();
    populationCached = -1L;
    clearMap(cells);
    lock.unlock();
}

int neighborsCount(LifeMap * cells, long col, long row) {
    int n = 0;
    for(long y  = -1;y < 2;y++) {
        for(long x = -1; x < 2; x++) {
            if((x != 0 || y != 0)
                    && test(cells, x + col, y + row))
                n++;
        }
    }
    return n;
}

void Life::step(void) {
    lock.lockForWrite();

    LifeMap* tested = new LifeMap();
    LifeMap* newMap = new LifeMap();

    for(LifeMap::iterator it = cells->begin();it != cells->end();it++) {
        int cellY = it.key();
        for(LifeRow::iterator it2 = (*it)->begin(); it2 != (*it)->end();it2++) {
            int cellX = it2.key();
            if(!::test(tested, cellX, cellY)) {
                ::burn(tested, cellX, cellY);
                int neighbors = 0;
                for(int _y  = -1;_y < 2;_y++) {
                    for(int _x = -1; _x < 2; _x++) {
                        if(_x == 0 && _y == 0) continue;
                        int x = cellX + _x,
                            y = cellY + _y;
                        if(::test(cells, x, y)) {
                            neighbors++;
                        } else if(!::test(tested, x, y)) {
                            ::burn(tested, x, y);
                            int n = neighborsCount(cells, x, y);
                            if(n == fBurn)
                                ::burn(newMap, x, y);
                        }
                    }
                }
                bool alive = ::test(cells, cellX, cellY);
                if(alive && neighbors >= fSurviveMin && neighbors <= fSurviveMax) {
                    ::burn(newMap, cellX, cellY);
                } else if(!alive && neighbors == fBurn) {
                    ::burn(newMap, cellX, cellY);
                }
            }
        }
    }

    clearMap(tested);
    delete tested;
    clearMap(cells);
    delete cells;
    cells = newMap;
    populationCached = -1L;

    lock.unlock();
}

unsigned long Life::population(void) {
    lock.lockForRead();
    if(populationCached < 0L) {
        populationCached = 0L;
        for(LifeMap::iterator it = cells->begin(); it != cells->end(); it++) {
            populationCached += it.value()->size();
        }
    }
    lock.unlock();

    return populationCached;
}

void Life::rdlock() {
    lock.lockForRead();
}

void Life::unlock() {
    lock.unlock();
}

LifeMap::const_iterator Life::begin() {
    return cells->begin();
}

LifeMap::const_iterator Life::end() {
    return cells->end();
}

LifeMap* Life::copy(LifeMap* ret) {
    if(ret == NULL)
        ret = new LifeMap();
//    ret->reserve(cells->size());
    for(LifeMap::iterator it = cells->begin(); it != cells->end(); it++) {
        LifeRow* row = new LifeRow();
//        row->reserve(it.value()->size());
        for(LifeRow::iterator it2 = it.value()->begin(); it2 != it.value()->end(); it2++) {
            row->insert(it2.key(), true);
        }
        ret->insert(it.key(), row);
    }
    return ret;
}

void Life::iterate(LifeCellConsumer *li) {
    rdlock();
    for(LifeMap::const_iterator it = cells->begin(); it != cells->end(); it++) {
        for(LifeRow::const_iterator it2 = it.value()->begin(); it2 != it.value()->end(); it2++) {
            li->run(it2.key(), it.key(), true);
        }
    }
    unlock();
}

void Life::iterate(LifeCellConsumer* li, long left, long top, long right, long bottom) {
    rdlock();
    for(LifeMap::const_iterator it = cells->begin(); it != cells->end(); it++) {
        int y = it.key();
        if(y < top || y > bottom) continue;
        for(LifeRow::const_iterator it2 = it.value()->begin(); it2 != it.value()->end(); it2++) {
            int x = it2.key();
            if(x >= left && x <= right)
                li->run(x, y, true);
        }
    }
    unlock();
}
