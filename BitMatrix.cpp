#include "BitMatrix.h"

/*
class rect {
public:
	int left, top;
	size_t width, height;

	rect();
	rect(int left, int top, size_t width, size_t height);
}

rect::rect(int l, int t, size_t w, size_t h) {
	left = l;
	top = t;
	width = w;
	height = h;
}

rect::rect() {
	left = top = 0;
	width = height = 0;
}
*/

BMatrix::BMatrix() {
// reserve 1x1 16-bit pack for 4x4 matrix
	data = new uint16_t*; // 4 rows / 1 4-bit row
	*data = new uint16_t; // 4 columns / 1 4-bit column
	cols = rows = 4;
	**data = 0;
//	left = top = 0;
}

BMatrix::BMatrix(size_t wd, size_t ht) {
	size_t wd16 = (wd - 1) / 4 + 1;
	size_t ht16 = (ht - 1) / 4 + 1;

	data = new uint16_t*[ht16];
	for(size_t row = 0;row < ht16;row++) {
		data[row] = new uint16_t[wd16];
		memset(data[row], 0, wd16 * 2);
	}
	cols = wd16 << 2;
	rows = ht16 << 2;
//	left = top = 0;
}

uint16_t BMatrix::getPack(size_t col, size_t row) const {
	if(col >= (cols >> 2) || row >= (rows >> 2)) return 0;
	return data[row][col];
}

/*
rect BMatrix::activePacks() const {
	size_t rows = this->rows >> 2;
	size_t cols = this->cols >> 2;

	rect r;

	for(size_t row = 0;row < rows;row++) {
		bool empty = true;

		for(size_t col = 0;col < cols;col++) {
			if(data[row][col]) {
				empty = false;
				if(col < r.left) r.left = col;
				if(!r.width) r.width = 1;
				else if(col - r.left + 1 > r.width)
					r.width = col - r.left + 1;
			}
		}

		if(!empty) {
			if(row < r.top) r.top = row;
			if(!r.height) r.height = 1;
			else if(row - r.top + 1 > r.height)
				r.height = row - r.top + 1;
		}
	}

	return r;
}
*/

bool BMatrix::test(size_t col, size_t row) const {
	if(col >= cols || row >= rows) return false;
	return (data[row >> 2][col >> 2] & (1 << ((row % 4) * 4 + (col % 4)))) != 0;
}

bool BMatrix::set(size_t col, size_t row, bool v) {
	if(col >= cols || row >= rows) return !v;
	uint16_t bit = 1 << ((row % 4) * 4 + (col % 4));
	if(v) data[row >> 2][col >> 2] |= bit;
	else data[row >> 2][col >> 2] &= ~bit;
	return true;
}

void BMatrix::expand(size_t n, size_t s, size_t w, size_t e) {
	size_t cols = this->cols / 4;
	for(size_t row = 0;row < rows;row++) {
		uint16_t* newdata = new uint16_t[cols + w + e];
		memcpy(newdata + w, data[row], cols * 2);
		memset(newdata, 0, w * 2);
		memset(newdata + w + cols, 0, e * 2);
		delete data[row];
		data[row] = newdata;
	}
	cols += w + e;
	this->cols = cols * 4;

	size_t rows = this->rows / 4;
	uint16_t** newdata = new uint16_t* [rows + n + s];
	memcpy(newdata + n, data, (sizeof (void*)) * rows);
	for(size_t y = 0;y < n;y++) {
		newdata[y] = new uint16_t[cols];
		memset(memdata[y], 0, cols * 2);
	}
	for(size_t y = n + rows;y < n + rows + s;y++) {
		newdata[y] = new uint16_t[cols];
		memset(memdata[y], 0, cols * 2);
	}
	delete data;
	data = newdata;
	rows += n + s;
	this->rows = rows * 4;
}

void BMatrix::clear() {
	size_t rows = this->rows / 4;
	soze_t cols = this->cols / 4;
	for(size_t y = 0;y < rows;y++)
		memset(data[y], 0, cols * 2);
}
