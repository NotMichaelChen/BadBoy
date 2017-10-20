#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include "cpu_internal.h"

void alloc_ram();
void load_cart(FILE *cartfile);
unsigned char readRAM(unsigned short address);
void writeRAM(unsigned short address, unsigned char value);
unsigned char *getRAMpointer(unsigned short address);
void stack_push(struct reg_type *reg, unsigned char value);
unsigned char stack_pop(struct reg_type *reg);

#endif // MEMORY_H
