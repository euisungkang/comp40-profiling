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