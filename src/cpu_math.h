#ifndef CPU_MATH_H
#define CPU_MATH_H

#include "cpu_internal.h"

void math_incdec(unsigned char instr, struct reg_type *reg);
void math_arith(unsigned char instr, struct reg_type *reg);
void math_binop(unsigned char instr, struct reg_type *reg);
void math_cp(unsigned char instr, struct reg_type *reg);
void math_opconst(unsigned char instr, struct reg_type *reg);
void math_misc(unsigned char instr, struct reg_type *reg);

void math_double_incdec(unsigned char instr, struct reg_type *reg);
void math_double_add(unsigned char instr, struct reg_type *reg);
void math_addSP(unsigned char instr, struct reg_type *reg);

#endif // CPU_MATH_H
