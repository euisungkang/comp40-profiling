/*
Easton Euisung Kang, Jonathan Liu
Professor Monroe
COMP-40

PROJECT: um
FILE: segmem.c
PURPOSE: Implementation file for segmem.h.
Last Modified: 11/19/2019
*/

#include <assert.h>
#include <string.h>
#include "segmem.h"

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

/* Input: segmem, registers A, B, C
   Loads the value at m[B][C] into A */
void Seg_Load(Segment segmem, uint32_t *A, uint32_t B, uint32_t C)
{
        assert(A != NULL);
        uint32_t *duplicate = Seq_get(segmem -> m, B);
        *A = duplicate[C+1];    /* +1 since index 0 holds the size of array */
}

/* Input: segmem, prog counter, registers B, C
   Replaces the current program with a new program in B. Then sets the 
   counter to the correct location to start from. If the loaded program
   is the same as current, process is much faster */
void Seg_Load_Program(Segment segmem, uint32_t *counter, uint32_t *B,
                      uint32_t *C)
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