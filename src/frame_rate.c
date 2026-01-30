#include "frame_rate.h"

const Uint64 MS_PER_FRAME_60 = ONE_SECOND_MS / FPS;

void update_frame_data(FrameData *frame_data)
{
    frame_data->frame_counter++;
    frame_data->delta = SDL_GetTicks() - frame_data->frame_start_time;

    if (frame_data->frame_start_time > frame_data->last_fps_update_time + ONE_SECOND_MS)
    {
        frame_data->last_fps_update_time = frame_data->frame_start_time;
        frame_data->fps = frame_data->frame_counter;
        frame_data->frame_counter = 0;
    }
}

void limit_frame_rate(Uint64 delta)
{
    if (MS_PER_FRAME_60 > delta)
    {
        SDL_Delay(MS_PER_FRAME_60 - delta);
    }
}