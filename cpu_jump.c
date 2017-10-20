#include "cpu_jump.h"

#include <stdbool.h>

#include "cpu_internal.h"
#include "memory.h"
#include "bitmanip.h"

//Check condition, and jump to relative address if true
//0b00xx 0x00
void jump_reg(unsigned char instr, struct reg_type *reg) {
    //Are we checking for a condition?
    if(get_bits(instr, 2, 2)) {
        bool flag;
        //Flag C
        if(get_bits(instr, 3, 3)) {
            flag = get_bits(*(reg->F), 3, 3);
        }
        //Flag Z
        else {
            flag = get_bits(*(reg->F), 7, 7);
        }

        //Don't do the jump if the condition is not satisfied
        if(flag != get_bits(instr, 5, 5)) {
            return;
        }
    }

    reg->PC += (char)readRAM(reg->PC+1);
}

//Pop two bytes off the stack and jump to that location if the condition is true
//0b110x x00x
void jump_ret(unsigned char instr, struct reg_type *reg) {
    //Are we checking for a condition?
    if(!get_bits(instr, 7, 7)) {
        bool flag;
        //Flag C
        if(get_bits(instr, 3, 3)) {
            flag = get_bits(*(reg->F), 3, 3);
        }
        //Flag Z
        else {
            flag = get_bits(*(reg->F), 7, 7);
        }

        //Don't perform return if the condition is not satisfied
        if(flag != get_bits(instr, 4, 4)) {
            return;
        }
    }

    //Lower was pushed first, so we get upper first when we pop
    unsigned char upper = stack_pop(reg);
    unsigned char lower = stack_pop(reg);
    reg->PC = (upper << 8) + lower;

    //Enable Interrupts
    if(instr == 0xD9) {
        //TODO: Enable interrupt here
    }
}

//Jump to the given 16-bit address if condition is true
//0b110x x01x
void jump_jp(unsigned char instr, struct reg_type *reg) {
//Are we checking for a condition?
    if(!get_bits(instr, 7, 7)) {
        bool flag;
        //Flag C
        if(get_bits(instr, 3, 3)) {
            flag = get_bits(*(reg->F), 3, 3);
        }
        //Flag Z
        else {
            flag = get_bits(*(reg->F), 7, 7);
        }

        //Don't do the jump if the condition is not satisfied
        if(flag != get_bits(instr, 4, 4)) {
            return;
        }
    }

    unsigned short address;

    //Get address from (HL) instead
    //TODO: Does not follow standard jp pattern, resolve
    if(instr == 0xE9) {
        address = reg->HL;
    }
    else {
        address = (readRAM(reg->PC+2) << 8) + readRAM(reg->PC+1);
    }
    reg->PC = address;
}

//Push current address onto stack then jump to immediate address, if condition
//is true
//0b110x x10x
void jump_call(unsigned char instr, struct reg_type *reg) {
    //Are we checking for a condition?
    if(!get_bits(instr, 7, 7)) {
        bool flag;
        //Flag C
        if(get_bits(instr, 3, 3)) {
            flag = get_bits(*(reg->F), 3, 3);
        }
        //Flag Z
        else {
            flag = get_bits(*(reg->F), 7, 7);
        }

        //Don't do the jump if the condition is not satisfied
        if(flag != get_bits(instr, 4, 4)) {
            return;
        }
    }

    //We want to push the next instruction on the stack
    reg->PC += 3;
    //Push lower, then upper
    stack_push(reg, reg->PC % 0b100000000);
    stack_push(reg, reg->PC / 0b100000000);

    //Jump to immediate address
    reg->PC = (readRAM(reg->PC+2) << 8) + readRAM(reg->PC+1);
}

//Restarts by pushing current PC and jumping to 0x0000 + param
//0b11xx x111
void jump_rst(unsigned char instr, struct reg_type *reg) {
    //Increment PC before pushing
    reg->PC += 1;

    //Push lower, then upper
    stack_push(reg, reg->PC % 0b100000000);
    stack_push(reg, reg->PC / 0b100000000);

    unsigned char lower = (get_bits(instr, 2, 3) << 4) + (8 * get_bits(instr, 4, 4));
    reg->PC = lower;
}
