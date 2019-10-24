#include "SDL_Interface.h"
#include "chip-8.h"
#include <SDL.h>

const int SCREEN_HEIGHT = 800;
const int SCREEN_WIDTH = 600;

SDL_Event current_event;
SDL_Window* window = NULL;
SDL_Surface* screen_surface = NULL;

void initializeGraphics(){
    if (SDL_Init(SDL_INIT_VIDEO) == -1)
        error(SDL_GetError());

    window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        error(SDL_GetError());
    
    screen_surface = SDL_GetWindowSurface(window);
    SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface->format, 255, 0, 0));
    SDL_UpdateWindowSurface(window);
    

}

void draw(){
    SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface->format, 255, 0, 0));
    SDL_UpdateWindowSurface(window);
}


void getEvent(){
    if (SDL_PollEvent(&current_event)){
        switch (current_event.type){
         case SDL_QUIT:
          setQuitFlag();
         }
    }
};


void killGraphics(){
    SDL_DestroyWindow(window);
    SDL_Quit();
}