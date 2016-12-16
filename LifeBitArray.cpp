#include <iostream>
#include "AbstractLife.h"

using namespace std;

class Life : AbstractLife {
private:
	int left, top;
	vector< vector<bool> > matrix;
	void expandHCenter();
	void expandVCenter();

public:
	Life();
	bool burn(int col, int row);
	bool kill(int col, int row);
	bool test(int col, int row) const;
	size_t population() const;
	std::vector<LifeCell>* exportCells() const;

	inline int getLeftBound() const { return left; }
	inline int getTopBound() const { return top; }

	LifeRect getRect() const;
};

Life::Life() : matrix(16, vector<bool>(16)) {
	left = top = 0;
}

void Life::expandHCenter() {
	size_t wd = matrix[0].size();

	for(vector< vector<bool> >::iterator it = matrix.begin();it != matrix.end();it++) {
		(*it).insert((*it).begin(), wd/2, false);
		(*it).insert((*it).end(), wd/2, false);
	}
	left -= wd/2;
}

void Life::expandVCenter() {
	size_t ht = matrix.size();
	vector<bool> ins(matrix[0].size(), false);
	matrix.insert(matrix.begin(), ht/2, ins);
	matrix.insert(matrix.end(), ht/2, ins);
	top -= ht/2;
}

LifeRect Life::getRect() const {
	int l = 0, t = 0, r = 0, b = 0;
	size_t wd = matrix[0].size();
	bool empty = true;
	for(size_t y = 0;y < matrix.size();y++) {
		for(size_t x = 0;x < wd;x++) {
			if(matrix[y][x]) {
				if(empty) {
					empty = false;
					l = r = x;
					t = b = y;
				} else {
					if(x < l) l = x;
					else if(x > r) r = x;
					if(y < t) t = y;
					else if(y > b) b = y;
				}
			}
		}
	}
	if(empty) return LifeRect(0, 0, 0, 0);
	return LifeRect(l+left, t+top, r-l+1, b-t+1);
}

vector<LifeCell>* Life::exportCells() const {
	vector<LifeCell>* res = new vector<LifeCell>;
	size_t wd = matrix[0].size();
	for(size_t y = 0;y < matrix.size();y++) {
		for(size_t x = 0;x < wd;x++) {
			if(matrix[y][x]) {
				res->push_back(LifeCell(x, y));
			}
		}
	}
	if(res->empty()) { delete res; return NULL; }
	return res;
}

bool Life::burn(int col, int row) {
}

int main(int argc, char** argv) {
	

	return 0;
}
