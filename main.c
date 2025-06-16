#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.c"

int main(){
    CHIP8 chip8;
    chip8_initialize(&chip8);

    for (int i = 0; i < 16; i++) {
        printf("V[%X] = %d\n", i, chip8.register_V[i]);
    }
    printf("PC = 0x%X\n", chip8.pc);
 
}
