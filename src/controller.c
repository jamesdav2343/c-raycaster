#include "controller.h"

Vector2 get_input_direction()
{
    const bool *key_states = SDL_GetKeyboardState(NULL);

    int xDir = 0;
    int yDir = 0;

    if (key_states[SDL_SCANCODE_W])
        yDir--;
    if (key_states[SDL_SCANCODE_A])
        xDir--;
    if (key_states[SDL_SCANCODE_S])
        yDir++;
    if (key_states[SDL_SCANCODE_D])
        xDir++;

    return (Vector2){xDir, yDir};
}