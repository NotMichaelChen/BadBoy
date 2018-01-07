#include "cpu_prefix.h"

#include "cpu_internal.h"
#include "memory.h"
#include "bitmanip.h"

//Decodes the next byte of the 2-byte instruction
void prefix_decode(unsigned char instr, struct reg_type *reg) {
    unsigned char instr_type = get_bits(instr, 0, 1);

    switch(instr_type) {
    case 0:; //semicolon allows for variable declaration in switch
        unsigned char sub_instr_type = get_bits(instr, 2, 4);

        switch(sub_instr_type) {
            case 0:
                prefix_rotate_circular_left(instr, reg);
                break;
            case 1:
                prefix_rotate_circular_right(instr, reg);
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
        }

        break;
    case 1:
        prefix_testbit(instr, reg);
        break;
    case 2:
        prefix_resetbit(instr, reg);
        break;
    case 3:
        prefix_setbit(instr, reg);
        break;
    }

    //Since we increment PC here, do not do it in any of the prefix functions
    reg->PC += 2;
}

//Circularly rotates the given register left. Overflow bit goes to Carry flag
//0b0000 0xxx
void prefix_rotate_circular_left(unsigned char instr, struct reg_type *reg) {
    unsigned char reg_num = get_bits(instr, 5, 7);
    unsigned char *src_reg = decode_three_bit(reg_num, reg);

    unsigned char result = (*src_reg << 1) | (*src_reg >> 7);

    //Assign flags before assigning the final result
    set_flag_Z(result, reg);
    reset_flag('N', reg);
    reset_flag('H', reg);
    //Assign the Carry flag the old bit (which is the leftmost one)
    *src_reg >> 7 ? set_flag('C', reg) : reset_flag('C', reg);

    *src_reg = result;
}

//Circularly rotates the given register right. Overflow bit goes to Carry flag
//0b0000 1xxx
void prefix_rotate_circular_right(unsigned char instr, struct reg_type *reg) {
    unsigned char reg_num = get_bits(instr, 5, 7);
    unsigned char *src_reg = decode_three_bit(reg_num, reg);

    unsigned char result = (*src_reg >> 1) | (*src_reg << 7);

    //Assign flags before assigning the final result
    set_flag_Z(result, reg);
    reset_flag('N', reg);
    reset_flag('H', reg);
    //Assign the Carry flag the old bit (which is the rightmost one)
    (*src_reg & 1) ? set_flag('C', reg) : reset_flag('C', reg);

    *src_reg = result;
}

//Tests bit b in register r
//0b01bb brrr
void prefix_testbit(unsigned char instr, struct reg_type *reg) {
    unsigned char bit = get_bits(instr, 2, 4);
    unsigned char reg_num = get_bits(instr, 5, 7);

    unsigned char *test_reg = decode_three_bit(reg_num, reg);

    set_flag_Z(*test_reg & (1 << bit), reg);
    reset_flag('N', reg);
    set_flag('H', reg);
}

//Reset bit b in register r
//0b10bb brrr
void prefix_resetbit(unsigned char instr, struct reg_type *reg) {
    unsigned char bit = get_bits(instr, 2, 4);
    unsigned char reg_num = get_bits(instr, 5, 7);

    unsigned char *reset_reg = decode_three_bit(reg_num, reg);

    *reset_reg &= ~(1 << bit);
}

//Set bit b in register r
//0b11bb brrr
void prefix_setbit(unsigned char instr, struct reg_type *reg) {
    unsigned char bit = get_bits(instr, 2, 4);
    unsigned char reg_num = get_bits(instr, 5, 7);

    unsigned char *set_reg = decode_three_bit(reg_num, reg);

    *set_reg |= (1 << bit);
}
