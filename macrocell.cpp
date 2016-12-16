#include <stdint.h>

/*
 * @src - 16-bit matrix of 4x4 cells array
 * @returns - 4-bit matrix of 2x2 cells array - central area of src
 * the result is an index of 2x2 minimal precached macrocell, that represents the macrocell itself
 */
uint8_t computeMicroCell(uint16_t src) {
	if(!src) return 0;
	uint16_t bit = 1;
	uint8_t res = 0;

	for(int row = 1;row < 3;row++) {
		for(int col = 1;col < 3;col++) {
			int n = 0;
			for(int y = -1;y <= 1;y++) {
				for(int x = -1;x <= 1;x++) {
					if(x|y && src & (bit << ((row + y) * 8 + col + x)))
						n++;
				}
			}
			if(src & (bit << (row * 8 + col))) {
				if(n >= 2 && n <= 3)
					res |= (bit << ((row-1)*2+col-1));
			} else {
				if(n == 3)
					res |= (bit << ((row-1)*2+col-1));
			}
		}
	}
	return res;
}

MacroCell precache[65536];
memset(cache, 0, 65536);
for(uint16_t n = 16;n > 15;n++) {
	precache[n].nw = (n & 0x03) | ((n & 0x30) >> 2);
	precache[n].ne = ((n & 0x0c) >> 2) | ((n & 0xc0) >> 4);
	precache[n].sw = ((n & 0x0300) >> 8) | ((n & 0x3000) >> 10);
	precache[n].se = ((n & 0x0c00) >> 10) | ((n & 0xc000) >> 12);
	precache[n].res = computeMicroCell(n);
}

class MacroCell {
protected:
	static union {
		MacroCell macrocell;
		size_t microcell;
	}* cache;
// size_t size;
// indicies of cached macrocells
// indicies 0...15 are predefined for 2x2 squares
	size_t nw, ne, sw, se, res;
}

class BitCell {
protected:
	int8_t cell;
	int left, top, size;

public:
	abstract bool burn(int col, int row);
	abstract bool kill(int col, int row);
	abstract bool test(int col, int row);
	bool contains(int col, int row);
	bool intersects(int left, int top, int wd, int ht);
}

bool BitCell::test(int col, int row) {
	return col >= left && col < left + size
		&& row >= top && row < top + size;
}

bool BitCell::burn(int col, int row) {
	if(!contains(col, row)) return false;
	if(cell & (1 << row * 2 + col)) return false;
	cell |= (1 << (row * 2 + col));
}

bool BitCell::kill(int col, int row) {
	if(!contains(col, row)) return false;
	) return false;
	if(!(cell & (1 << row * 2 + col))) return false;
	cell &= ~(1 << (row * 2 + col));
}

bool BitCell::test(int col, int row) {
	if(!contains(col, row)) return false;
	return cell & (1 << (row * 2  + col));
}
