#include "memory.h"

#include "cpu_internal.h"

#include <stdlib.h>

//0000-7FFF
unsigned char *cart;
//8000-9FFF
unsigned char *vram;
//A000-BFFF
unsigned char *cartram;
//C000-DFFF
unsigned char *ram;
//E000-FDFF
unsigned char *ramecho;
//FE00-FE9F
unsigned char *OAM;
//FEA0-FEFF, unusable
//FF00-FF7F
unsigned char *IOports;
//FF80-FFFE
unsigned char *internram;
//FFFF
unsigned char interrupt;

//Must be called before any other functions are used
void alloc_ram() {
    cart = malloc(0x8000);
    vram = malloc(0x2000);
    cartram = malloc(0x2000);
    ram = malloc(0x2000);
    ramecho = ram;
    OAM = malloc(0xA0);
    IOports = malloc(0x80);
    internram = malloc(0x7E);
    interrupt = 0;
}

//TODO: implement support for different cartridge types
void load_cart(FILE *cartfile) {
    //TODO: implement cart size checking
    for(int i = 0; i < 0x8000; i++) {
        cart[i] = fgetc(cartfile);
    }
}

unsigned char readRAM(unsigned short address) {
    if(address < 0x8000)
        return cart[address];
    else if(address < 0xA000)
        return vram[address - 0x8000];
    else if(address < 0xC000)
        return cartram[address - 0xA000];
    else if(address < 0xE000)
        return ram[address - 0xC000];
    else if(address < 0xFE00)
        return ramecho[address - 0xE000];
    else if(address < 0xFEA0)
        return OAM[address - 0xFE00];
    else if(address < 0xFF80)
        return IOports[address - 0xFF00];
    else if(address < 0xFFFF)
        return internram[address - 0xFF80];
    else if(address == 0xFFFF)
        return interrupt;
    else return 0; //Definitely better error code to return here
    //TODO: Check for invalid memory accesses
}

void writeRAM(unsigned short address, unsigned char value) {
    if(address < 0x8000)
        cart[address] = value;
    else if(address < 0xA000)
        vram[address - 0x8000] = value;
    else if(address < 0xC000)
        cartram[address - 0xA000] = value;
    else if(address < 0xE000)
        ram[address - 0xC000] = value;
    else if(address < 0xFE00)
        ramecho[address - 0xE000] = value;
    else if(address < 0xFEA0)
        OAM[address - 0xFE00] = value;
    else if(address < 0xFF80)
        IOports[address - 0xFF00] = value;
    else if(address < 0xFFFF)
        internram[address - 0xFF80] = value;
    else if(address == 0xFFFF)
        interrupt = value;

    //TODO: Check for invalid memory accesses
}

//Get a pointer to the memory location specified
//Using this function is not preferred over using the read/write functions
unsigned char *getRAMpointer(unsigned short address) {
    if(address < 0x8000)
        return &cart[address];
    else if(address < 0xA000)
        return &vram[address - 0x8000];
    else if(address < 0xC000)
        return &cartram[address - 0xA000];
    else if(address < 0xE000)
        return &ram[address - 0xC000];
    else if(address < 0xFE00)
        return &ramecho[address - 0xE000];
    else if(address < 0xFEA0)
        return &OAM[address - 0xFE00];
    else if(address < 0xFF80)
        return &IOports[address - 0xFF00];
    else if(address < 0xFFFF)
        return &internram[address - 0xFF80];
    else if(address == 0xFFFF)
        return &interrupt;
    else return NULL;
    //TODO: Check for invalid memory accesses
}

//Push the byte onto the stack
void stack_push(struct reg_type *reg, unsigned char value) {
    reg->SP -= 1;
    writeRAM(reg->SP, value);
}

//Pop a byte off the stack
unsigned char stack_pop(struct reg_type *reg) {
    unsigned char val = readRAM(reg->SP);
    reg->SP += 1;
    return val;
}
