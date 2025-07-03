
//Testing the SDL library:

#include <stdio.h>
#include "src/include/SDL.h"

#undef main

const int WIDTH = 500, HEIGHT = 500;

int main( int argc, char *argv[] )
{
    SDL_Init( SDL_INIT_EVERYTHING );
    

    SDL_Window *window = SDL_CreateWindow( "Hello SDL WORLD miao", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI );

    if ( NULL == window )
    {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Event windowEvent;

    while (1)
    {
        if ( SDL_PollEvent( &windowEvent ) )
        {
            if ( SDL_QUIT == windowEvent.type )
            { break; }
        }
    }

    SDL_DestroyWindow( window );
    SDL_Quit( );

    return EXIT_SUCCESS;
}  