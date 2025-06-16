#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.c"

int main(){
    CHIP8 chip8;
    
    /*
    //testing the chip8_initialize function:
    chip8_initialize(&chip8);
    for (int i = 0; i < 16; i++) {
        printf("V[%X] = %d\n", i, chip8.register_V[i]);
    }
    printf("PC = 0x%X\n", chip8.pc);
    return 0;
    */

   //testing the chip8_load_ROM function:
   chip8_initialize(&chip8);
   uint8_t test_rom[] = {
        0x00, 0xE0, // CLS
        0xA2, 0x2A, // LD I, 0x22A
        0x60, 0x0C, // LD V0, 0x0C
        0x61, 0x08  // LD V1, 0x08
    };

    /*
    //ROM that exceeds memory size:
    const size_t ROM_SIZE = 4096;
    uint8_t test_rom[ROM_SIZE];
    memset(test_rom, 0xAA, ROM_SIZE);
    */

    chip8_load_ROM(&chip8, test_rom, sizeof(test_rom));
    printf("Memory at 0x200:\n");
    for (int i = 0; i < sizeof(test_rom); i++) {
        printf("0x%03X: 0x%02X\n", START_ADDRESS + i, chip8.memory[START_ADDRESS + i]);
    }
    
    return 0;
 
}
