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

typedef struct Segment {
        Seq_T m;
        Seq_T unmapped;
        int seg_count;
}*Segment;

void Seg_Free(Segment *);
Segment Seg_New();

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
                short A = 0, B = 0, C = 0;
                uint32_t value = 0;
                
                uint32_t code = (uint32_t) Bitpack_getu(zero[counter],
                                                        4, 28);
                if(code < 13) {
                        A = (uint32_t) Bitpack_getu(zero[counter], 3, 6);
                        B = (uint32_t) Bitpack_getu(zero[counter], 3, 3);
                        C = (uint32_t) Bitpack_getu(zero[counter], 3, 0);
                } else {
                        A = (uint32_t) Bitpack_getu(zero[counter], 3, 25);
                        value = (uint32_t) Bitpack_getu(zero[counter], 25, 0);
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
                                free(Seq_get(segmem->m, 0));
                                uint32_t *duplicate = Seq_get(segmem -> m, registers[B]);
                                
                                uint32_t size2 = duplicate[0];
                                uint32_t *zero_seg = malloc(size2 * sizeof(uint32_t));
                                memcpy(zero_seg, duplicate, size2 * sizeof(uint32_t));

                                Seq_put(segmem->m, 0, zero_seg);

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

Segment Seg_New()
{
        Segment segmem = malloc(sizeof(struct Segment));
        assert(segmem != NULL);
        segmem -> m = Seq_new(0);
        segmem -> unmapped = Seq_new(0);
        segmem -> seg_count = 0;
        return segmem;
}