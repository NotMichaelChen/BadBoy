#ifndef CPU_H
#define CPU_H

#include "cpu_internal.h"

int power_up();
void cpu_loop();
struct reg_type init_mem();
void parse_instr(unsigned char instr, struct reg_type *reg);

#endif // CPU_H
