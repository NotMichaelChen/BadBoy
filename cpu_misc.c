
#include "cpu_internal.h"
#include "memory.h"
#include "bitmanip.h"

//0b0000 0000
void misc_nop(struct reg_type *reg) {
    reg->PC += 1;
}

//0b0001 0000
void misc_stop(struct reg_type *reg) {

    //TODO: Directly implement input reading to determine when to resume
    while(1)
        ;
    reg->PC += 1;
}


void misc_halt(struct reg_type *reg) {
}
