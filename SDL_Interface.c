#include "SDL_Interface.h"
#include "chip-8.h"
#include <SDL.h>

const int SCREEN_HEIGHT = 600;
const int SCREEN_WIDTH = 800;


SDL_Window* window = NULL;
SDL_Window* debugger = NULL;
SDL_Surface* screen_surface = NULL;
SDL_Texture* texture = NULL;
SDL_Renderer* renderer = NULL;

void initializeGraphics(){
    if (SDL_Init(SDL_INIT_VIDEO) == -1)
        error(SDL_GetError());
    
    window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 640, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    debugger = SDL_CreateWindow("Chip-8 Debugger", SDL_WINDOWPOS_CENTERED+1280,SDL_WINDOWPOS_CENTERED,800,600,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        error(SDL_GetError());
    if (!(renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)))
        error(SDL_GetError());
    if (!(screen_surface = SDL_GetWindowSurface(window)))
        error(SDL_GetError());
    if (!(texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 64, 32)))
        error(SDL_GetError());
}

void draw(void const* buffer, int pitch){
    if (SDL_UpdateTexture(texture, NULL, buffer, 256))
        error(SDL_GetError());
    if (SDL_RenderClear(renderer))
        error(SDL_GetError());
    if (SDL_RenderCopy(renderer, texture, NULL, NULL))
        error(SDL_GetError());
    SDL_RenderPresent(renderer);
}

void updateInput(unsigned char* array){

    int length;
    Uint8 *keystate = SDL_GetKeyboardState(&length);

    array[0] = keystate[SDL_SCANCODE_X];
    array[1] = keystate[SDL_SCANCODE_1];
    array[2] = keystate[SDL_SCANCODE_2];
    array[3] = keystate[SDL_SCANCODE_3];
    array[4] = keystate[SDL_SCANCODE_Q];
    array[5] = keystate[SDL_SCANCODE_W];
    array[6] = keystate[SDL_SCANCODE_E];
    array[7] = keystate[SDL_SCANCODE_A];
	array[8] = keystate[SDL_SCANCODE_S];
    array[9] = keystate[SDL_SCANCODE_D];
    array[10] = keystate[SDL_SCANCODE_Z];
    array[11] = keystate[SDL_SCANCODE_C];
    array[12] = keystate[SDL_SCANCODE_4];
    array[13] = keystate[SDL_SCANCODE_R];
    array[14] = keystate[SDL_SCANCODE_F];
    array[15] = keystate[SDL_SCANCODE_V];

}

void getEvent(bool* p){
    SDL_Event current_event;
    if (SDL_PollEvent(&current_event)){
        switch (current_event.type){
        case SDL_QUIT:
            *p = 1;
            break;
        case SDL_WINDOWEVENT_RESIZED:
            setDrawFlag();
            break;
        }
    }
    
};


void killGraphics(){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyWindow(debugger);
    SDL_Quit();
}