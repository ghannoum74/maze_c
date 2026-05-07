#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600
#define CELL 10

#define COLS (WIDTH / CELL)
#define ROWS (HEIGHT / CELL)

typedef struct {
    int visited;
    int up, down, left, right;
} Cell;

typedef struct {
    int x, y;
} Point;

Cell maze[ROWS][COLS];

int solver_visited[ROWS][COLS];
Point solution[ROWS * COLS];
int solution_len = 0;
int running = 1;

int in_bounds(int x, int y) {
    return x >= 0 && x < COLS && y >= 0 && y < ROWS;
}

Point random_cell(void) {
    return (Point){rand() % COLS, rand() % ROWS};
}

Point random_neighbor(Point p) {
    Point options[4];
    int count = 0;

    if (in_bounds(p.x, p.y - 1)) options[count++] = (Point){p.x, p.y - 1};
    if (in_bounds(p.x, p.y + 1)) options[count++] = (Point){p.x, p.y + 1};
    if (in_bounds(p.x - 1, p.y)) options[count++] = (Point){p.x - 1, p.y};
    if (in_bounds(p.x + 1, p.y)) options[count++] = (Point){p.x + 1, p.y};

    return options[rand() % count];
}

void connect_cells(Point a, Point b) {
    if (b.x == a.x + 1) {
        maze[a.y][a.x].right = 1;
        maze[b.y][b.x].left = 1;
    } else if (b.x == a.x - 1) {
        maze[a.y][a.x].left = 1;
        maze[b.y][b.x].right = 1;
    } else if (b.y == a.y + 1) {
        maze[a.y][a.x].down = 1;
        maze[b.y][b.x].up = 1;
    } else if (b.y == a.y - 1) {
        maze[a.y][a.x].up = 1;
        maze[b.y][b.x].down = 1;
    }
}

int all_visited(void) {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            if (!maze[y][x].visited) return 0;
        }
    }

    return 1;
}

Point get_unvisited_cell(void) {
    Point p;

    do {
        p = random_cell();
    } while (maze[p.y][p.x].visited);

    return p;
}

int same_point(Point a, Point b) {
    return a.x == b.x && a.y == b.y;
}

int find_in_path(Point path[], int path_len, Point p) {
    for (int i = 0; i < path_len; i++) {
        if (same_point(path[i], p)) return i;
    }

    return -1;
}

void generate_wilson_maze(void) {
    Point first = random_cell();
    maze[first.y][first.x].visited = 1;

    while (!all_visited()) {
        Point path[ROWS * COLS];
        int path_len = 0;

        Point current = get_unvisited_cell();
        path[path_len++] = current;

        while (!maze[current.y][current.x].visited) {
            Point next = random_neighbor(current);

            int loop_index = find_in_path(path, path_len, next);

            if (loop_index != -1) {
                path_len = loop_index + 1;
            } else {
                path[path_len++] = next;
            }

            current = next;
        }

        for (int i = 0; i < path_len - 1; i++) {
            connect_cells(path[i], path[i + 1]);
            maze[path[i].y][path[i].x].visited = 1;
        }

        Point last = path[path_len - 1];
        maze[last.y][last.x].visited = 1;
    }
}

void handle_events(void) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = 0;
        }
    }
}

void draw_solution(SDL_Surface *surface) {
    Uint32 green = SDL_MapRGB(surface->format, 0, 200, 80);

    for (int i = 0; i < solution_len - 1; i++) {
        Point a = solution[i];
        Point b = solution[i + 1];

        int x1 = a.x * CELL + CELL / 2;
        int y1 = a.y * CELL + CELL / 2;

        int x2 = b.x * CELL + CELL / 2;
        int y2 = b.y * CELL + CELL / 2;

        if (x1 == x2) {
            int start = y1 < y2 ? y1 : y2;
            int end = y1 > y2 ? y1 : y2;

            SDL_Rect line = {x1 - 1, start, 3, end - start};
            SDL_FillRect(surface, &line, green);
        } else {
            int start = x1 < x2 ? x1 : x2;
            int end = x1 > x2 ? x1 : x2;

            SDL_Rect line = {start, y1 - 1, end - start, 3};
            SDL_FillRect(surface, &line, green);
        }
    }
}

void draw_maze(SDL_Surface *surface) {
    Uint32 black = SDL_MapRGB(surface->format, 0, 0, 0);
    Uint32 white = SDL_MapRGB(surface->format, 255, 255, 255);

    SDL_FillRect(surface, NULL, white);

    draw_solution(surface);

    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            int px = x * CELL;
            int py = y * CELL;

            if (!maze[y][x].up) {
                SDL_Rect wall = {px, py, CELL, 1};
                SDL_FillRect(surface, &wall, black);
            }

            if (!maze[y][x].down) {
                SDL_Rect wall = {px, py + CELL, CELL, 1};
                SDL_FillRect(surface, &wall, black);
            }

            if (!maze[y][x].left) {
                SDL_Rect wall = {px, py, 1, CELL};
                SDL_FillRect(surface, &wall, black);
            }

            if (!maze[y][x].right) {
                SDL_Rect wall = {px + CELL, py, 1, CELL};
                SDL_FillRect(surface, &wall, black);
            }
        }
    }
}

int solve_maze_visual(SDL_Window *window, SDL_Surface *surface, Point current, Point end) {
    handle_events();

    if (!running) return 0;

    if (!in_bounds(current.x, current.y)) return 0;
    if (solver_visited[current.y][current.x]) return 0;

    solver_visited[current.y][current.x] = 1;
    solution[solution_len++] = current;

    draw_maze(surface);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(1);

    if (same_point(current, end)) {
        return 1;
    }

    Cell c = maze[current.y][current.x];

    if (c.up && solve_maze_visual(window, surface, (Point){current.x, current.y - 1}, end)) return 1;
    if (c.down && solve_maze_visual(window, surface, (Point){current.x, current.y + 1}, end)) return 1;
    if (c.left && solve_maze_visual(window, surface, (Point){current.x - 1, current.y}, end)) return 1;
    if (c.right && solve_maze_visual(window, surface, (Point){current.x + 1, current.y}, end)) return 1;

    solution_len--;

    draw_maze(surface);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(15);

    return 0;
}

void reset_maze(void) {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            maze[y][x] = (Cell){0, 0, 0, 0, 0};
            solver_visited[y][x] = 0;
        }
    }

    solution_len = 0;
}

void reset_solver(void) {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            solver_visited[y][x] = 0;
        }
    }

    solution_len = 0;
}

int main(void) {
    srand((unsigned int)time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Wilson Maze Solver",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH,
        HEIGHT,
        0
    );

    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface *surface = SDL_GetWindowSurface(window);

    Point start = {0, 0};
    Point end = {COLS - 1, ROWS - 1};

    generate_wilson_maze();

    draw_maze(surface);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(50);

    solve_maze_visual(window, surface, start, end);

    draw_maze(surface);
    SDL_UpdateWindowSurface(window);

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                reset_maze();
                generate_wilson_maze();
                reset_solver();

                draw_maze(surface);
                SDL_UpdateWindowSurface(window);
                SDL_Delay(50);

                solve_maze_visual(window, surface, start, end);

                draw_maze(surface);
                SDL_UpdateWindowSurface(window);
            }
        }

        SDL_Delay(16);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}