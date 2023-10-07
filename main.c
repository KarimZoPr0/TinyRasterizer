#include <stdio.h>
#include "stdbool.h"
#include <SDL.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

bool initialize_window( void )
{
    if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
    {
        fprintf( stderr, "Error initializing SDL \n" );
        return false;
    }

    window = SDL_CreateWindow( NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,
                               SDL_WINDOW_ALWAYS_ON_TOP );
    if( !window )
    {
        fprintf( stderr, "Error initializing SDL window. \n" );
        return false;
    }

    renderer = SDL_CreateRenderer( window, -1, 0 );

    if( !renderer )
    {
        fprintf( stderr, "Error initializing SDL renderer. \n" );
        return false;
    }

    return true;
}

int main( int argc, char *argv[] )
{
    initialize_window( );

    return 0;
}