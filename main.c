#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "memory.h"

int main(int argc, char **argv)
{
    alloc_ram();

    FILE *cart = fopen("Tetris (World).gb", "r");
    if(cart == NULL)
        printf("Error opening cart\n");
    else
        load_cart(cart);

    if(power_up() != 0)
        printf("Error loading cart\n");

    cpu_loop();

    printf("Hello world!\n");
    return 0;
}
