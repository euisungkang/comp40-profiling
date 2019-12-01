/*
 * umlab.c
 * 
 * The functions defined in this lab should be linked against umlabwrite.c
 * to produce a unit test writing program. Any additional functions and unit
 * tests written for the lab go here.
 */

#include <stdint.h>
#include <stdio.h>

#include <assert.h>
#include <seq.h>

#include "bitpack.h"

/* RA, RB, RC all length 3 */
const unsigned int RA_WIDTH    =  3;
const unsigned int RB_WIDTH    =  3;
const unsigned int RC_WIDTH    =  3;
const unsigned int RA_LSB       =  6;
const unsigned int RB_LSB       =  3;
const unsigned int RC_LSB       =  0;

const unsigned int OPCODE_WIDTH =  4;
const unsigned int OPCODE_LSB   = 28;

const unsigned int VALUE_WIDTH  = 25;
const unsigned int LV_LSB       = 25;
const unsigned int BYTE_SIZE    =  8;

typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;
typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
static inline Um_instruction loadval(unsigned ra, unsigned val);

/* Functions for working with streams */

static inline void emit(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        uint64_t instruction = 0;

        /* Create the 32 bit instruction */
        instruction = Bitpack_newu(instruction, RA_WIDTH, RA_LSB, ra);
        instruction = Bitpack_newu(instruction, RA_WIDTH, RB_LSB, rb);
        instruction = Bitpack_newu(instruction, RA_WIDTH, RC_LSB, rc);
        instruction = Bitpack_newu(instruction, OPCODE_WIDTH, OPCODE_LSB, op);

        return (Um_instruction)instruction;
}

extern void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);

        uint64_t instruction;
        char temp;
        unsigned num_bytes = 32 / BYTE_SIZE;

        for (int i = 0; i < Seq_length(stream); i++) {
                instruction = (uintptr_t)Seq_get(stream, i);
                for (unsigned j = 1; j <= num_bytes; j++) {
                        temp = Bitpack_getu(instruction, BYTE_SIZE, 
                                            (num_bytes - j) * BYTE_SIZE);
                        putc(temp, output);
                }
        }
}

static inline Um_instruction cmove(Um_register ra, Um_register rb, Um_register rc) {
        return three_register(CMOV, ra, rb, rc);
}

static inline Um_instruction segload(Um_register ra, Um_register rb, Um_register rc) {
        return three_register(SLOAD, ra, rb, rc);
}

static inline Um_instruction segstore(Um_register ra, Um_register rb, Um_register rc) {
        return three_register(SSTORE, ra, rb, rc);
}

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction multiply(Um_register a, Um_register b, Um_register c) 
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction division(Um_register a, Um_register b, Um_register c) 
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction nand(Um_register a, Um_register b, Um_register c) 
{
        return three_register(NAND, a, b, c);
}

static inline Um_instruction halt() 
{
        return three_register(HALT, 0, 0, 0);
}

static inline Um_instruction map(Um_register rb, Um_register rc) 
{
        return three_register(ACTIVATE, 0, rb, rc);
}

static inline Um_instruction unmap(Um_register rc) 
{
        return three_register(INACTIVATE, 0, 0, rc);
}

static inline Um_instruction output(Um_register c) 
{
        return three_register(OUT, 0, 0, c);
}

static inline Um_instruction input(Um_register c) 
{
        return three_register(IN, 0, 0, c);
}

static inline Um_instruction loadprogram(Um_register b, Um_register c) 
{
        return three_register(LOADP, 0, b, c);
}

static inline Um_instruction loadval(Um_register ra, Um_register val)
{
        uint64_t ins = 0;

        ins = Bitpack_newu(ins, OPCODE_WIDTH, OPCODE_LSB, LV);
        ins = Bitpack_newu(ins, RA_WIDTH, LV_LSB, ra);
        ins = Bitpack_newu(ins, VALUE_WIDTH, 0, val);

        return (Um_instruction)ins;
}

void add_test(Seq_T stream)
{
        emit(stream, add(r1, r2, r3));
        emit(stream, halt());
}

void emit_halt_test(Seq_T stream)
{
        emit(stream, halt());
}

void emit_verbose_halt_test(Seq_T stream)
{
        emit(stream, halt());
        emit(stream, loadval(r1, 'B'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, 'a'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, 'd'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, '!'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, '\n'));
        emit(stream, output(r1));
}

void emit_tests_six(Seq_T stream)
{
        emit(stream, loadval(r1, 48));
        emit(stream, loadval(r2,  6));
        emit(stream, add(r3, r1, r2));
        emit(stream, output(r3));
        emit(stream, halt());
}

void emit_map_test(Seq_T stream)
{
        emit(stream, loadval(r1, 10));
        emit(stream, loadval(r2, 20));
        emit(stream, map(r1, r2));
        emit(stream, loadval(r2, 48));
        emit(stream, add(r3, r1, r2));
        emit(stream, output(r3));
        emit(stream, halt());
}

