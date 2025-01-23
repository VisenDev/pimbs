#ifndef FORMAT_H
#define FORMAT_H

/*INTEGER FORMATTING*/

/*returns the number of characters used*/
unsigned long
format_unsigned_long(char * buf, const unsigned long buflen, const unsigned long number, const unsigned long radix)
#ifdef FORMAT_IMPLEMENTATION
{
    const long last_digit_i = (long)buflen - 1;
    unsigned long remaining = number;
    long i = 0;
    long num_digits_written = 0;

    /*temporary buffer*/
    char tmp[sizeof(long) * 16] = {0};

    /*don't do anything if empty buffer is provided*/
    if(buflen == 0) {
        return 0;
    }


    for(i = 0; remaining != 0 && i < last_digit_i; ++i) {
        const char digit = (char)(remaining % radix);
        if(digit >= 10) {
            tmp[i] = (char)(digit - 10) + 'A';
        } else {
            tmp[i] = (char)digit + '0';
        }

        remaining /= radix;
    }

    num_digits_written = i - 1;

    /*write to actual buffer now*/
    for(i = 0; i <= num_digits_written; ++i){
        buf[i] = tmp[num_digits_written - i];
    }

    /*append null terminator*/
    buf[num_digits_written + 1] = 0;

    if (buflen > 0) {
        buf[buflen - 1] = 0; /* Ensure null-termination even if truncated */
    }

    return (unsigned long)num_digits_written;
}
#else
;
#endif


unsigned long
format_long(char * buf, const unsigned long buflen, const long number, const unsigned short radix)
#ifdef FORMAT_IMPLEMENTATION
{
    if(number < 0) {
        buf[0] = '-';
        return format_unsigned_long(buf + 1, buflen - 1, (unsigned long)(-number), radix) + 1;
    } else {
        return format_unsigned_long(buf, buflen, (unsigned long)number, radix);
    }

}
#else
;
#endif

/*
#define format_unsigned_int(buf, buflen, number, radix) format_long(buf, buflen, number, radix)
*/

unsigned long
format_unsigned_int(char * buf, const unsigned long buflen, const unsigned int number, const unsigned short radix)
#ifdef FORMAT_IMPLEMENTATION
{
    return format_unsigned_long(buf, buflen, (unsigned long)number, radix);

}
#else
;
#endif


unsigned long
format_int(char * buf, const unsigned long buflen, const int number, const unsigned short radix)
#ifdef FORMAT_IMPLEMENTATION
{
    return format_long(buf, buflen, (long)number, radix);
}
#else
;
#endif


unsigned long
format_unsigned_short(char * buf, const unsigned long buflen, const unsigned short number, const unsigned short radix)
#ifdef FORMAT_IMPLEMENTATION
{
    return format_unsigned_long(buf, buflen, (unsigned long)number, radix);
}
#else
;
#endif


unsigned long
format_short(char * buf, const unsigned long buflen, const short number, const unsigned short radix)
#ifdef FORMAT_IMPLEMENTATION
{
    return format_long(buf, buflen, (long)number, radix);
}
#else
;
#endif


/* FLOATING POINT FORMATTING */

#define ABS(num) (num < 0 ? (num * -1) : num)

unsigned long
format_double(char * buf, const unsigned long buflen, const double number, const unsigned short precision)
#ifdef FORMAT_IMPLEMENTATION
{
    const long integer_part = (long)number;
    const double decimal_part = ABS(number) - (double)(ABS(integer_part));
    unsigned long digits_used = format_long(buf, buflen, integer_part, 10);
    debug_assert(double, decimal_part, <=, 1.0);
    debug_assert(double, decimal_part, >=, 0.0);

    if (precision > 0) {
        unsigned long i;
        double fraction;

        if (digits_used < buflen - 1) {
            buf[digits_used + 1] = '.';
            digits_used += 2;
        } else {
            return digits_used;
        }

        fraction = decimal_part;
        for (i = 0; i < precision; ++i) {
            unsigned char digit = 0;
            fraction *= 10;
            digit = (unsigned char)fraction;
            fraction -= digit;

            if (digits_used < buflen - 1) {
                buf[digits_used] = (char)('0' + digit);
                digits_used += 1;
            } else {
                return digits_used;
            }
        }
    }

    if (digits_used < buflen) {
        buf[digits_used] = 0; /* Null-terminate the string */
    } 

    if (buflen > 0) {
        buf[buflen - 1] = 0; /* Ensure null-termination even if truncated */
    }

    return digits_used;
}
#else
;
#endif


unsigned long
format_float(char * buf, const unsigned long buflen, const float number, const unsigned short precision)
#ifdef FORMAT_IMPLEMENTATION
{
    return format_double(buf, buflen, (double)number, precision);
}
#else
;
#endif


#endif /*FORMAT_H*/
