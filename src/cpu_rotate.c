#include "cpu_rotate.h"

#include "cpu_internal.h"

//TODO: Refactor rotates into one function

//Circularly rotates register A left. Old bit goes to carry flag
//0b0000 0111
void rotate_circular_left(unsigned char instr, struct reg_type *reg) {
    unsigned char result = (*(reg->A) << 1) | (*(reg->A) >> 7);

    //TODO: Determine if one-byte rotates assign the zero flag or not
    reset_flag('Z', reg);
    reset_flag('N', reg);
    reset_flag('H', reg);
    //Assign the Carry flag the old bit (which is the leftmost one)
    (*(reg->A) >> 7) ? set_flag('C', reg) : reset_flag('C', reg);

    *(reg->A) = result;

    reg->PC += 1;
}

//Circularly rotates register A right. Old bit goes to carry flag
//0b0000 1111
void rotate_circular_right(unsigned char instr, struct reg_type *reg) {
    unsigned char result = (*(reg->A) >> 1) | (*(reg->A) << 7);

    reset_flag('Z', reg);
    reset_flag('N', reg);
    reset_flag('H', reg);
    //Assign the Carry flag the old bit (which is the rightmost one)
    (*(reg->A) & 1) ? set_flag('C', reg) : reset_flag('C', reg);

    *(reg->A) = result;

    reg->PC += 1;
}

//Rotates register A left. Incoming bit comes from carry, outgoing bit goes to
//carry
//0b0001 0111
void rotate_left(unsigned char instr, struct reg_type *reg) {
    unsigned char result = (*(reg->A) << 1) | (get_flag('C', reg) >> 7);

    reset_flag('Z', reg);
    reset_flag('N', reg);
    reset_flag('H', reg);
    //Assign the Carry flag the old bit (which is the leftmost one)
    (*(reg->A) >> 7) ? set_flag('C', reg) : reset_flag('C', reg);

    *(reg->A) = result;

    reg->PC += 1;
}

//Rotates register A right. Incoming bit comes from carry, outgoing bit goes to
//carry
//0b0001 1111
void rotate_right(unsigned char instr, struct reg_type *reg) {
    unsigned char result = (*(reg->A) >> 1) | (get_flag('C', reg) << 7);

    reset_flag('Z', reg);
    reset_flag('N', reg);
    reset_flag('H', reg);
    //Assign the Carry flag the old bit (which is the rightmost one)
    (*(reg->A) & 1) ? set_flag('C', reg) : reset_flag('C', reg);

    *(reg->A) = result;

    reg->PC += 1;
}