void emit_unmap_test(Seq_T stream)
{
        emit(stream, loadval(r1, 10));
        emit(stream, loadval(r2, 20));
        emit(stream, map(r1, r2));
        emit(stream, loadval(r2, 48));
        emit(stream, add(r3, r1, r2));
        emit(stream, output(r3));
        emit(stream, unmap(r1));
        emit(stream, loadval(r2, 5));
        emit(stream, loadval(r3, 65));
        emit(stream, segstore(r1, r2, r3));

        emit(stream, halt());
}

void emit_map_unmap_test(Seq_T stream)
{
        emit(stream, loadval(r1, 10));
        emit(stream, loadval(r2, 20));
        emit(stream, map(r1, r2));
        emit(stream, map(r1, r2));
        emit(stream, loadval(r2, 48));
        emit(stream, add(r3, r1, r2));
        emit(stream, output(r3));
        emit(stream, unmap(r1));
        emit(stream, loadval(r1, 1));
        emit(stream, loadval(r2, 5));
        emit(stream, loadval(r3, 65));
        emit(stream, segstore(r1, r2, r3));
        emit(stream, segload(r4, r1, r2));
        emit(stream, output(r4));
        emit(stream, loadval(r2, 20));
        emit(stream, map(r1, r2));
        emit(stream, loadval(r2, 48));
        emit(stream, add(r3, r1, r2));
        emit(stream, output(r3));

        emit(stream, halt());
}
void emit_seg_test(Seq_T stream)
{
        emit(stream, loadval(r1, 10));
        emit(stream, loadval(r2, 20));
        emit(stream, map(r1, r2));
        emit(stream, map(r1, r2));
        emit(stream, loadval(r2, 5));
        emit(stream, loadval(r3, 65));
        emit(stream, segstore(r1, r2, r3));
        emit(stream, loadval(r3, 66));
        emit(stream, loadval(r2, 8));
        emit(stream, segstore(r1, r2, r3));
        emit(stream, loadval(r2, 5));
        emit(stream, segload(r4, r1, r2));
        emit(stream, output(r4));
        emit(stream, loadval(r2, 8));
        emit(stream, segload(r4, r1, r2));
        emit(stream, output(r4));
        emit(stream, halt());
}

void emit_io_test(Seq_T stream)
{
        emit(stream, input(r1));
        emit(stream, input(r0));
        emit(stream, output(r1));
        emit(stream, output(r0));
        emit(stream, halt());
}

void emit_op_test(Seq_T stream)
{
        /*Add */
        emit(stream, loadval(r0, 65));
        emit(stream, loadval(r1, 1));
        emit(stream, add(r2, r1, r0));
        emit(stream, output(r2));
        emit(stream, add(r2, r0, r1));
        emit(stream, output(r2));

        /*Multiply */
        emit(stream, loadval(r0, 5));
        emit(stream, loadval(r1, 13));
        emit(stream, multiply(r2, r1, r0));
        emit(stream, output(r2));
        emit(stream, multiply(r2, r0, r1));
        emit(stream, output(r2));

        /*Division */
        emit(stream, loadval(r0, 5));
        emit(stream, loadval(r1, 325));
        emit(stream, division(r2, r1, r0));
        emit(stream, output(r2));

        emit(stream, halt());
}

void emit_nand_test(Seq_T stream)
{
        emit(stream, loadval(r0, 1));
        emit(stream, loadval(r1, 0));
        emit(stream, nand(r2, r0, r1));
        emit(stream, nand(r3, r1, r0));
        emit(stream, loadval(r5, 67));
        emit(stream, add(r4, r2, r5));
        emit(stream, output(r4));
        emit(stream, loadval(r5, 67));
        emit(stream, add(r4, r3, r5));
        emit(stream, output(r4));

        emit(stream, loadval(r0, 1));
        emit(stream, loadval(r1, 1));
        emit(stream, nand(r2, r0, r1));
        emit(stream, loadval(r5, 67));
        emit(stream, add(r4, r2, r5));
        emit(stream, output(r4));

        emit(stream, loadval(r0, 0));
        emit(stream, loadval(r1, 0));
        emit(stream, nand(r2, r0, r1));
        emit(stream, loadval(r5, 67));
        emit(stream, add(r4, r2, r5));
        emit(stream, output(r4));

        emit(stream, halt());
}

void emit_cmov_test(Seq_T stream)
{
        emit(stream, loadval(r0, 0));
        emit(stream, loadval(r1, 65));
        emit(stream, loadval(r2, 66));
        emit(stream, cmove(r1, r2, r0));
        emit(stream, output(r1));
        emit(stream, loadval(r0, 1));
        emit(stream, loadval(r1, 65));
        emit(stream, loadval(r2, 66));
        emit(stream, cmove(r1, r2, r0));
        emit(stream, output(r1));

        emit(stream, halt());
}

void emit_lprog_test(Seq_T stream)
{
        emit(stream, loadval(r1, 0));
        emit(stream, loadval(r0, 11));
        emit(stream, loadprogram(r1, r0));
        emit(stream, halt());
        emit(stream, halt());
        emit(stream, halt());
        emit(stream, halt());
        emit(stream, halt());
        emit(stream, halt());
        emit(stream, halt());
        emit(stream, loadval(r2, 65));
        emit(stream, output(r2));
        emit(stream, halt());
}