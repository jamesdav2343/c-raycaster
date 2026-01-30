#ifndef FRAME_RATE_H
#define FRAME_RATE_H

#include <SDL3/SDL.h>

#define FPS 60
#define ONE_SECOND_MS 1000

extern const Uint64 MS_PER_FRAME_60;

typedef struct FrameData
{
    Uint64 frame_start_time;
    Uint64 delta;
    Uint64 last_fps_update_time;
    Uint64 ms_per_frame;
    int fps;
    int frame_counter;
} FrameData;

void update_frame_data(FrameData *frame_data);

void limit_frame_rate(Uint64 delta);

#endif