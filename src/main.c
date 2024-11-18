#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <X11/Xlib.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

const int WIDTH = 1920;
const int HEIGHT = 1080;
const double X_MIN = -8;
const double X_MAX = 8;
const double Y_MIN = -3;
const double Y_MAX = 3;
const double stretch_step = 0.005;
const double step = 0.1;
double stretch = 1;
int stretch_col = 0;
double cached_g[161][400];
double cached_h[161][400];

double scale_x(double x) {
    return (x - X_MIN) * WIDTH / (X_MAX - X_MIN);
}

double scale_y(double y) {
    return HEIGHT - ((y - Y_MIN) * HEIGHT / (Y_MAX - Y_MIN));
}

void cacheFunc(double cached_f[161][400], double (*f)(double)) {
    int i = 0;
    for (double x = X_MIN; x < X_MAX; x += step) {
        int j = 0;
        for (double s = 1; s > -1; s -= stretch_step) {
            cached_f[i][j] = (*f)(x * s);
            j++;
        }
        i++;
    }
}

void drawCachedFunc(SDL_Renderer* render, double cached_f[161][400], int thickness, double step) {
    for (int step_row = 0; step_row < 160; step_row++) {
        double x = step_row * step - 8;
        double y = cached_f[step_row][stretch_col];
        double x_next = x + step;
        double y_next = cached_f[step_row + 1][stretch_col];

        int screen_x1 = (int)scale_x(x);
        int screen_y1 = (int)scale_y(y);
        int screen_x2 = (int)scale_x(x_next);
        int screen_y2 = (int)scale_y(y_next);

        for (int i = -thickness / 2; i <= thickness / 2; i++) {
            SDL_RenderDrawLine(render, screen_x1 + i, screen_y1, screen_x2 + i, screen_y2);
            SDL_RenderDrawLine(render, screen_x1, screen_y1 + i, screen_x2, screen_y2 + i);
        }
    }
}

void drawFunc(SDL_Renderer* render, double (*f)(double), int thickness, double step) {
    for (double x = X_MIN; x < X_MAX; x += step) {
        double y = (*f)(x);
        double x_next = x + step;
        double y_next = (*f)(x_next);

        int screen_x1 = (int)scale_x(x);
        int screen_y1 = (int)scale_y(y);
        int screen_x2 = (int)scale_x(x_next);
        int screen_y2 = (int)scale_y(y_next);

        for (int i = -thickness / 2; i <= thickness / 2; i++) {
            SDL_RenderDrawLine(render, screen_x1 + i, screen_y1, screen_x2 + i, screen_y2);
            SDL_RenderDrawLine(render, screen_x1, screen_y1 + i, screen_x2, screen_y2 + i);
        }
    }
}

double g(double x) {return tanh(x * stretch);}
double h(double x) {return -tanh(x * stretch);}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    Display *x_display = XOpenDisplay(NULL);
    if (!x_display) return -1;

    Window x_window = RootWindow(x_display, DefaultScreen(x_display));
    if (x_window == None) {
        XCloseDisplay(x_display);
        return -1;
    }

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window;
    #if DEBUG
    window = SDL_CreateWindow("tanhell", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    #else
    window = SDL_CreateWindowFrom((void*)x_window);
    #endif

    SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    int close = 0;
    #if DEBUG
    int fps_frames = 0;
    int fps_last = SDL_GetTicks();
    int fps = 0;
    #endif

    cacheFunc(cached_g, g);
    cacheFunc(cached_h, h);

    while (!close) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    close = 1;
                    break;
            }
        }

        SDL_SetRenderDrawColor(render, 24, 24, 24, 255);
        SDL_RenderClear(render);
    
        SDL_SetRenderDrawColor(render, 173, 0, 118, 255);
        // drawFunc(render, g, 3, step);
        // drawFunc(render, h, 3, step);
        drawCachedFunc(render, cached_g, 3, step);
        drawCachedFunc(render, cached_h, 3, step);
        
        SDL_RenderPresent(render);

        if (stretch > -1 + stretch_step) stretch -= stretch_step;
        else stretch = 1;
        stretch_col = round(fabs(stretch - 1) / stretch_step);

        #if DEBUG
        fps_frames++;
        int fps_curr = SDL_GetTicks();
        if (fps_curr - fps_last >= 1000) {
            fps = fps_frames;
            fps_frames = 0;
            fps_last = SDL_GetTicks();

            printf("fps: %d\n", fps);
        }
        #endif

        SDL_Delay(1000 / 60);
    }
    
    XCloseDisplay(x_display);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}