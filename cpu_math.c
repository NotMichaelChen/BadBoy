#include "cpu_math.h"

#include <stdbool.h>

#include "cpu_internal.h"
#include "memory.h"
#include "bitmanip.h"

//Increment or decrement the given register
//0b00xx x10x
void math_incdec(unsigned char instr, struct reg_type *reg) {
    unsigned char srcnum = get_bits(instr, 2, 4);
    unsigned char *srcptr = decode_three_bit(srcnum, reg);

    //0 = inc, 1 = dec
    char crementer = 1 - 2 * get_bits(instr, 7, 7);

    //Set flags before assigning the final result
    set_flag_Z(*srcptr + crementer, reg);
    crementer > 0 ? reset_flag('N', reg) : set_flag_N(reg);
    set_flag_H(*srcptr, crementer, reg, crementer < 0);

    *srcptr += crementer;

    reg->PC += 1;
}

//Add or subtract the given register to/from A, including a carry when specified
//0b100s cxxx
void math_arith(unsigned char instr, struct reg_type *reg) {
    unsigned char srcnum = get_bits(instr, 5, 7);
    unsigned char *srcptr = decode_three_bit(srcnum, reg);
    short val = *srcptr;

    unsigned char is_carry = get_bits(instr, 4, 4);
    unsigned char carry = get_bits(*(reg->F), 3, 3);

    unsigned char is_sub = get_bits(instr, 3, 3);

    //Must come before negation for subtraction, otherwise we get "A - n + c"
    //instead of "A - n - c" or "A - (n + c)"
    if(is_carry)
        val += carry;
    //1001 0xxx = Sub
    if(is_sub)
        val = -val;


    set_flag_Z(*(reg->A) + val, reg);
    is_sub ? set_flag_N(reg) : reset_flag('N', reg);
    set_flag_H(*(reg->A), val, reg, is_sub);
    set_flag_C(*(reg->A), val, reg, is_sub);

    *(reg->A) += val;

    reg->PC += 1;
}

//Perform the binary operation with A, storing in A
//0b101x xyyy
void math_binop(unsigned char instr, struct reg_type *reg) {
    unsigned char srcnum = get_bits(instr, 5, 7);
    unsigned char *srcptr = decode_three_bit(srcnum, reg);

    unsigned char optype = get_bits(instr, 3, 4);
    unsigned char result;

    if(optype == 0)
        result = *(reg->A) & (*srcptr);
    else if(optype == 1)
        result = *(reg->A) ^ (*srcptr);
    else if(optype == 2)
        result = *(reg->A) | (*srcptr);

    set_flag_Z(result, reg);
    reset_flag('N', reg);
    //Only set the H flag if we did an 'and'. Forces a set by adding 0xF and 0xF
    optype == 0 ? set_flag_H(0xf, 0xf, reg, 0) : reset_flag('H', reg);
    reset_flag('C', reg);

    *(reg->A) = result;

    reg->PC += 1;
}

//Compare A with the specified register
//0b1011 1xxx
void math_cp(unsigned char instr, struct reg_type *reg) {
    unsigned char srcnum = get_bits(instr, 5, 7);
    unsigned char *srcptr = decode_three_bit(srcnum, reg);

    //Only sets flags, don't assign result of operation
    set_flag_Z(*(reg->A) - (*srcptr), reg);
    set_flag_N(reg);
    set_flag_H(*(reg->A), -(*srcptr), reg, 1);
    set_flag_C(*(reg->A), -(*srcptr), reg, 1);

    reg->PC += 1;
}

