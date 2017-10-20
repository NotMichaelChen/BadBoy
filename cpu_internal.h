#ifndef CPU_INTERNAL_H
#define CPU_INTERNAL_H

#include <stdbool.h>

struct reg_type {
    //Actual memory values allocated on the heap
    unsigned short *AF;
    unsigned short *BC;
    unsigned short *DE;
    unsigned short *HL;
    //Pointers to individual bytes
    unsigned char *A;
    unsigned char *F;
    unsigned char *B;
    unsigned char *C;
    unsigned char *D;
    unsigned char *E;
    unsigned char *H;
    unsigned char *L;
    //Stack pointer
    //Points at the top of the stack, not the next available space
    unsigned short SP;
    //Program Counter
    unsigned short PC;
};

struct reg_type new_reg();
void dealloc_reg(struct reg_type *registers);

//Flag setting
void set_flag_Z(int result, struct reg_type *reg);
void set_flag_H(int a, int b, struct reg_type *reg, int negate);
void set_flag_C(int a, int b, struct reg_type *reg, int negate);
void set_flag_N(struct reg_type *reg);
bool get_flag(char flag, struct reg_type *reg);
void reset_flag(char flag, struct reg_type *reg);

//Register Decoding
unsigned char *decode_three_bit(unsigned char num, struct reg_type *reg);
unsigned char *decode_two_bit_pointer(unsigned char num, struct reg_type *reg);
unsigned short *decode_two_bit(unsigned char num, struct reg_type *reg);
unsigned short *decode_two_bit_stack(unsigned char num, struct reg_type *reg);

#endif // CPU_INTERNAL_H
