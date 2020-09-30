#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <unistd.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 700

int draw(int del, int len);

int main(void) {
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);


    for (int i = 0; i < WINDOW_WIDTH; ++i){
        SDL_SetRenderDrawColor(renderer, 0, 100, i, 255);
        SDL_RenderDrawPoint(renderer, i, 54);

    }
    SDL_RenderPresent(renderer);


    while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

