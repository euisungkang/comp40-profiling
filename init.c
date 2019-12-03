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
#include <seq.h>
#include <sys/stat.h>

#define REG_LENGTH 3
#define CODE_LENGTH 4
#define A_LSB 6
#define B_LSB 3
#define C_LSB 0
#define VAL_LSB 0
#define OPCODE_LSB 28
#define VAL_LENGTH 25
#define T Segment

typedef struct T {
        Seq_T m;
        Seq_T unmapped;
        int seg_count;
}*T;

void Seg_Free(T *);
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb);

int main(int argc, char *argv[])
{
        (void) argv;
        if (argc != 2) {
                fprintf(stderr, "Usage: ./um [program um file]\n");
                exit(EXIT_FAILURE);
        }
        T segmem = malloc(sizeof(struct T));
        segmem -> m = Seq_new(0);
        segmem -> unmapped = Seq_new(0);
        segmem -> seg_count = 0;

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
                short A = 0, B = 0, C = 0;
                uint32_t value = 0;
                
                uint32_t code = (uint32_t) Bitpack_getu(zero[counter],
                                                        CODE_LENGTH, OPCODE_LSB);
                if(code < 13) {
                        A = (uint32_t) Bitpack_getu(zero[counter], REG_LENGTH, A_LSB);
                        B = (uint32_t) Bitpack_getu(zero[counter], REG_LENGTH, B_LSB);
                        C = (uint32_t) Bitpack_getu(zero[counter], REG_LENGTH, C_LSB);
                } else {
                        A = (uint32_t) Bitpack_getu(zero[counter], REG_LENGTH, VAL_LENGTH);
                        value = (uint32_t) Bitpack_getu(zero[counter], VAL_LENGTH, VAL_LSB);
                }

                switch(code) {
                        case 0:
                                if (registers[C] != 0) {
                                        registers[A] = registers[B];
                                }
                                break;
                        case 1:;
                                uint32_t *dup = Seq_get(segmem -> m, registers[B]);
                                registers[A] = dup[registers[C] + 1];
                                break;
                        case 2:;
                                uint32_t *dest = Seq_get(segmem -> m, registers[A]);
                                dest[registers[B] + 1] = registers[C];
                                break;
                        case 3:
                                registers[A] = (registers[B] + registers[C]);
                                break;
                        case 4:
                                registers[A] = (registers[B] * registers[C]);
                                break;
                        case 5:
                                registers[A] = (uint32_t) floor(registers[B] / registers[C]);
                                break;
                        case 6:
                                registers[A] = ~(registers[B] & registers[C]);
                                break;
                        case 7:
                                free(registers); 
                                Seg_Free(&segmem);
                                exit(EXIT_SUCCESS);
                                break;
                        case 8:;
                                int size = registers[C];
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
                                registers[B] = id;                                
                                break;
                        case 9:;
                                uint32_t *id2 = malloc(sizeof(uint32_t));
                                *id2 = registers[C];
                                uint32_t *unused = Seq_get(segmem -> m, *id2);
                                free(unused);
                                Seq_put(segmem -> m, *id2, NULL);
                                Seq_addhi(segmem -> unmapped, (void *) id2);
                                break;
                        case 10:
                                printf("%c", (char)registers[C]);
                                break;
                        case 11:
                                ;
                                char in = (char) getc(stdin);
                                if (in == '\n') {
                                        in = (char) getc(stdin);
                                }
                                if (in == EOF) {
                                        registers[C] = 1;
                                        break;
                                }
                                registers[C] = in;
                                break;
                        case 12:
                                if (registers[B] == 0) {
                                        counter = registers[C] + 1;
                                        break;
                                }
                                uint32_t *zero_seg = Seq_get(segmem->m, 0);
                                uint32_t *duplicate = Seq_get(segmem -> m, registers[B]);
                                zero_seg = realloc(zero_seg, duplicate[0]);
                                memcpy(zero_seg, duplicate, duplicate[0] * sizeof(uint32_t));

                                counter = registers[C] + 1;
                                break;
                        case 13:
                                registers[A] = value;
                                break;
                        default:
                                fprintf(stderr, "Invalid Instruction\n");
                                exit(EXIT_FAILURE);
                                break;
                }

                if(code != 12) {
                        (counter)++;
                } else {
                        zero = Seq_get(segmem->m, 0);
                }
        }


        free(registers);
        exit(EXIT_SUCCESS);
}

void Seg_Free(T *segmem)
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