#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.c"
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include "src/include/SDL.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCALE 10  // each CHIP-8 pixel becomes 10x10 real pixels
#define WINDOW_WIDTH (SCREEN_WIDTH * SCALE)
#define WINDOW_HEIGHT (SCREEN_HEIGHT * SCALE)

#undef main

void draw_display(CHIP8* chip8, SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White pixels

        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                if (chip8->gfx[y * SCREEN_WIDTH + x]) {
                    SDL_Rect pixel_rect = {
                        x * SCALE,
                        y * SCALE,
                        SCALE,
                        SCALE
                    };
                    SDL_RenderFillRect(renderer, &pixel_rect);
                }
            }
        }

        SDL_RenderPresent(renderer);
}



int main(int argc, char *argv[]){
    CHIP8 chip8;
    srand(time(NULL));
    
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
    /*
    printf("Memory at 0x200:\n");
    for (int i = 0; i < sizeof(test_rom); i++) {
        printf("0x%03X: 0x%02X\n", START_ADDRESS + i, chip8.memory[START_ADDRESS + i]);
    }
    */
    

    //----------------------------------------------------------------------------------------------------------

    // Example sprite: the hex digit 'F' from the font set (5 bytes)
    uint8_t sprite_f[5] = {
        0xF0, // 11110000
        0x80, // 10000000
        0xF0, // 11110000
        0x80, // 10000000
        0x80  // 10000000
    };

    // Initialize chip8
    chip8_initialize(&chip8);

    // Load the sprite into memory starting at I = 0x300 (arbitrary location)
    chip8.register_I = 0x300;
    memcpy(&chip8.memory[chip8.register_I], sprite_f, sizeof(sprite_f));

    // Set V0 = 10 (x-coordinate), V1 = 5 (y-coordinate)
    chip8.register_V[0] = 10;
    chip8.register_V[1] = 5;

    // Create the Dxyn opcode for drawing 5 rows at (V0, V1):
    // D x y n
    // x=0, y=1, n=5
    uint16_t draw_opcode = 0xD000 | (0 << 8) | (1 << 4) | 5;

    // Manually set PC to start of your test ROM or just run this opcode:
    chip8.pc = START_ADDRESS;

    // Put the draw opcode in memory at PC
    chip8.memory[chip8.pc] = (draw_opcode >> 8) & 0xFF;
    chip8.memory[chip8.pc + 1] = draw_opcode & 0xFF;

    // Clear the gfx buffer before drawing
    memset(chip8.gfx, 0, sizeof(chip8.gfx));


    // --- SDL Setup ---
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("CHIP-8 Emulator - Dxyn Test",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
        }

        chip8_emulate(&chip8);  // This will execute your Dxyn opcode once and increment PC past it
        draw_display(&chip8, renderer);

        SDL_Delay(1000 / 60); // 60Hz frame rate

        // Since we only want to run the draw opcode once for testing,
        // break after the first frame to avoid redrawing endlessly.
        // (Optional: comment this out if you want continuous loop)
        
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
 
}
