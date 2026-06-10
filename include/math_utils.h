#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <math.h>

// Clamps a value between a lower and upper boundary.
#define RAY_CLAMP(value, low, high) (fmin(high, fmax(low, value)))

float distance_between_two_points(float ax, float ay, float bx, float by);

#endif