#include "SDL_Interface.h"
#include "chip-8.h"
#include <SDL.h>

const int SCREEN_HEIGHT = 600;
const int SCREEN_WIDTH = 800;


SDL_Window* window = NULL;
SDL_Surface* screen_surface = NULL;
SDL_Texture* texture = NULL;
SDL_Renderer* renderer = NULL;

void initializeGraphics(){
    if (SDL_Init(SDL_INIT_VIDEO) == -1)
        error(SDL_GetError());
    
    window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        error(SDL_GetError());
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    screen_surface = SDL_GetWindowSurface(window);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 800, 600);
}

void draw(void const* buffer){
    SDL_UpdateTexture(texture, NULL, buffer, 2048);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void updateInput(unsigned char* array){

    int length;
    Uint8 *keystate = SDL_GetKeyboardState(&length);

    array[0] = keystate[SDL_SCANCODE_1];
    array[1] = keystate[SDL_SCANCODE_2];
    array[2] = keystate[SDL_SCANCODE_3];
    array[3] = keystate[SDL_SCANCODE_4];
    array[4] = keystate[SDL_SCANCODE_Q];
    array[5] = keystate[SDL_SCANCODE_W];
    array[6] = keystate[SDL_SCANCODE_E];
    array[7] = keystate[SDL_SCANCODE_R];
	array[8] = keystate[SDL_SCANCODE_A];
    array[9] = keystate[SDL_SCANCODE_S];
    array[10] = keystate[SDL_SCANCODE_D];
    array[11] = keystate[SDL_SCANCODE_F];
    array[12] = keystate[SDL_SCANCODE_Z];
    array[13] = keystate[SDL_SCANCODE_X];
    array[14] = keystate[SDL_SCANCODE_C];
    array[15] = keystate[SDL_SCANCODE_V];

}

void getEvent(int* p){
    SDL_Event current_event;
    if (SDL_PollEvent(&current_event)){
        switch (current_event.type){
         case SDL_QUIT:
         *p = 1;
        }
    }
};


void killGraphics(){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}