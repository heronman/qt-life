#include <sys/types.h>
#include <vector>

class LifeCell {
public:
	int col, row;
	LifeCell(int col, int row);
	LifeCell(const LifeCell& cell);
	LifeCell();
};

class LifeRect {
public:
	int left, top;
	size_t width, height;
	LifeRect(int l, int t, size_t wd, size_t ht);
	LifeRect(const LifeRect& bounds);
	LifeRect();

	int right() const;
	int bottom() const;
	void setRight(int x);
	void setBottom(int y);
	bool contains(const LifeCell& cell) const;
	bool contains(int x, int y) const;
	bool intersects(int x, int y, size_t wd, size_t ht) const;
	bool intersects(const LifeRect& b) const;
};

class AbstractLife {
public:
	virtual bool burn(int col, int row) = 0;
	virtual bool kill(int col, int row) = 0;
	virtual bool test(int col, int row) const = 0;
	virtual size_t population() const = 0;
	virtual std::vector<LifeCell>* exportCells() const = 0;
};
