/*
Easton Euisung Kang, Jonathan Liu
Professor Monroe
COMP-40

PROJECT: um
FILE: init.c
PURPOSE: Main program file. Handles UM initializations, instructions
         and control flow.
Last Modified: 11/19/2019
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdint.h>
#include <math.h>
#include "assert.h"
#include "bitpack.h"
#include <seq.h>
#include <sys/stat.h>
#include "instructions.h"

/* Function Prototypes */
void run_instructions(Segment, uint32_t *, uint32_t, short, short, short, short, uint32_t);

int main(int argc, char *argv[])
{
        (void) argv;
        if (argc != 2) {
                fprintf(stderr, "Usage: ./um [program um file]\n");
                exit(EXIT_FAILURE);
        }

        Segment segmem = Seg_New();
        uint32_t *registers = calloc(8, sizeof(uint32_t));
        uint32_t counter = 1;

        FILE *input = fopen(argv[1], "r");
        struct stat sb;
        assert(stat(argv[1], &sb) == 0);
        int size = sb.st_size/4 + 1;
        uint32_t *m0 = malloc(sizeof(uint32_t)*size);
        char byteArray[4] = {0};
        int index = 1;
        int i = 3;

        m0[0] = size;
        while(fscanf(input, "%c", &(byteArray[i])) == 1) {
                if(i == 0) {
                        m0[index++] = *(uint32_t *) byteArray;
                        i = 4;
                }
                i--;
        }
        fclose(input);
        
        Seq_addhi(segmem->m, m0);
        segmem->seg_count++;


        uint32_t *zero = Seq_get(segmem->m, 0);
        while(counter < zero[0]) {
                short ra = 0, rb = 0, rc = 0;
                uint32_t value = 0;
                
                uint32_t code = (uint32_t) Bitpack_getu(zero[counter],
                                                        4, 28);
                if(code < 13) {
                        ra = (uint32_t) Bitpack_getu(zero[counter], 3, 6);
                        rb = (uint32_t) Bitpack_getu(zero[counter], 3, 3);
                        rc = (uint32_t) Bitpack_getu(zero[counter], 3, 0);
                }else {
                        ra = (uint32_t) Bitpack_getu(zero[counter], 3, 25);
                        value = (uint32_t) Bitpack_getu(zero[counter],
                                                        25, 0);
                }

                run_instructions(segmem, registers, counter, code, ra, rb, rc, value);
                if(code != 12) {
                        (counter)++;
                } else {
                        zero = Seq_get(segmem->m, 0);
                }
        }


        free(registers);
        exit(EXIT_SUCCESS);
}

/* Input: UM, opcode, register index A, B, C, value
   Determines which instruction is run through the opcode. A, B, C
   are passed in as actual register addresses. Entire condition structure
   is switch cases */
void run_instructions(Segment segmem, uint32_t *reg, uint32_t counter, short opcode, short A, short B,
                        short C, uint32_t value)
{
        switch(opcode) {
                case 0:
                        if (reg[C] != 0) {
                                reg[A] = reg[B];
                        }
                        break;
                case 1:
                        load(segmem, &reg[A],
                            reg[B], reg[C]);
                        break;
                case 2:
                        store(segmem, &reg[A],
                                &reg[B], &reg[C]);
                        break;
                case 3:
                        reg[A] = (reg[B] + reg[C]);
                        break;
                case 4:
                        reg[A] = (reg[B] * reg[C]);
                        break;
                case 5:
                        reg[A] = (uint32_t) floor(reg[B] / reg[C]);
                        break;
                case 6:
                        reg[A] = ~(reg[B] & reg[C]);
                        break;
                case 7:
                        free(reg); 
                        exit(EXIT_SUCCESS);
                        break;
                case 8:
                        map(segmem, &reg[B],
                                &reg[C]);
                        break;
                case 9:
                        unmap(segmem, &reg[C]);
                        break;
                case 10:
                        printf("%c", (char)reg[C]);
                        break;
                case 11:
                        ;
                        char in = (char) getc(stdin);
                        if (in == '\n') {
                                in = (char) getc(stdin);
                        }
                        if (in == EOF) {
                                reg[C] = 1;
                                break;
                        }
                        reg[C] = in;
                        break;
                case 12:
                        load_program(segmem, &counter, &reg[B], &reg[C]);
                        break;
                case 13:
                        reg[A] = value;
                        break;
                default:
                        fprintf(stderr, "Invalid Instruction\n");
                        exit(EXIT_FAILURE);
                        break;
        }
}