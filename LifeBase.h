#ifndef LIFEBASE_H
#define LIFEBASE_H

class LifeCellConsumer {
public:
    virtual void run(long x, long y, bool alive) = 0;
};

class LifeBase {
protected:
    int fBurn, fSurviveMin, fSurviveMax;

public:
    virtual bool test(long x, long y) = 0;
    virtual void burn(long x, long y) = 0;
    virtual void kill(long x, long y) = 0;
    virtual unsigned long population() = 0;
    virtual void clear() = 0;
    virtual void step() = 0;

    virtual void iterate(LifeCellConsumer* it) = 0;
    virtual void iterate(LifeCellConsumer* it, long l, long t, long r, long b) = 0;
};

#endif // LIFEBASE_H
