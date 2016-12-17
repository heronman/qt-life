#include "life.h"
#include <QDateTime>

// internal utils

void clearHash(QHash<int, QSet<int> *> * hash) {
    for(QHash<int, QSet<int> *>::iterator it = hash->begin(); it != hash->end(); it++) {
        delete it.value();
    }
    hash->clear();
}

bool test(QHash<int, QSet<int> *> *cells, long col, long row) {
    QSet<int> *r = cells->value(row);
    if(r == NULL) return false;
    else return r->contains(col);
}

void burn(QHash<int, QSet<int> *> *cells, long col, long row) {
    QSet<int> *r = cells->value(row);
    if(r == NULL) {
        r = new QSet<int>();
        cells->insert(row, r);
    }
    r->insert(col);
}

// end of internal utils

void kill(QHash<int, QSet<int> *> *cells, long col, long row) {
    QSet<int> * r = cells->value(row);
    if(r != NULL) {
        if(r->size() == 1) {
            cells->remove(row);
            delete r;
        } else {
            r->remove(col);
        }
    }
}

Life::Life() : Life(LIFE_FORMULA_BURN, LIFE_FORMULA_SURVIVE_MIN, LIFE_FORMULA_SURVIVE_MAX) {}

Life::Life(int burn, int surviveMin, int surviveMax) {
    setFormula(burn, surviveMin, surviveMax, false);
    cells = new QHash<int, QSet<int> *>();
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
    ::burn(cells, col, row);
    lock.unlock();
}

void Life::kill(long col, long row) {
    lock.lockForWrite();
    ::kill(cells, col, row);
    lock.unlock();
}

void Life::clear(void) {
    lock.lockForWrite();
    clearHash(cells);
    lock.unlock();
}

int neighborsCount(QHash<int, QSet<int>*> * cells, long col, long row) {
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

    QHash<int, QSet<int>*>* tested = new QHash<int, QSet<int>*>();
    QHash<int, QSet<int>*>* newMap = new QHash<int, QSet<int>*>();

    for(QHash<int, QSet<int> *>::iterator it = cells->begin();it != cells->end();it++) {
        int cellY = it.key();
        for(QSet<int>::iterator it2 = (*it)->begin(); it2 != (*it)->end();it2++) {
            int cellX = *it2;
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

    clearHash(tested);
    delete tested;
    clearHash(cells);
    delete cells;
    cells = newMap;

    lock.unlock();
}

unsigned long Life::population(void) {
    lock.lockForRead();
    unsigned long size = 0L;
    for(QHash<int, QSet<int>*>::iterator it = cells->begin(); it != cells->end(); it++) {
        size += it.value()->size();
    }
    lock.unlock();
    return size;
}

void Life::rdlock() {
    lock.lockForRead();
}

void Life::unlock() {
    lock.unlock();
}

QHash<int, QSet<int>*>::const_iterator Life::begin() {
    return cells->begin();
}

QHash<int, QSet<int>*>::const_iterator Life::end() {
    return cells->end();
}

QHash<int, QSet<int>*>* Life::copy(QHash<int, QSet<int>*>* ret) {
    if(ret == NULL)
        ret = new QHash<int, QSet<int>*>();
    ret->reserve(cells->size());
    for(QHash<int, QSet<int>*>::iterator it = cells->begin(); it != cells->end(); it++) {
        QSet<int> *row = new QSet<int>();
        row->reserve(it.value()->size());
        for(QSet<int>::iterator it2 = it.value()->begin(); it2 != it.value()->end(); it2++) {
            row->insert(*it2);
        }
        ret->insert(it.key(), row);
    }
    return ret;
}

void Life::iterate(LifeCellConsumer *li) {
    rdlock();
    for(QHash<int, QSet<int> *>::const_iterator it = cells->begin(); it != cells->end(); it++) {
        for(QSet<int>::const_iterator it2 = it.value()->begin(); it2 != it.value()->end(); it2++) {
            li->run(*it2, it.key(), true);
        }
    }
    unlock();
}

void Life::iterate(LifeCellConsumer* li, long left, long top, long right, long bottom) {
    rdlock();
    for(QHash<int, QSet<int> *>::const_iterator it = cells->begin(); it != cells->end(); it++) {
        if(it.key() < top || it.key() > bottom) continue;
        for(QSet<int>::const_iterator it2 = it.value()->begin(); it2 != it.value()->end(); it2++) {
            if(*it2 >= left && *it2 <= right)
                li->run(*it2, it.key(), true);
        }
    }
    unlock();
}
