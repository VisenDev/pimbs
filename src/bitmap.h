#ifndef BITMAP_H
#define BITMAP_H

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif /*CHAR_BIT*/

#include "debug.h"

#define word_offset(bit) ((bit) / CHAR_BIT)
#define bit_offset(bit)  ((bit) % CHAR_BIT)

#define Bitmap(bit_count) struct { unsigned char bits[(bit_count / CHAR_BIT) + 1]; }

#define get_bit_unsafe(bitmap, bit) (bitmap.bits[word_offset(bit)] & (1 << bit_offset(bit)) ? 1 : 0)

#define get_bit(bitmap, bit) ( \
    inline_assert(bit < sizeof(bitmap.bits)), \
    inline_assert(bit > 0), \
    get_bit_unsafe(bitmap, bit) \
)

#define set_bit(bitmap, bit)   do { \
    simple_assert(bit < sizeof(bitmap.bits) * CHAR_BIT, "Bit chosen is out of bounds"); \
    simple_assert(bit > 0, "Bit chosen is less than zero"); \
    bitmap.bits[word_offset(bit)] |= (1 << bit_offset(bit)); \
} while (0)

#define unset_bit(bitmap, bit) do { \
    simple_assert(bit < sizeof(bitmap.bits) * CHAR_BIT, "Bit chosen is out of bounds"); \
    simple_assert(bit > 0, "Bit chosen is less than zero"); \
    bitmap.bits[word_offset(bit)] |= (0 << bit_offset(bit)); \
 } while (0)

#define Bitmap2D(width, height) struct { Bitmap(width) row[height]; }
#define get_bit_2d(bitmap, x, y) get_bit(bitmap.row[y], x)
#define set_bit_2d(bitmap, x, y) set_bit(bitmap.row[y], x)
#define unset_bit_2d(bitmap, x, y) unset_bit(bitmap.row[y], x)

#endif /*BITMAP_H*/
