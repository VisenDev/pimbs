#ifndef BITMAP_H
#define BITMAP_H

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif /*CHAR_BIT*/

#include "debug.h"

#define word_offset(bit) ((bit) / CHAR_BIT)
#define bit_offset(bit)  ((bit) % CHAR_BIT)
#define Bitmap(bit_count) struct { char bits[(bit_count / CHAR_BIT) + 1]; }

/*generic function*/
#define get_bit(bitmap, bit) get_bit_internal(bitmap.bits, sizeof(bitmap.bits), bit)
UNUSED
static int get_bit_internal(char bits[], long num_bytes, long bit) {
    const long max_bit = num_bytes * CHAR_BIT;
    debug_assert((long)bit, <, (long)max_bit);
    return !!(bits[word_offset(bit)] & (1 << bit_offset(bit)));
}


#define set_bit(bitmap, bit) set_bit_internal(bitmap.bits, sizeof(bitmap), bit, 1)
#define unset_bit(bitmap, bit) set_bit_internal(bitmap.bits, sizeof(bitmap), bit, 0)
UNUSED
static void set_bit_internal(char bits[], long num_bytes, long bit, int boolean) {
    const long max_bit = num_bytes * CHAR_BIT;
    const int true_or_false = !!boolean;
    debug_assert((true_or_false == 0), ||, (true_or_false == 1));
    debug_assert((long)bit, <, (long)max_bit);
    bits[word_offset(bit)] |= ((char)true_or_false << bit_offset(bit));
}

#define Bitmap2D(width, height) struct { Bitmap(width) row[height]; }
#define get_bit_2d(bitmap, x, y) get_bit(bitmap.row[y], x)
#define set_bit_2d(bitmap, x, y) set_bit(bitmap.row[y], x)
#define unset_bit_2d(bitmap, x, y) unset_bit(bitmap.row[y], x)



#endif /*BITMAP_H*/
