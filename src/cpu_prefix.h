#ifndef CPU_PREFIX_H
#define CPU_PREFIX_H

#include "cpu_internal.h"

void prefix_decode(unsigned char instr, struct reg_type *reg);

void prefix_rotate_circular_left(unsigned char instr, struct reg_type *reg);
void prefix_rotate_circular_right(unsigned char instr, struct reg_type *reg);

void prefix_testbit(unsigned char instr, struct reg_type *reg);
void prefix_resetbit(unsigned char instr, struct reg_type *reg);
void prefix_setbit(unsigned char instr, struct reg_type *reg);

#endif // CPU_PREFIX_H
