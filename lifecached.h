#ifndef LIFECACHED_H
#define LIFECACHED_H

#include "LifeBase.h"

#define BM16
#include "bmatrix.h"

class LifeCached : public LifeBase {
private:
    unsigned char *cache;
    BMatrix m1, m2, *active, *sparse;
    int left, top;
    int fBurn, fSurviveMin, fSurviveMax;

    void makePrecache();
    unsigned char computeLife(u_int16_t v) const;

    static quint16 uniteH(quint16 left, quint16 right);
    static quint16 uniteV(quint16 left, quint16 right);
    static quint16 unite(quint16 l, quint16 t, quint16 r, quint16 b);
    void init(int fBurn, int fSurviveMin, int fSurviveMax);
    void turn();

public:
    LifeCached();
    LifeCached(int fBurn, int fSurviveMin, int fSurviveMax);
    ~LifeCached();

    void setFormula(int fBurn, int fSurviveMin, int fSurviveMax);
    const int *getFormula();
    bool test(int x, int y) const;
    void burn(int x, int y);
    void kill(int x, int y);
    unsigned long population() const;
    void clear();
    void step();

    void iterate(LifeCellConsumer* it);
    void iterate(LifeCellConsumer* it, int l, int t, int r, int b);

    inline BMatrix *getActive() { return active; }
    inline BMatrix *getSparse() { return sparse; }
};

#endif // LIFECACHED_H
