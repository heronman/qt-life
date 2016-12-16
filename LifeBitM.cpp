class Life {
private:
	uint16_t* data;
	size_t cols, rows;

public:
	Life();
}

Life::Life() {
	data = new uint16_t[1];
	cols = rows = 4;
}

void expand() {
	size_t cols = this->cols / 4;
	size_t rows = this->rows / 4;
	size_t size = cols * rows;

	size_t lastrow = cols * (rows - 1);
	uint8_t top = 0, bottom = 0;
	for(size_t x = 0; x < cols && (!top || !bottom);x++) {
		if(data[x] & 0xff)
			top = 1;
		if(data[lastrow + x] & 0xff00)
			bottom = 1;
	}
	uint8_t left = 0, right = 0;
	for(size_t y = 0;y < rows && (!left || !right);y++) {
		if(data[y * cols] & 0x3333)
			left = 1;
		if(data[y * cols + cols - 1] & 0xcccc)
			right = 1;
	}

	size_t newcols = cols + left + right;
	size_t newrows = rows + top + bottom;
	size_t newsize = newcols * newrows;
	if(newsize > size) {
		uint16_t* newdata = new uint16_t[newsize];
		if(top) memset(newdata, 0, newcols * 2);
		if(bottom) memset(newdata + newcols * (newrows - 1), 0, newcols * 2);
		for(size_t y = top;y < newrows - bottom;y++) {
			newdata[y * newcols] = data[y * newcols + newcols - 1] = 0;
			memcpy(newdata + (y * newcols), data + ((y - top) * cols), cols * 2);
		}
	}
}
