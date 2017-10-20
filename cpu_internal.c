#include "cpu_internal.h"

#include <ctype.h>
#include <stdlib.h>

#include "memory.h"
#include "bitmanip.h"

struct reg_type new_reg()
{
    //Create the registers struct and initialize the values
    struct reg_type registers;
    registers.AF = malloc(sizeof(short));
    *(registers.AF) = 0x01B0;
    //These casts work since casting is higher precedence than addition
    registers.A = (unsigned char *)&registers.AF;
    registers.F = (unsigned char *)&registers.AF+1;
    registers.BC = malloc(sizeof(short));
    *(registers.BC) = 0x0013;
    registers.B = (unsigned char *)&registers.BC;
    registers.C = (unsigned char *)&registers.BC+1;
    registers.DE = malloc(sizeof(short));
    *(registers.DE) = 0x00D8;
    registers.D = (unsigned char *)&registers.DE;
    registers.E = (unsigned char *)&registers.DE+1;
    registers.HL = malloc(sizeof(short));
    *(registers.HL) = 0x014D;
    registers.H = (unsigned char *)&registers.HL;
    registers.L = (unsigned char *)&registers.HL+1;
    registers.SP = 0xFFFE;
    registers.PC = 0x100;

    return registers;
}

void dealloc_reg(struct reg_type *registers)
{
    free(registers->AF);
    free(registers->BC);
    free(registers->DE);
    free(registers->HL);
}

/// All flag operations will add the two numbers together
/// If you want to subtract, you must do that before passing the number in

//Set the zero flag
//To allow for many types of operations, only the result is passed into the
//function
void set_flag_Z(int result, struct reg_type *reg) {
    if(!result)
        *(reg->F) |= 0b10000000;
    else
        *(reg->F) &= 0b01111111;
}

/* Contrary to GBCPUman.pdf, the H/C flag is set on borrow.
 *
 * https://stackoverflow.com/questions/31409444/what-is-the-behavior-of-the-carry-flag-for-cp-on-a-game-boy/31415312
 *
 * The negate flag is used to negate the result of the calculation; if a carry
 * is detected when subtracting via 2's compliment, then there was no borrow,
 * and vice versa.
 * Therefor set the negate flag when doing a subtraction operation
 */

//Set the half carry flag
void set_flag_H(int a, int b, struct reg_type *reg, int is_sub) {
    //Due to the way binary addition works, result will either be 0x10 or 0x00
    unsigned char result = (((a&0xf) + (b&0xf))&0x10);
    result = is_sub ? !result : result;

    if(result)
        *(reg->F) |= 0b100000;
    else
        *(reg->F) &= 0b11011111;
}

//Set the carry flag
void set_flag_C(int a, int b, struct reg_type *reg, int is_sub) {
    //Due to the way binary addition works, result will either be 0x100 or 0x000
    //NOTE: Even if the operation is between a 16-bit and 8-bit value, the flag
    //is only set if there is a carry between the 7th and 8th bit
    unsigned short result = (((a&0xFF) + (b&0xFF))&0x100);
    result = is_sub ? !result : result;

    if(result)
        *(reg->F) |= 0b10000;
    else
        *(reg->F) &= 0b11101111;
}

//For convenience
void set_flag_N(struct reg_type *reg) {
    *(reg->F) |= 0b1000000;
}

//For convenience
void reset_flag(char flag, struct reg_type *reg) {
    flag = toupper(flag);

    switch(flag) {
    case 'Z':
        *(reg->F) &= 0b01111111;
        break;
    case 'N':
        *(reg->F) &= 0b10111111;
        break;
    case 'H':
        *(reg->F) &= 0b11011111;
        break;
    case 'C':
        *(reg->F) &= 0b11101111;
        break;
    //TODO: add some error if flag is invalid
    }
}

/// Register Decoding

//Turns a 3-bit binary number into the correct register
unsigned char *decode_three_bit(unsigned char num, struct reg_type *reg) {
    switch(num) {
    case 0b000:
        return reg->B;
    case 0b001:
        return reg->C;
    case 0b010:
        return reg->D;
    case 0b011:
        return reg->E;
    case 0b100:
        return reg->H;
    case 0b101:
        return reg->L;
    case 0b110:
        return getRAMpointer((*reg->HL));
    case 0b111:
        return reg->A;
    default:
        return NULL;
    }
}

//Turns a 2-bit binary number into the correct memory location
//Specific to 8-bit operations
unsigned char *decode_two_bit_pointer(unsigned char num, struct reg_type *reg) {
    unsigned char *memloc;
    switch(num) {
    case 0b00:
        memloc = getRAMpointer((*reg->B << 8) + *reg->C);
    case 0b01:
        memloc = getRAMpointer((*reg->D << 8) + *reg->E);
    case 0b10:
        memloc = getRAMpointer((*reg->H << 8) + *reg->L);
        *memloc += 1;
    case 0b11:
        memloc = getRAMpointer((*reg->H << 8) + *reg->L);
        *memloc -= 1;
    default:
        return NULL;
    }

    return memloc;
}

//Turns a 2-bit binary number into the correct register
//Specific to 16-bit operations
unsigned short *decode_two_bit(unsigned char num, struct reg_type *reg) {
    switch(num) {
    case 0b00:
        return reg->BC;
    case 0b01:
        return reg->DE;
    case 0b10:
        return reg->HL;
    case 0b11:
        return &(reg->SP);
    default:
        return NULL;
    }
}

//Turns a 2-bit binary number into the correct memory location
//Specific to 16-bit stack operations
unsigned short *decode_two_bit_stack(unsigned char num, struct reg_type *reg) {
    switch(num) {
    case 0b00:
        return reg->BC;
    case 0b01:
        return reg->DE;
    case 0b10:
        return reg->HL;
    case 0b11:
        return reg->AF;
    default:
        return NULL;
    }
}
