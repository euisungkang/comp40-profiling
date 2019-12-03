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
                }else {
                        A = (uint32_t) Bitpack_getu(zero[counter], 3, 25);
                        value = (uint32_t) Bitpack_getu(zero[counter],
                                                        25, 0);
                }

                switch(code) {
                        case 0:
                                if (registers[C] != 0) {
                                        registers[A] = registers[B];
                                }
                                break;
                        case 1:
                                load(segmem, &registers[A],
                                    registers[B], registers[C]);
                                break;
                        case 2:
                                store(segmem, &registers[A],
                                        &registers[B], &registers[C]);
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
                        case 8:
                                map(segmem, &registers[B],
                                        &registers[C]);
                                break;
                        case 9:
                                unmap(segmem, &registers[C]);
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
                                load_program(segmem, &counter, &registers[B], &registers[C]);
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