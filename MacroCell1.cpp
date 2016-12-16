#include <stdint.h>

typedef vector< vector<uint16_t> > MMatrix;

/*
 * concatenate 2 macrocells 4x4 into one 8x4 and extract the middle 4x4
 */
uint16_t centerH(uint16_t left, uint16_t right) {
	return ((left & 0x0c) >> 2)
		| ((left & 0xc0) >> 2)
		| ((left & 0x0c00) >> 2)
		| ((left & 0xc000) >> 2)
		| ((right & 0x03) << 2)
		| ((right & 0x30) << 2)
		| ((right & 0x0300) << 2)
		| ((right & 0x3000) << 2);
}

/*
 * concatenate 2 macrocells 4x4 into one 4x8 and extract the middle 4x4
 */
uint16_t centerV(uint16_t top, uint16_t bottom) {
	return (top >> 8) | (bottom << 8);
}

/*
 * concatenate 4 macrocells 4x4 into one 8x8 and extract the central 4x4 square
 */
uint16_t center(uint16_t nw, uint16_t ne, uint16_t sw, uint16_t se) {
	return (nw & 0x0c00) >> 10
		| (nw & 0xc000) >> 10
		| (ne & 0x0300) >> 6
		| (ne & 0x3000) >> 6
		| (sw & 0x0c) << 6
		| (sw & 0xc0) << 6
		| (se & 0x03) << 10
		| (se & 0x30) << 10;
}

/*
 * concatenate 4 macrocells 2x2 into one macrocell 4x4
 */
uint16_t concat(uint8_t nw, uint8_t ne, uint8_t sw, uint8_t se) {
	return (nw & 0x03)
		| ((nw & 0x0c) << 2)
		| ((ne & 0x03) << 2)
		| ((ne & 0x0c) << 4)
		| ((sw & 0x03) << 8)
		| ((sw & 0x0c) << 10)
		| ((se & 0x03) << 10)
		| ((se & 0x0c) << 12);
}

uint8_t get_nw(uint16_t c) { // 0
	return
		(c & 0x03) | ((c & 0x30) >> 2);

		(c & 0x03)
		((c >> 2) & 0x30)
}

uint8_t get_ne(uint16_t c) { // 1
	return
		((c & 0x0c) >> 2) | ((c & 0xc0) >> 4);

	((c >> 2) & 0x03)
	((c >> 4) & 0x30)
}

uint8_t get_sw(uint16_t c) { // 2
	return
		((c & 0x0300) >> 8) | ((c & 0x3000) >> 10);

	((c >> 8) & 0x03)
	((c >> 10) & 0x30)
}

uint8_t get_se(uint16_t c) { // 3
	return
		((c & 0x0c00) >> 10) | ((c & 0xc000) >> 12);

	((c >> 10) & 0x03)
	((c >> 12) & 0x30)
}

uint16_t get4x4(const MMatrix& m, size_t col, size_t row) {
	if(row >= m.size() || col >= m[row].size())
		return 0;
	return m[row][col];
}

void set4x4(const MMatrix& m, size_t col, size_t row, uint16_t val) {
	if(row >= m.size())
		m.resize(row + 1);
	if(col >= m[row].size())
		m[row].resize(col+1);
	m[row][col] = val;
}

uint8_t get2x2(uint16_t m, unsigned short col, unsigned short row) {
	static short idx[] = { 0, 2, 8, 10 };
	unsigned short index = row * 2 + col;
	if(!mc || index >= 4)
		return 0;
	return ((m >> idx[index]) & 0x03) | ((mc >> (idx[index]+2)) & 0x0c);
}

bool test(const MMatrix& m, size_t col, size_t row) {
	uint16_t macro4 = get4x4(m, col / 4, row / 4);
	if(!macro4) return false;
	int n = (row % 4) * 4 + (col % 4);
	return (macro4 & (1 << n)) != 0;
}

bool burn(MMatrix& m, size_t col, size_t row) {
	uint16_t macro4 = get4x4(m, col / 4, row / 4);
	int n = (row % 4) * 4 + (col % 4);
	if(macro4 & (1 << n)) return false;
	macro4 |= (1 << n);
	set4x4(m, col / 4, row / 4, macro4);
	return true;
}

bool kill(MMatrix& m, size_t col, size_t row) {
	uint16_t macro4 = get4x4(m, col / 4, row / 4);
	int n = (row % 4) * 4 + (col % 4);
	if(!(macro4 & (1 << n))) return false;
	macro4 &= ~(1 << n);
	set4x4(m, col / 4, row / 4, macro4);
	return true;
}

class Life {
protected:
	int left, top;
	uint16_t ** life, sparse;

	void swap() {
		MMatrix *tmp = life;
		life = sparse;
		sparse = tmp;
	}

	void padH(int size) {
		if(size == 0) return;
		size = size / 4 + (size % 4 ? 1 : 0);
		for(MMatrix::iterator it = sparse->begin();it != sparse->end();it++) {
			(*it).insert(size > 0 ? (*it).end() : (*it).begin(), size, 0);
		}
	}

	void padV(int size) {
		if(size == 0) return;
		size = size / 4 + (size % 4 ? 1 : 0);
		sparse->insert(size > 0 ? sparse->end() : sparse->begin(), size, vector<uint16_t>((*sparse)[0].size()));
	}

	static bool isEmpty(const Matrix& m) {
		for(size_t y = 0;y < m.size();y++) {
			for(size_t x = 0;x < m[y].size();x++) {
				if(m[y][x]) return false;
			}
		}
		return true;
	}

public:
};

MMatrix matrix1(1);
MMatrix matrix2(1);

MMatrix *life = &matrix1, *sparse = &matrix2;

size_t ht = life->size();
size_t wd = (*life)[0].size();

for(MMatrix::iterator it = life->begin();it != life->end();it++) {
	(*it)
}
