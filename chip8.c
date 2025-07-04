#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

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
    //instructions are 2 bytes
    uint16_t opcode = (chip8->memory[chip8->pc] << 8) | (chip8->memory[chip8->pc + 1]);
    printf("Executing opcode: 0x%X at PC: 0x%X\n", opcode, chip8->pc);

    //Decode & Execute:
    switch (opcode & 0xF000) {
        case 0x0000: {
            switch (opcode & 0x00FF) {
                case 0x00E0:
                    // 00E0 - CLS: Clear the display
                    memset(chip8->gfx, 0, sizeof(chip8->gfx));
                    break;
                case 0x00EE:
                    // 00EE - RET: Return from a subroutine
                    chip8->sp--;
                    chip8->pc = chip8->stack[chip8->sp]; //resume from the address that was saved during a previous CALL (2NNN)
                    break;

                default:
                    // 0nnn - SYS addr: Ignored (original CHIP-8 systems)
                    printf("Ignored or unknown 0-series opcode: 0x%04X\n", opcode);
                    break;
            }
            break;
        }
        case 0x1000:;
            //1nnn - JP addr : Jump to location nnn
            chip8->pc = opcode & 0x0FFF;
            break;
        case 0x2000:;
            //2nnn - CALL addr : Call subroutine at nnn
            uint16_t address = opcode & 0x0FFF;
            chip8->stack[chip8->sp] = chip8->pc + 2; //Save the return address (next instruction) on the stack
            chip8->sp +=1;
            chip8->pc = address;
            break;
        case 0x3000:;
            //3xkk - SE Vx, byte: Skip next instruction if Vx = kk
            uint8_t compare_val3 = opcode & 0x00FF;
            uint8_t reg3 = (opcode & 0x0F00) >> 8;
            
            if (chip8->register_V[reg3] == compare_val3) {chip8->pc += 2;}
            break;
        case 0x4000:;
            //4xkk - SNE Vx, byte: Skip next instruction if Vx != kk.
            uint8_t compare_val4 = opcode & 0x00FF;
            uint8_t reg4 = (opcode & 0x0F00) >> 8;
            if (chip8->register_V[reg4] != compare_val4) {chip8->pc += 2;}
            break;
        case 0x5000:;
            //5xy0 SE Vx, Vy: Skip next instruction if Vx = Vy
            if ((opcode & 0x000F) == 0x0000) {  // Confirm it's 5xy0
                uint8_t regx5 = (opcode & 0x0F00) >> 8;
                uint8_t regy5 = (opcode & 0x00F0) >> 4; ///??????
                if (chip8->register_V[regx5] == chip8->register_V[regy5]){chip8->pc +=2;}

            } else{printf("Unknown 5-series opcode: 0x%04X\n", opcode);}
            break;
        case 0x6000:;
            //6xkk - LD Vx, byte : Set Vx = kk. load kk into reg Vx
            uint8_t regx6 = (opcode & 0x0F00) >> 8;
            uint8_t val6 = (opcode & 0x00FF);

            chip8->register_V[regx6] = val6;
            break;
        case 0x7000:;
            //7xkk - ADD Vx, byte : Set Vx = Vx + kk
            uint8_t regx7 = (opcode & 0x0F00)>>8;
            uint8_t val7 = (opcode & 0x00FF);

            chip8->register_V[regx7] = chip8->register_V[regx7] + val7;
            break;
        case 0x8000:;
            //8xy0 - LD Vx, Vy : Set Vx = Vy.
            if ((opcode & 0x000F) == 0x0000){
                uint8_t regx8 = (opcode & 0x0F00) >> 8;
                uint8_t regy8 = (opcode & 0x00F0) >> 4; ///??????

                chip8->register_V[regx8] = chip8->register_V[regy8];
            } else if ((opcode & 0x000F) == 0x0001){
                //8xy1 - OR Vx, Vy: Set Vx = Vx OR Vy.
                uint8_t regx81 = (opcode & 0x0F00) >> 8;
                uint8_t regy81 = (opcode & 0x00F0) >> 4; //??????

                chip8->register_V[regx81] = chip8->register_V[regx81] | chip8->register_V[regy81];

            }else if ((opcode & 0x000F) == 0x0002){
                //8xy2 - AND Vx, Vy: Set Vx = Vx AND Vy.
                uint8_t regx82 = (opcode & 0x0F00) >> 8;
                uint8_t regy82 = (opcode & 0x00F0) >> 4; ///????????

                chip8->register_V[regx82] = chip8->register_V[regx82] & chip8->register_V[regy82];

            }else if ((opcode & 0x000F) == 0x0003){
                //8xy3 - XOR Vx, Vy: Set Vx = Vx XOR Vy.
                uint8_t regx83 = (opcode & 0x0F00) >> 8;
                uint8_t regy83 = (opcode & 0x00F0) >> 4; ///????????

                chip8->register_V[regx83] = chip8->register_V[regx83] ^ chip8->register_V[regy83];

            }else if ((opcode & 0x000F) == 0x0004){
                //8xy4 - ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry.
                uint8_t regx84 = (opcode & 0x0F00) >> 8;
                uint8_t regy84 = (opcode & 0x00F0) >> 4; ///????????

                uint16_t sum4 = chip8->register_V[regx84] + chip8->register_V[regy84];

                chip8->register_V[0x0F] = (sum4 > 0xFF);  // More than 8-bit -> VF = 1

                chip8->register_V[regx84] = sum4 & 0xFF; 

            }else if ((opcode & 0x000F) == 0x0005){
                //8xy5 - SUB Vx, Vy: Set Vx = Vx - Vy, set VF = NOT borrow
                uint8_t regx85 = (opcode & 0x0F00) >> 8;
                uint8_t regy85 = (opcode & 0x00F0) >> 4;

                if ( chip8->register_V[regx85] > chip8->register_V[regy85]){ // Vx > Vy
                    chip8->register_V[0x0F] = 1;
                }else{
                    chip8->register_V[0x0F] = 0;
                }
                chip8->register_V[regx85] = chip8->register_V[regx85] - chip8->register_V[regy85];

            }else if ((opcode & 0x000F) == 0x0006){
                //8xy6 - SHR Vx {, Vy}: Set Vx = Vx SHR 1.
                uint8_t regx86 = (opcode & 0x0F00) >> 8;
                if ((chip8->register_V[regx86] & 0x0F) == 0x01){
                    chip8->register_V[0x0F] = 1;
                } else{
                    chip8->register_V[0x0F] = 0;
                }
                chip8->register_V[regx86] = chip8->register_V[regx86] / 2;

            }else if ((opcode & 0x000F )== 0x0007){
                //8xy7 - SUBN Vx, Vy: Set Vx = Vy - Vx, set VF = NOT borrow.
                uint8_t regx87 = (opcode & 0x0F00) >> 8;
                uint8_t regy87 = (opcode & 0x00F0) >> 4;

                if ( chip8->register_V[regy87] > chip8->register_V[regx87]){ // Vx > Vy
                    chip8->register_V[0x0F] = 1;
                }else{
                    chip8->register_V[0x0F] = 0;
                }
                chip8->register_V[regx87] = chip8->register_V[regy87] - chip8->register_V[regx87];

            }else if ((opcode & 0x000F) == 0x000E){
                //8xyE - SHL Vx {, Vy}: Set Vx = Vx SHL 1.
                uint8_t regx88 = (opcode & 0x0F00) >> 8;
                if ((chip8->register_V[regx88] & 0x80) == 0x80){
                    chip8->register_V[0x0F] = 1;
                } else{
                    chip8->register_V[0x0F] = 0;
                }
                chip8->register_V[regx88] = chip8->register_V[regx88] * 2;

            }else {printf("Unknown 8-series opcode: 0x%04X\n", opcode);}
            break;
        case 0x9000:;
            //9xy0 - SNE Vx, Vy: Skip next instruction if Vx != Vy.
            if ((opcode & 0x000F) == 0x0000) {  // Confirm it's 9xy0
                uint8_t regx9 = (opcode & 0x0F00) >> 8;
                uint8_t regy9 = (opcode & 0x00F0) >> 4;
            
                if (chip8->register_V[regx9] != chip8->register_V[regy9]){
                    chip8->pc +=2;
                }
            }
            break;
        case 0xA000:;
            //Annn - LD I, addr: Set I = nnn.
            uint16_t vala = (opcode & 0x0FFF);
            chip8->register_I = vala;
            break;
        case 0xB000:;
            //Bnnn - JP V0, addr: Jump to location nnn + V0.
            uint16_t valb = (opcode & 0x0FFF);
            chip8->pc = chip8->register_V[0] + valb;
            break;
        case 0xC000:;
            //Cxkk - RND Vx, byte: Set Vx = random byte AND kk.
            uint8_t regc = (opcode &0x0F00) >> 8;
            uint8_t valc = (opcode &0x00FF);
            uint8_t rand_byte = rand()% 256;

            chip8->register_V[regc] = rand_byte & valc;
            break;

        case 0xD000:;
            //Dxyn - DRW Vx, Vy, nibble:
            //Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision flag.
            uint8_t rows_byte = (opcode & 0x000F); // how many bytes to draw (rows)
            uint8_t xPos = chip8->register_V[(opcode & 0x0F00) >> 8]; //xPos is value inside Vx
            uint8_t yPos =  chip8->register_V[(opcode & 0x00F0) >> 4]; //yPos is value inside Vy

            chip8->register_V[0x0F] = 0; //VF=0 initially

            //loop over each row (byte) of the sprite:
            for (uint8_t row = 0; row < rows_byte; row++){
                uint8_t sprite_byte = chip8->memory[chip8->register_I + row]; // Fetch the sprite byte from memory starting at address I
                //Loop over all 8 bits of that byte — each is a horizontal pixel:
                for (int col=0; col<8; col++){
                    uint8_t sprite_pixel = (sprite_byte >> (7-col)) & 0x1;

                    //Compute the actual (x, y) screen coordinates of the pixel
                    //by shifting from the starter coords (xPos, yPos):
                    int gfx_x = (xPos + col) % 64; //width
                    int gfx_y = (yPos + row) % 32; //height
                    
                    int gfx_index = gfx_y * 64 + gfx_x; //Converts (x, y) coordinates to a 1D index in gfx[]
                    // index = y * width + x

                    if (sprite_pixel){
                        //i.e., If we want to draw on the pixel
                        if (chip8->gfx[gfx_index] == 1){
                            //i.e., If Pixel at that location is already drawn on
                            chip8->register_V[0xF] = 1; //collision detected
                        }
                        //free pixel:
                        chip8->gfx[gfx_index] ^= 1; // XOR the pixel
                    }
                }
            }
            break;

            
            case 0xE000:;
                switch (opcode & 0x00FF){
                    case 0x009E:; 
                        // //Ex9E - SKP Vx: Skip next instruction if key with the value of Vx is pressed.
                        uint8_t VxE = (opcode & 0x0F00) >> 8;
                        if (chip8->keyboard ==chip8->memory[chip8->register_V[VxE]]){
                            chip8->pc +=2;
                        }
                        break;
                    case 0x00A1:;
                        


                }
               
    

                










                
                
            

            
        default:
            printf("Unknown opcode: 0x%X\n", opcode);
            chip8->pc += 2; // Skip to next instruction
            break;
    }
    // INCREMENT PC (for instructions that didn’t jump or return)
    chip8->pc += 2;

    if (chip8->delay_timer > 0) chip8->delay_timer--;
    if (chip8->sound_timer > 0) chip8->sound_timer--;
    


}


////////////////////////////////////////  fonts  //////////////////////////////////////////////////
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