/*
Easton Euisung Kang, Jonathan Liu
Professor Monroe
COMP-40

PROJECT: um
FILE: segmem.h
PURPOSE: Segment memory of UM as an object. Contains all functionalities
         tied with using the segment or altering it.
Last Modified: 11/19/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <seq.h>

#ifndef _SEGMEM_H_
#define _SEGMEM_H_

typedef struct Segment {
        Seq_T m;
        Seq_T unmapped;
        int seg_count;
        
}*Segment;

Segment Seg_New();
void Seg_Load(Segment, uint32_t *, uint32_t, uint32_t);
void Seg_Load_Program(Segment, uint32_t *, uint32_t *, uint32_t *);
void Seg_Store(Segment, uint32_t *, uint32_t *, uint32_t *);
void Seg_Map(Segment, uint32_t *, uint32_t *);
void Seg_Unmap(Segment, uint32_t *);
void Seg_Free(Segment *segmem);

#endif