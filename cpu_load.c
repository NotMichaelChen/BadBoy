#include "cpu_load.h"

#include "cpu_internal.h"
#include "memory.h"
#include "bitmanip.h"

/// 8-bit loads

//Load register to register, (HL) included
//0b10xx xxxx
void load_rr(unsigned char instr, struct reg_type *reg) {
    unsigned char srcnum = get_bits(instr, 5, 7);
    unsigned char dstnum = get_bits(instr, 2, 4);

    unsigned char *srcptr = decode_three_bit(srcnum, reg);
    unsigned char *dstptr = decode_three_bit(dstnum, reg);

    *dstptr = *srcptr;

    reg->PC += 1;
}

//Load immediate 8-bit value to register, (HL) included
//0b00xx x110
void load_rn(unsigned char instr, struct reg_type *reg) {
    unsigned char dstnum = get_bits(instr, 2, 4);

    unsigned char *dstptr = decode_three_bit(dstnum, reg);
    *dstptr = readRAM(reg->PC + 1);

    reg->PC += 2;
}

//Load register memory location to A (or vice versa)
//0b00xx x010
void load_memr(unsigned char instr, struct reg_type *reg) {
    unsigned char addrnum = get_bits(instr, 2, 3);

    unsigned char *addrptr = decode_two_bit_pointer(addrnum, reg);

    unsigned char direction = get_bits(instr, 4, 4);

    //(XX)->A
    if(direction) {
        *(reg->A) = *addrptr;
    }
    else {
        *addrptr = *(reg->A);
    }

    reg->PC += 1;
}

//Load value + 0xFF00 to A (or vice versa)
//0b111x 00x0
void load_io(unsigned char instr, struct reg_type *reg) {
    unsigned short address;

    unsigned char srcloc = get_bits(instr, 6, 6);
    //Increment PC when we know how much to increment by
    //register C
    if(srcloc) {
        address = 0xFF00 + *(reg->C);
        reg->PC += 1;
    }
    //immediate value
    else {
        address = 0xFF00 + readRAM(reg->PC + 1);
        reg->PC += 2;
    }

    unsigned char direction = get_bits(instr, 3, 3);
    //val->A
    if(direction) {
        *(reg->A) = readRAM(address);
    }
    //A->val
    else {
        writeRAM(address, *(reg->A));
    }
}

//Load immediate memory location to A (or vice versa)
//0b111x 1010
void load_addr(unsigned char instr, struct reg_type *reg) {
    unsigned short address = (readRAM(reg->PC + 2) << 8) + readRAM(reg->PC + 1);

    unsigned char direction = get_bits(instr, 3, 3);
    //loc->A
    if(direction) {
        *(reg->A) = readRAM(address);
    }
    //A->loc
    else {
        writeRAM(address, *(reg->A));
    }

    reg->PC += 3;
}

/// 16-bit loads

//Load immediate 16-bit value to register
//0b00xx 0001
void load_rnn(unsigned char instr, struct reg_type *reg) {
    unsigned char dstnum = get_bits(instr, 2, 3);

    unsigned short *dstptr = decode_two_bit(dstnum, reg);
    *dstptr = (readRAM(reg->PC + 2) << 8) + (readRAM(reg->PC + 1));

    reg->PC += 3;
}

//Load stack pointer in different ways
//0bxxxx 100x
void load_sp(unsigned char instr, struct reg_type *reg) {
    //Easier just to brute-check opcode until I find a pattern
    switch(instr) {
    //Load SP into memory
    case 0x08: {
        unsigned short address = ((reg->PC+1) << 8) + (reg->PC+2);
        writeRAM(address, reg->PC % 100000000);
        writeRAM(address+1, reg->PC / 100000000);

        reg->PC += 3;
        break;
    }
    //Load SP into HL plus an extra byte
    case 0xF8: {
        char val = (char)readRAM(reg->PC+1);
        reg->HL = reg->SP + val;

        reset_flag('Z', reg);
        reset_flag('N', reg);
        set_flag_C(reg->SP, val, reg, 0);
        set_flag_H(reg->SP, val, reg, 0);

        reg->PC += 2;
        break;

    }
    //Load HL into SP
    case 0xF9:
        reg->SP = reg->HL;

        reg->PC += 1;
        break;
    }

}

//Pop two bytes off the stack into the specified register
//0b11xx 0001
void load_pop(unsigned char instr, struct reg_type *reg) {
    unsigned char dstnum = get_bits(instr, 2, 3);

    unsigned short *dstptr = decode_two_bit_stack(dstnum, reg);

    //Lower was pushed first, so we get upper first when we pop
    unsigned char upper = stack_pop(reg);
    unsigned char lower = stack_pop(reg);
    *dstptr = (upper << 8) + lower;

    reg->PC += 1;
}

//Push two bytes from the specified register onto the stack
//0b11xx 0101
void load_push(unsigned char instr, struct reg_type *reg) {
    unsigned char srcnum = get_bits(instr, 2, 3);

    unsigned short *srcptr = decode_two_bit_stack(srcnum, reg);

    //Push lower, then upper
    stack_push(reg, *srcptr % 100000000);
    stack_push(reg, *srcptr / 100000000);

    reg->PC += 1;
}
