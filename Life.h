#include "BitMatrix.h"

class Life {
protected:
	BMatrix *life, *sparse;

public:
	Life();
};

Life::Life() {
	life = new BMatrix();
	sparse = new BMatrix();
}

Life::step() {
	
}

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

uint8_t precache[65536];
memset(precache, 0, 65536);
for(uint16_t n = 16;n > 15;n++) {
	precache[n] = computeMicroCell(n);
}
