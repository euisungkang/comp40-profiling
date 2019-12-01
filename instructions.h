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
#include "segmem.h"

#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

/* Arithmetic Operations */
void add(uint32_t *, uint32_t *, uint32_t *);
void multiply(uint32_t *, uint32_t *, uint32_t *);
void divide(uint32_t *, uint32_t *, uint32_t *);
void NAND(uint32_t *, uint32_t *, uint32_t *);

/* Store/Load Operations */
void store(Segment, uint32_t *, uint32_t *, uint32_t *);
void load(Segment, uint32_t *, uint32_t, uint32_t);
void load_program(Segment, uint32_t *, uint32_t *, uint32_t *);
void load_value(uint32_t, uint32_t *);

/* Mapping Operations */
void map(Segment, uint32_t *, uint32_t *);
void unmap(Segment, uint32_t *);

/* I/O Operations */
void input(uint32_t *C);
void output(uint32_t *C);

void cond_move(uint32_t *, uint32_t *, uint32_t *);
void halt();

#endif