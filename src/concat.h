#ifndef CONCAT

// Indirection to force expansion
#define CONCAT3(x, y) x##y
#define CONCAT2(x, y) CONCAT3(x, y)
#define CONCAT1(x, y) CONCAT2(x, y)
#define CONCAT(x, y) CONCAT1(x, y)

#endif //CONCAT
