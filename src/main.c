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
double stretch = 1;

double scale_x(double x) {
    return (x - X_MIN) * WIDTH / (X_MAX - X_MIN);
}

double scale_y(double y) {
    return HEIGHT - ((y - Y_MIN) * HEIGHT / (Y_MAX - Y_MIN));
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

    SDL_Window* window = SDL_CreateWindowFrom((void*)x_window);
    SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    int close = 0;
    int fps_frames = 0;
    int fps_last = SDL_GetTicks();
    int fps = 0;

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
        drawFunc(render, g, 3, 0.1);
        drawFunc(render, h, 3, 0.1);
        
        SDL_RenderPresent(render);

        if (stretch > -1) stretch -= 0.005;
        else stretch = 1;

        fps_frames++;
        int fps_curr = SDL_GetTicks();
        if (fps_curr - fps_last >= 1000) {
            fps = fps_frames;
            fps_frames = 0;
            fps_last = SDL_GetTicks();

            printf("fps: %d\n", fps);
        }

        SDL_Delay(1000 / 60);
    }
    
    XCloseDisplay(x_display);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}