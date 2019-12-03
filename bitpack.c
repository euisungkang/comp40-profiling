/* 
 * bitpack.c
 * COMP40
 * Fall 2019
 *
 * Assignment: HW4, Arith
 * Module handling bitwise operations regarding words. Handles
 * deeming whether a value will fit, acquiring values from a word,
 * and creating a word. Works for both unsigned integers and
 * signed integers.
 * 
 * Created by Felix J. Yu (fyu04), Jonathan S. Liu (jliu36)
 * October 24, 2019
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "bitpack.h"
#include "assert.h"
#include "except.h"

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/*
 * Function: Bitpack_fitsu
 * Input: 64 bit unsigned integer to determine fit, and tested width
 * Returns: Boolean value
 * Should return boolean value, true if word of length width can fit inside
 * unsigned integer. Default value is false if width is 0
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        uint64_t temp = 1;
        if (width == 0) {
                return false;
        }
        if (n >= temp << width) {
                return false;
        }
        else {
                return true;
        }
}

/*
 * Function: Bitpack_fitss
 * Input: 64 bit signed integer to determine fit, and tested width
 * Returns: Boolean value
 * Should return boolean value, true if word of length width can fit inside
 * signed integer. Default value is false if width is 0.
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
        int64_t temp = 1;
        if (width == 0) {
                return false;
        }
        if (n >= -temp << (width-1) && n < temp << (width-1)) {
                return true;
        }
        else {
                return false;
        }
}

/*
 * Function: Bitpack_getu
 * Input: 64 bit unsigned integer word, and width, 
 * and least significant bit
 * Returns: uint64_t value of queried bitword
 * Returns bits of length width from the entire word stating at lsb,
 * with default value as 0.
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert((width + lsb) <= 64);
        if (width == 0) {
                return 0;
        }

        uint64_t temp = ~0;
        temp = temp >> (64 - width) << lsb;
        return (word & temp) >> lsb;
}

/*
 * Function: Bitpack_gets
 * Input: 64 bit unsigned integer word, and width, 
 * and least significant bit
 * Returns: int64_t value of queried bitword
 * Returns bits of length width from the entire word stating at lsb,
 * with default value as 0. Returns a signed integer instead of the
 * traditional unsigned version.
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert((width + lsb) <= 64);
        if(width == 64) {
                return (int64_t) word;
        }
        else if(width == 0) {
                return 0;
        }

        uint64_t mask = ~(uint64_t)0 >> (64 - width) << lsb;
        uint64_t sign = (uint64_t)1 << (lsb + width - 1);

        bool negative = (sign & word) >> (lsb + width - 1);

        uint64_t unsigned_data = (mask & word) >> lsb;

        int64_t data = unsigned_data - negative * ((uint64_t)1 << width);
        
        return data;
}

/*
 * Function: Bitpack_newu
 * Input: 64 bit unsigned integer word, width of bits, 
 * least significant bit of where to put bits, and 64 bit unsigned integer
 * bit that you want to insert into the word
 * Returns: uint64_t value of new word with added value
 * Adds value with length width to the word starting at lsb. Errors if 
 * value would be longer than the length of word, or if width is longer than
 * MAX_BITS.
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
        uint64_t value)
{
        assert(width <= 64);
        assert((width + lsb) <= 64);

        if (!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        uint64_t temp = ~0;
        temp = temp >> (64 - width) << lsb;

        word = (word & ~temp);
        value = value << lsb;

        return (word | value);
}

/*
 * Function: Bitpack_news
 * Input: 64 bit unsigned integer word, width of bits, 
 * least significant bit of where to put bits, and 64 bit signed integer
 * bit that you want to insert into the word
 * Returns: uint64_t value of new word with added value
 * Adds value with length width to the word starting at lsb. Errors if 
 * value would be longer than the length of word, or if width is longer than
 * MAX_BITS.
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
        int64_t value)
{
        assert(width <= 64);
        assert((width + lsb) <= 64);

        if (!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        uint64_t temp = ~0;
        temp = temp >> (64 - width) << lsb;

        word = (word & ~temp);
        value = value << lsb;

        value = (value & temp);

        return (word | value);
}