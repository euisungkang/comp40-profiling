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
#include <string.h>


/* Input: 
   Creates a new segmented memory in heap and returns it. All sequences in
   segmem are initialized to a default size of 0 */
Segment Seg_New()
{
        Segment segmem = malloc(sizeof(struct Segment));
        assert(segmem != NULL);
        segmem -> m = Seq_new(0);
        segmem -> unmapped = Seq_new(0);
        segmem -> seg_count = 0;
        return segmem;
}

/* Input: segmem *
   Frees all memory associated with the segmem given. Frees the
   segmem itself as well */
void Seg_Free(Segment *segmem)
{
        while(Seq_length((*segmem) -> m) > 0) {
                uint32_t *temp = (uint32_t *)Seq_remhi((*segmem) -> m);
                if (temp != NULL) {
                        free(temp);
                }
        }
        Seq_free(&((*segmem)) -> m);
        while(Seq_length((*segmem) -> unmapped) > 0) {
                free((uint32_t *)Seq_remhi((*segmem) -> unmapped));
        }
        Seq_free(&((*segmem) -> unmapped));

        free(*segmem);
}

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
        uint32_t *destination = (uint32_t *) Seq_get(segmem -> m, *A);
        assert(destination != NULL);
        destination[*B + 1] = *C;
}

/* Input: segmem, registers A, B, C 
   Command #1 in spec. Loads the value m[B][C] into A */
void load(Segment segmem, uint32_t *A, uint32_t B, uint32_t C)
{
        assert(A != NULL);
        uint32_t *duplicate = Seq_get(segmem -> m, B);
        *A = duplicate[C+1];    /* +1 since index 0 holds the size of array */
}

/* Input: segmem, prog counter, registers B, C
   Command #12 in spec. Loads a new program B into segmem. counter
   is set to m[0][C]        */
void load_program(Segment segmem, uint32_t *counter, uint32_t *B, uint32_t *C)
{
        if (*B == 0) {
                *counter = *C + 1;
                return;
        }
        /* Create the destination and duplicate, and make sure to copy values,
           not reference */
        free(Seq_get(segmem->m, 0));
        uint32_t *duplicate = Seq_get(segmem -> m, *B);
        
        uint32_t size = duplicate[0];
        uint32_t *zero_seg = malloc(size * sizeof(uint32_t));
        memcpy(zero_seg, duplicate, size * sizeof(uint32_t));

        Seq_put(segmem->m, 0, zero_seg);

        *counter = *C + 1;
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
        int size = *C;
        uint32_t id;
        if(Seq_length(segmem->unmapped) > 0) {
                id = *(uint32_t *) Seq_get(segmem->unmapped,
                                    Seq_length(segmem->unmapped) - 1);
                free(Seq_remhi(segmem->unmapped));
                uint32_t *seg = calloc((size + 1), sizeof(uint32_t));
                assert(seg != NULL);
                seg[0] = size + 1;
                Seq_put(segmem -> m, id, seg);

        }else {
                id = segmem->seg_count;
                segmem->seg_count++;
                uint32_t *seg = calloc((size + 1), sizeof(uint32_t));
                assert(seg != NULL);
                seg[0] = size + 1;
                Seq_addhi(segmem->m, seg);
        }   
        *B = id;
}

/* Input: segmem, register C
   Command #9 in spec. Frees the memory m[C] for later reusage */
void unmap(Segment segmem, uint32_t *C)
{
        uint32_t *id = malloc(sizeof(uint32_t));
        *id = *C;
        uint32_t *unused = Seq_get(segmem -> m, *id);
        free(unused);
        Seq_put(segmem -> m, *id, NULL);
        Seq_addhi(segmem -> unmapped, (void *) id);
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

        if (in == '\n') {
                in = (char) getc(stdin);
        } 

        /* If EOF, then we must fill C with 1s */
        if (in == EOF) {
                *C = 1;
        } else {
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