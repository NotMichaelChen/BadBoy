#ifndef CPU_JUMP_H
#define CPU_JUMP_H

#include "cpu_internal.h"

void jump_reg(unsigned char instr, struct reg_type *reg);
void jump_ret(unsigned char instr, struct reg_type *reg);
void jump_jp(unsigned char instr, struct reg_type *reg);
void jump_call(unsigned char instr, struct reg_type *reg);
void jump_rst(unsigned char instr, struct reg_type *reg);


#endif // CPU_JUMP_H
