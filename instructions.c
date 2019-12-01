/*
Easton Euisung Kang, Jonathan Liu
Professor Monroe
COMP-40

PROJECT: um
FILE: ionstructions.c
PURPOSE: Implementation of instructions.h
Last Modified: 11/19/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdint.h> 
#include "instructions.h"
#include "segmem.h"


/* Arithmetic Operations */
/* Input: registers A, B, C
   Command #3 in spec. Add's the values of B and C, and puts it in A */
void add(uint32_t *A, uint32_t *B, uint32_t *C)
{
        assert(A != NULL);
        *A = (*B + *C);
}

/* Input: registers A, B, C
   Command #4 in spec. Multiplies the values B and C, and puts it in A */
void multiply(uint32_t *A, uint32_t *B, uint32_t *C)
{
        assert(A != NULL);
        *A = (*B * *C);
}

/* Input: registers A, B, C
   Command #5 in spec. Divides the values B and C, and puts it in A */
void divide(uint32_t *A, uint32_t *B, uint32_t *C)
{
        assert(A != NULL);
        assert(*C != 0);

        *A = (uint32_t) floor(*B / *C);
}

/* Input: registers A, B, C 
   Command #6 in spec. Performs NAND bitwise operation on B and C */
void NAND(uint32_t *A, uint32_t *B, uint32_t *C)
{
        assert(A != NULL);
        *A = ~((*B) & (*C));
}

/* Store/Load Operations */
/* Input: segmem, registers A, B, C
   Command #2 in spec. Stores a given value C in m[A][B] */
void store(Segment segmem, uint32_t *A, uint32_t *B, uint32_t *C)
{
        Seg_Store(segmem, A, B, C);
}

/* Input: segmem, registers A, B, C 
   Command #1 in spec. Loads the value m[B][C] into A */
void load(Segment segmem, uint32_t *A, uint32_t B, uint32_t C)
{
        Seg_Load(segmem, A, B, C);
}

/* Input: segmem, prog counter, registers B, C
   Command #12 in spec. Loads a new program B into segmem. counter
   is set to m[0][C]        */
void load_program(Segment segmem, uint32_t *counter, uint32_t *B, uint32_t *C)
{
        Seg_Load_Program(segmem, counter, B, C);
}
void load_value(uint32_t value, uint32_t *A)
{
        *A = value;
}

////////////////////////
/* Mapping Operations */
////////////////////////
/* Input: segmem, registers B, C
   Command #8 in spec. Maps C amounts of words into m[B] */
void map(Segment segmem, uint32_t *B, uint32_t *C)
{
        Seg_Map(segmem, B, C);
}

/* Input: segmem, register C
   Command #9 in spec. Frees the memory m[C] for later reusage */
void unmap(Segment segmem, uint32_t *C)
{
        Seg_Unmap(segmem, C);
}

/* I/O Operations */
/* Input: register C
   Command #11 in spec. Gets a char from stdin, checks for EOF, then sets it
   to C */
void input(uint32_t *C)
{
        char in = (char) getc(stdin);

        /* chars are defaultly limited to range of 0 ~ 255. Thus, no need
           to check if input is in desired range. Compilation error
           if we do */

        /* If EOF, then we must fill C with 1s */
        if (in == EOF) {
                *C = ~0;
        }

        else {
                if(in == '\n') {
                        input(C);
                }
                *C = in;
        }
}

/* Input: register C
   Command #10 in spec. Displays whatever is in C to stdout */
void output(uint32_t *C)
{
        /* char's range is 0 ~ 255 as detailed in spec */
        printf("%c", (char)*C);
}

/* Input: registers A, B, C
   Command #0 in spec. Moves B into A unless C is 0 */
void cond_move(uint32_t *A, uint32_t *B, uint32_t *C)
{
        if (*C != 0) {
                *A = *B;
        }
}

/* Input: 
   Command #7 in spec. Halts the program by exiting with EXIT_SUCCESS */
void halt()
{
        exit(EXIT_SUCCESS);
}