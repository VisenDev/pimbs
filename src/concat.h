#ifndef CONCAT_H
#define CONCAT_H

/* Indirection to force expansion */
#define CONCAT9(x, y) x##y
#define CONCAT8(x, y) CONCAT9(x, y)
#define CONCAT7(x, y) CONCAT8(x, y)
#define CONCAT6(x, y) CONCAT7(x, y)
#define CONCAT5(x, y) CONCAT6(x, y)
#define CONCAT4(x, y) CONCAT5(x, y)
#define CONCAT3(x, y) CONCAT4(x, y)
#define CONCAT2(x, y) CONCAT3(x, y)
#define CONCAT1(x, y) CONCAT2(x, y)
#define CONCAT(x, y) CONCAT1(x, y)

#endif /*CONCAT_H*/
