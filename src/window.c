#include "window.h"

void update_window(SDL_Window *window, WindowDimensions *window_dimensions)
{
    SDL_GetWindowSize(window, &window_dimensions->width_pixels, &window_dimensions->height_pixels);
}