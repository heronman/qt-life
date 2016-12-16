#include "AbstractLife.h"

LifeCell::LifeCell(int col, int row) {
	this->col = col;
	this->row = row;
}

LifeCell::LifeCell(const LifeCell& cell) {
	col = cell.col;
	row = cell.row;
}

LifeCell::LifeCell() {
	col = row = 0;
}

LifeRect::LifeRect(int l, int t, size_t wd, size_t ht) {
	left = l;
	top = t;
	width = wd;
	height = ht;
}

LifeRect::LifeRect(const LifeRect& r) {
	left = r.left;
	top = r.top;
	width = r.width;
	height = r.height;
}

LifeRect::LifeRect() {
	left = top = 0;
	width = height = 0;
}

int LifeRect::right() const {
	return left + width - 1;
}

int LifeRect::bottom() const {
	return top + height - 1;
}

void LifeRect::setRight(int x) {
	left = x - width + 1;
}

void LifeRect::setBottom(int y) {
	top = y - width + 1;
}

bool LifeRect::contains(const LifeCell& cell) const {
	return contains(cell.col, cell.row);
}

bool LifeRect::contains(int x, int y) const {
	return x >= left && x < (left + width)
		&& y >= top && y < (top + height);
}

bool LifeRect::intersects(int x, int y, size_t wd, size_t ht) const {
	return std::min(x + wd, left + width) - std::max(x, left) >= 0
        && std::min(y + ht, top + height) - std::max(y, top) >= 0;
}

bool LifeRect::intersects(const LifeRect& r) const {
	return intersects(r.left, r.top, r.width, r.height);
}
