#include "cpu.h"

#include "memory.h"
#include "bitmanip.h"

//LR35902 is LITTLE endian

//Make sure that memory is initialized and a cart is loaded before running the
//CPU

//TODO: Most emulators skip this, decide whether to keep or not
int power_up() {

    //TODO: Display nintendo logo

    //Lookup table
    unsigned char logo[48] = {0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
                                0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
                                0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
                                0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
                                0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
                                0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};

    int j = 0;
    for(int i = 0x104; i <= 0x133; i++) {
        if(logo[j] != readRAM(i))
            return -1;
        j++;
    }

    //must be unsigned since memory is unsigned
    unsigned int sum = 0;
    for(int i = 0x134; i <= 0x14D; i++) {
        sum += readRAM(i);
    }
    if((sum + 25) % 2 != 0)
        return -2;

    return 0;
}

void cpu_loop() {
    struct reg_type registers = init_mem();

    while(1) {
        unsigned char instruction = readRAM(registers.PC);
        parse_instr(instruction, &registers);
    }
}

void parse_instr(unsigned char instr, struct reg_type *reg) {
    //All called functions assume that the instruction is correct
    // 01xx xxxx
    if(get_bits(instr, 0, 1) == 1)
        return;
}

struct reg_type init_mem() {
    //Create the registers struct and initialize the values
    struct reg_type registers;
    registers.AF = 0x01B0;
    //These casts work since casting is higher precedence than addition
    registers.A = (unsigned char *)&registers.AF;
    registers.F = (unsigned char *)&registers.AF+1;
    registers.BC = 0x0013;
    registers.B = (unsigned char *)&registers.BC;
    registers.C = (unsigned char *)&registers.BC+1;
    registers.DE = 0x00D8;
    registers.D = (unsigned char *)&registers.DE;
    registers.E = (unsigned char *)&registers.DE+1;
    registers.HL = 0x014D;
    registers.H = (unsigned char *)&registers.HL;
    registers.L = (unsigned char *)&registers.HL+1;
    registers.SP = 0xFFFE;
    registers.PC = 0x100;

    //initialize the I/O registers
    writeRAM(0xFF05, 0x00);
    writeRAM(0xFF06, 0x00);
    writeRAM(0xFF07, 0x00);
    writeRAM(0xFF10, 0x80);
    writeRAM(0xFF11, 0xBF);
    writeRAM(0xFF12, 0xF3);
    writeRAM(0xFF14, 0xBF);
    writeRAM(0xFF16, 0x3F);
    writeRAM(0xFF17, 0x00);
    writeRAM(0xFF19, 0xBF);
    writeRAM(0xFF1A, 0x7F);
    writeRAM(0xFF1B, 0xFF);
    writeRAM(0xFF1C, 0x9F);
    writeRAM(0xFF1E, 0xBF);
    writeRAM(0xFF20, 0xFF);
    writeRAM(0xFF21, 0x00);
    writeRAM(0xFF22, 0x00);
    writeRAM(0xFF23, 0xBF);
    writeRAM(0xFF24, 0x77);
    writeRAM(0xFF25, 0xF3);
    writeRAM(0xFF26, 0xF1);
    writeRAM(0xFF40, 0x91);
    writeRAM(0xFF42, 0x00);
    writeRAM(0xFF43, 0x00);
    writeRAM(0xFF45, 0x00);
    writeRAM(0xFF47, 0xFC);
    writeRAM(0xFF48, 0xFF);
    writeRAM(0xFF49, 0xFF);
    writeRAM(0xFF4A, 0x00);
    writeRAM(0xFF4B, 0x00);
    writeRAM(0xFFFF, 0x00);

    return registers;
}
