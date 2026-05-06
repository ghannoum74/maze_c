#include<SDL2/SDL.h>

#define WIDTH 800
#define HEIGHT 600

int main() {

    SDL_Window *pwindow = SDL_CreateWindow("Random Walk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface *psurface = SDL_GetWindowSurface(pwindow);

     int app_running = 1;
    while (app_running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                app_running = 0;
            }
        }

        SDL_UpdateWindowSurface(pwindow);
        SDL_Delay(1);
    }
}