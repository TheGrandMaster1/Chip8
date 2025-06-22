#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct chip_8{
    uint8_t memory[4096]; //4K byte-addressable memory
    uint8_t register_V[16]; //16 8-bit general purpose registers V0 to VF
    uint16_t register_I; //16-bit index register
    uint16_t pc; //2 byte Program Counter
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
    //Fetch:
    uint16_t opcode = (chip8->memory[chip8->pc] >> 8) | (chip8->memory[chip8->pc + 1]);
    printf("Executing opcode: 0x%X at PC: 0x%X\n", opcode, chip8->pc);


     // Placeholder example (you can expand this)
    switch (opcode & 0xF000) {
        case 0x1000:
        //1nnn - JP addr : Jump to location nnn
            chip8->pc = opcode & 0x0FFF;
            break;
        
        default:
            printf("Unknown opcode: 0x%X\n", opcode);
            chip8->pc += 2; // Skip to next instruction
            break;
    }

    if (chip8->delay_timer > 0) chip8->delay_timer--;
    if (chip8->sound_timer > 0) chip8->sound_timer--;
    //Decode:
    
    //Execute:

}

uint8_t fonts[80] = {
    //sprites of 5 bytes per character:
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void set_fonts(CHIP8* chip8){
    memcpy(&chip8->memory[0x050], fonts, sizeof(fonts));
}