//Perform the given operation using the immediate constant
//0b11xx x110
void math_opconst(unsigned char instr, struct reg_type *reg) {
    unsigned char val = readRAM(reg->PC+1);
    unsigned char op = get_bits(instr, 2, 4);

    //TODO: Find a way to remove redundant code
    switch(op) {
    //Add
    case 0b000:
        set_flag_Z(*(reg->A) + val, reg);
        reset_flag('N', reg);
        set_flag_H(*(reg->A), val, reg, 0);
        set_flag_C(*(reg->A), val, reg, 0);

        *(reg->A) += val;

        break;
    //Subtract
    case 0b001:
        set_flag_Z(*(reg->A) - val, reg);
        set_flag_N(reg);
        set_flag_H(*(reg->A), -val, reg, 1);
        set_flag_C(*(reg->A), -val, reg, 1);

        *(reg->A) -= val;

        break;
    //And
    case 0b010:
        set_flag_Z(*(reg->A) & val, reg);
        reset_flag('N', reg);
        //Forces a set by adding 0xF and 0xF
        set_flag_H(0xf, 0xf, reg, 0);
        reset_flag('C', reg);

        *(reg->A) &= val;

        break;
    //Or
    case 0b011:
        set_flag_Z(*(reg->A) | val, reg);
        reset_flag('N', reg);
        reset_flag('H', reg);
        reset_flag('C', reg);

        *(reg->A) |= val;

        break;
    //ADC
    case 0b100:
        //Add carry
        val += get_bits(*(reg->F), 3, 3);

        set_flag_Z(*(reg->A) + val, reg);
        reset_flag('N', reg);
        set_flag_H(*(reg->A), val, reg, 0);
        set_flag_C(*(reg->A), val, reg, 0);

        *(reg->A) += val;

        break;
    //SBC
    case 0b101:
        //Add carry. Works since val gets negated later
        val += get_bits(*(reg->F), 3, 3);

        set_flag_Z(*(reg->A) - val, reg);
        set_flag_N(reg);
        set_flag_H(*(reg->A), -val, reg, 1);
        set_flag_C(*(reg->A), -val, reg, 1);

        *(reg->A) -= val;

        break;
    //XOR
    case 0b110:
        set_flag_Z(*(reg->A) ^ val, reg);
        reset_flag('N', reg);
        reset_flag('H', reg);
        reset_flag('C', reg);

        *(reg->A) ^= val;

        break;
    //CP
    case 0b111:
        set_flag_Z(*(reg->A) - val, reg);
        set_flag_N(reg);
        set_flag_H(*(reg->A), -val, reg, 1);
        set_flag_C(*(reg->A), -val, reg, 1);

        break;
    }

    reg->PC += 2;
}

//Miscellaneous math operations (Decimal adjust, Complement A, Complement C,
//Complement C)
//0b001x x111
void math_misc(unsigned char instr, struct reg_type *reg) {
    unsigned char op = get_bits(instr, 3, 4);

    switch(op) {
    //Decimal Adjust A (DAA)
    //Pseudocode from http://gbdev.gg8.se/wiki/articles/Main_Page
    case 0b00:
        if(get_flag('N', reg)) {
            if(get_flag('C', reg))
                *(reg->A) -= 0x60;
            if(get_flag('H', reg))
                *(reg->A) -= 0x06;
        }
        else {
            if (get_flag('C', reg) || (*(reg->A) & 0xFF) > 0x99) {
                *(reg->A) += 0x60;
                set_flag('C', reg);
            }
            if (get_flag('H', reg) || (*(reg->A) & 0x0F) > 0x09)
                *(reg->A) += 0x06;
        }

        set_flag_Z(*(reg->A), reg);
        reset_flag('H', reg);
    //Complement A (CPL)
    case 0b01:
        *(reg->A) = ~(*(reg->A));
        set_flag_N(reg);
        set_flag_H(0xf, 0xf, reg, 0);
        break;
    //Set Carry Flag (SCF)
    case 0b10:
        set_flag_C(0xff, 0xff, reg, 0);
        reset_flag('N', reg);
        reset_flag('H', reg);
        break;
    //Complement Carry Flag (CCF)
    case 0b11:
        //If carry flag is set, this will invert it due to the behavior of the
        //negate flag in set_flag
        set_flag_C(0xff, 0xff, reg, get_bits(*(reg->F), 3, 3));
        reset_flag('N', reg);
        reset_flag('H', reg);
        break;
    }

    reg->PC += 1;
}
