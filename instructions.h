/*
Easton Euisung Kang, Jonathan Liu
Professor Monroe
COMP-40

PROJECT: um
FILE: ionstructions.h
PURPOSE: Processes all instructions given by UM.
Last Modified: 11/19/2019
*/

#include <stdio.h>
#include <stdint.h>
#include <seq.h>

#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

typedef struct Segment {
        Seq_T m;
        Seq_T unmapped;
        int seg_count;
        
}*Segment;

Segment Seg_New();
void Seg_Free(Segment *segmem);

/* Store/Load Operations */
void store(Segment, uint32_t *, uint32_t *, uint32_t *);
void load(Segment, uint32_t *, uint32_t, uint32_t);
void load_program(Segment, uint32_t *, uint32_t *, uint32_t *);

#endif