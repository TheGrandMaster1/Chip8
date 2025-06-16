#include <stdio.h>
#include <stdint.h>

typedef struct chip_8{
    uint8_t memory[4096]; //4K byte-addressable memory
    uint8_t register_V[16]; //16 8-bit general purpose registers V0 to VF
    uint16_t register_I; //16-bit index register
    uint16_t pc; //2 byte PC counter
    uint8_t gfx[64*32]; //64 x 32 graphics display... 1 byte per pixel for simplicity
    uint16_t stack[16]; //stack has max. 16 levels
    uint8_t sp; //1 byte stack pointer
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t keyboard; // 4x4 hexadecimal keyboard
} CHIP8;

const unsigned int START_ADDRESS = 0x200;

void chip8_initialize(CHIP8* chip8){
    memset(chip8, 0, sizeof(CHIP8)); // sets all the structure a\members to 0
    chip8->pc = START_ADDRESS;
    
}
