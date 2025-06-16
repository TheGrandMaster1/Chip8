#include <stdio.h>
#include <stdint.h>
#include <string.h>

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
    memset(chip8, 0, sizeof(CHIP8)); // sets all the structure members to 0
    chip8->pc = START_ADDRESS;
}

void chip8_load_ROM(CHIP8* chip8, const uint8_t* ROM_file, size_t ROM_size){
    // Inputs: - Constant 1 byte pointer to the ROM array
    //         - size of the ROM (size_t is unsigned); used in the memset function.

    //check if memory overflow:
    if (ROM_size + START_ADDRESS > sizeof(chip8->memory)){
        printf("Can't load ROM. Ran out of memory space.\n");
        return;
    }
    //no memory overflow so we add ROM to memory:
    memcpy(&chip8->memory[0x200], ROM_file, ROM_size);

}

void chip8_emulate(CHIP8* chip8){

}

