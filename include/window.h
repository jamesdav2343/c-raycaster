#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>

typedef struct WindowDimensions
{
    int width_pixels;
    int height_pixels;
} WindowDimensions;

void update_window(SDL_Window *window, WindowDimensions *window_dimensions);

#endif