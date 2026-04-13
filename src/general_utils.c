#include "general_utils.h"

Uint32 color_lerp(Uint32 color1, Uint32 color2, double t)
{
    if (t < 0)
        return color1;
    if (t >= 1)
        return color2;

    Uint32 r1 = (color1 >> 16) & 0xFF;
    Uint32 r2 = (color2 >> 16) & 0xFF;

    Uint32 g1 = (color1 >> 8) & 0xFF;
    Uint32 g2 = (color2 >> 8) & 0xFF;

    Uint32 b1 = (color1) & 0xFF;
    Uint32 b2 = (color2) & 0xFF;

    r1 = (1 - t) * r1 + t * r2;
    if (r1 > 0xFF)
        r1 = 0xFF;

    g1 = (1 - t) * g1 + t * g2;
    if (g1 > 0xFF)
        g1 = 0xFF;

    b1 = (1 - t) * b1 + t * b2;
    if (b1 > 0xFF)
        b1 = 0xFF;

    return (r1 << 16) + (g1 << 8) + (b1);
}