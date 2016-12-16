#ifndef MLIFE_H
#define MLIFE_H

#include "LifeBase.h"
#include "bmatrix.h"

class MLife : public LifeBase {
private:
    BMatrix m1, m2, *active, *sparse;
    long left, top;
    size_t alive;

    void prepareTurn();

public:
    MLife();
    ~MLife();

    bool test(long x, long y) const;
    void burn(long x, long y);
    void kill(long x, long y);
    unsigned long population() const;
    void clear();
    void step();

    void iterate(LifeCellConsumer* it);
    void iterate(LifeCellConsumer* it, long l, long t, long r, long b);

    inline BMatrix* getActive() { return active; }
    inline BMatrix* getSparse() { return sparse; }
};

#endif // MLIFE_H
