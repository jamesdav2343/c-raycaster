#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>

typedef struct WindowDimensions
{
    int widthPixels;
    int heightPixels;
} WindowDimensions;

void update_window(SDL_Window *window, WindowDimensions *windowDimensions);

#endif