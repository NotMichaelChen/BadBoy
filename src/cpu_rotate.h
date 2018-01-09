#ifndef CPU_ROTATE_H
#define CPU_ROTATE_H

#include "cpu_internal.h"

void rotate_circular_left(unsigned char instr, struct reg_type *reg);
void rotate_circular_right(unsigned char instr, struct reg_type *reg);
void rotate_left(unsigned char instr, struct reg_type *reg);
void rotate_right(unsigned char instr, struct reg_type *reg);

#endif // CPU_ROTATE_H
