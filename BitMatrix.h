#include <stdint.h>
#include <string.h>

class BMatrix {
private:
	size_t rows, cols;
	uint16_t **data;

public:
	BMatrix();
	BMatrix(size_t cols, size_t rows);

	uint16_t getPack(size_t col, size_t row) const;
	bool test(size_t col, size_t row) const;
	bool set(size_t col, size_t row, bool val);

	inline size_t getRows() const { return rows; }
	inline size_t getCols() const { return cols; }

	void expand(size_t n, size_t s, size_t w, size_t e);
	void clear();
};
