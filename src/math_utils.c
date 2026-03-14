#include "math_utils.h"

// Clamps a value between a lower and upper boundary.
int clamp_in_range(int value, int lower_boundary, int upper_boundary)
{
    int max = fmax(value, lower_boundary);
    return (int)fmin(max, upper_boundary);
}

float distance_between_two_points(float ax, float ay, float bx, float by)
{
    return sqrtf((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}