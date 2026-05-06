#include<SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600

typedef struct Velocity {
    int x,y;
}Velocity;

Velocity get_rand_v()
{
   int choice = rand() / (RAND_MAX / 4);
   switch(choice)
   {
    case 0:
    return (Velocity){0, -1};
    case 1:
    return (Velocity){0, 1};
    case 2:
    return (Velocity){-1, 0};
    case 3:
    return (Velocity){1, 0};
   }
   exit(-1);
}

int find_in_path(Velocity path[], int path_len, Velocity p) {
    for (int i = 0; i < path_len; i++) {
        if (path[i].x == p.x && path[i].y == p.y) {
            return i;
        }
    }

    return -1;
}

void make_maze(Velocity s, SDL_Surface *psurface) {
    Velocity rs = {410, 301};

    Velocity path[WIDTH * HEIGHT];
    int path_len = 0;

    path[path_len++] = s;

    while (s.x != rs.x || s.y != rs.y) {
        Velocity r = get_rand_v();

        s.x += r.x;
        s.y += r.y;

        if (s.x < 0) s.x = 0;
        if (s.y < 0) s.y = 0;
        if (s.x >= WIDTH) s.x = WIDTH - 1;
        if (s.y >= HEIGHT) s.y = HEIGHT - 1;

        int loop_index = find_in_path(path, path_len, s);

        if (loop_index != -1) {
            path_len = loop_index + 1;
            printf("loop erased, new path_len = %d\n", path_len);
        } else {
            path[path_len++] = s;
        }

        printf("current: %d %d | target: %d %d | path_len: %d\n",
               s.x, s.y, rs.x, rs.y, path_len);
    }

    for (int i = 0; i < path_len; i++) {
        SDL_Rect rect = {path[i].x, path[i].y, 1, 1};
        SDL_FillRect(
            psurface,
            &rect,
            SDL_MapRGB(psurface->format, 255, 255, 255)
        );
    }
}

int main() {

    SDL_Window *pwindow = SDL_CreateWindow("Random Walk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface *psurface = SDL_GetWindowSurface(pwindow);

    Velocity start_point = {WIDTH/2, HEIGHT/2};
    make_maze(start_point, psurface);

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