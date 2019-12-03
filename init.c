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

typedef struct UM {
        Segment segmem;
        uint32_t *registers;
        uint32_t counter;
} *UM;

/* Function Prototypes */
UM UM_new();
void UM_free(UM *);
void run_instructions(UM, short, short, short, short, uint32_t);
uint32_t *read_program(UM, char *);

int main(int argc, char *argv[])
{
        (void) argv;
        if (argc != 2) {
                fprintf(stderr, "Usage: ./um [program um file]\n");
                exit(EXIT_FAILURE);
        }

        UM um = UM_new();

        FILE *input = fopen(argv[1], "r");
        struct stat sb;
        assert(stat(argv[1], &sb) == 0);
        int size = sb.st_size/4 + 1;
        uint32_t *m0 = malloc(sizeof(uint32_t)*size);
        char byteArray[4] = {0};
        int counter = 1;
        int i = 3;

        m0[0] = size;
        while(fscanf(input, "%c", &(byteArray[i])) == 1) {
                if(i == 0) {
                        m0[counter++] = *(uint32_t *) byteArray;
                        i = 4;
                }
                i--;
        }
        fclose(input);
        
        Seq_addhi(um->segmem->m, m0);
        um->segmem->seg_count++;


        uint32_t *zero = Seq_get(um->segmem->m, 0);
        while(um->counter < zero[0]) {
                short ra = 0, rb = 0, rc = 0;
                uint32_t value = 0;
                
                uint32_t code = (uint32_t) Bitpack_getu(zero[um -> counter],
                                                        4, 28);
                if(code < 13) {
                        ra = (uint32_t) Bitpack_getu(zero[um -> counter], 3, 6);
                        rb = (uint32_t) Bitpack_getu(zero[um -> counter], 3, 3);
                        rc = (uint32_t) Bitpack_getu(zero[um -> counter], 3, 0);
                }else {
                        ra = (uint32_t) Bitpack_getu(zero[um -> counter], 3, 25);
                        value = (uint32_t) Bitpack_getu(zero[um -> counter],
                                                        25, 0);
                }

                run_instructions(um, code, ra, rb, rc, value);
                if(code != 12) {
                        (um->counter)++;
                } else {
                        zero = Seq_get(um->segmem->m, 0);
                }
        }
        exit(EXIT_SUCCESS);
}

UM UM_new()
{
        UM um = malloc(sizeof(struct UM));
        um -> segmem = Seg_New();
        um -> registers = calloc(8, sizeof(uint32_t));
        um -> counter = 1;
        return um;
}

/* Input: UM, opcode, register index A, B, C, value
   Determines which instruction is run through the opcode. A, B, C
   are passed in as actual register addresses. Entire condition structure
   is switch cases */
void run_instructions(UM um, short opcode, short A, short B,
                        short C, uint32_t value)
{
        switch(opcode) {
                case 0:
                        if (um -> registers[C] != 0) {
                                um -> registers[A] = um -> registers[B];
                        }
                        break;
                case 1:
                        load(um -> segmem, &um -> registers[A],
                            um -> registers[B], um -> registers[C]);
                        break;
                case 2:
                        store(um -> segmem, &um -> registers[A],
                                &um -> registers[B], &um -> registers[C]);
                        break;
                case 3:
                        um -> registers[A] = (um -> registers[B] + um -> registers[C]);
                        break;
                case 4:
                        um -> registers[A] = (um -> registers[B] * um -> registers[C]);
                        break;
                case 5:
                        um -> registers[A] = (uint32_t) floor(um -> registers[B] / um -> registers[C]);
                        break;
                case 6:
                        um -> registers[A] = ~(um -> registers[B] & um -> registers[C]);
                        break;
                case 7:
                        UM_free(&um);
                        exit(EXIT_SUCCESS);
                        break;
                case 8:
                        map(um -> segmem, &um -> registers[B],
                                &um -> registers[C]);
                        break;
                case 9:
                        unmap(um -> segmem, &um -> registers[C]);
                        break;
                case 10:
                        printf("%c", (char)um -> registers[C]);
                        break;
                case 11:
                        ;
                        char in = (char) getc(stdin);
                        if (in == '\n') {
                                in = (char) getc(stdin);
                        }
                        if (in == EOF) {
                                um -> registers[C] = 1;
                                break;
                        }
                        um -> registers[C] = in;
                        break;
                case 12:
                        load_program(um -> segmem, &um -> counter, &um ->
                                     registers[B], &um -> registers[C]);
                        break;
                case 13:
                        um -> registers[A] = value;
                        break;
                default:
                        fprintf(stderr, "Invalid Instruction\n");
                        exit(EXIT_FAILURE);
                        break;
        }
}

/* Input: UM *
   Free's the UM machine. Free's the segmem as well using Seg_Free */
void UM_free(UM *um)
{
    Seg_Free(&((*um) -> segmem));
    free((*um) -> registers);
    free(*um);
}