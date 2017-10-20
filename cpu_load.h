#ifndef CPU_LOAD_H
#define CPU_LOAD_H

#include "cpu_internal.h"

//8-bit
void load_rr(unsigned char instr, struct reg_type *reg);
void load_rn(unsigned char instr, struct reg_type *reg);
void load_memr(unsigned char instr, struct reg_type *reg);
void load_io(unsigned char instr, struct reg_type *reg);
void load_addr(unsigned char instr, struct reg_type *reg);
//16-bit
void load_rnn(unsigned char instr, struct reg_type *reg);
void load_sp(unsigned char instr, struct reg_type *reg);
void load_pop(unsigned char instr, struct reg_type *reg);
void load_push(unsigned char instr, struct reg_type *reg);

#endif // CPU_LOAD_H
