#include "window.h"

void update_window(SDL_Window *window, WindowDimensions *windowDimensions)
{
    SDL_GetWindowSize(window, &windowDimensions->widthPixels, &windowDimensions->heightPixels);
}