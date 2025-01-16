#ifndef BITMAP_H
#define BITMAP_H

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif /*CHAR_BIT*/

#include "debug.h"

#define word_offset(bit) ((bit) / CHAR_BIT)
#define bit_offset(bit)  ((bit) % CHAR_BIT)
#define Bitmap(bit_count) struct { unsigned char bits[(bit_count / CHAR_BIT) + 1]; }

/*generic function*/
#define get_bit(bitmap, bit) get_bit_internal(bitmap.bits, sizeof(bitmap.bits), bit)
UNUSED
static int get_bit_internal(unsigned char bits[], unsigned long num_bytes, unsigned long bit) {
    const unsigned long max_bit = num_bytes * CHAR_BIT;
    simple_assert(bit < max_bit, "bit is too big");
    return !!(bits[word_offset(bit)] & (1 << bit_offset(bit)));
}


#define set_bit(bitmap, bit) set_bit_internal(bitmap.bits, sizeof(bitmap), bit, 1)
#define unset_bit(bitmap, bit) set_bit_internal(bitmap.bits, sizeof(bitmap), bit, 0)
UNUSED
static void set_bit_internal(unsigned char bits[], unsigned long num_bytes, unsigned long bit, int boolean) {
    const unsigned long max_bit = num_bytes * CHAR_BIT;
    const int true_or_false = !!boolean;
    simple_assert(bit < max_bit, "bit is too big");
    bits[word_offset(bit)] |= (true_or_false << bit_offset(bit));
}

#define Bitmap2D(width, height) struct { Bitmap(width) row[height]; }
#define get_bit_2d(bitmap, x, y) get_bit(bitmap.row[y], x)
#define set_bit_2d(bitmap, x, y) set_bit(bitmap.row[y], x)
#define unset_bit_2d(bitmap, x, y) unset_bit(bitmap.row[y], x)



#endif /*BITMAP_H*/
