#include "lighting.h"
#include <glib.h>
#include <math.h>

#define NUM_DIRECTIONS 4
#define NUM_LIGHTS 3
#define INITIAL_INTENSITY 15

#define INTENSITY_DIFF 2
#define DECAY 1

float light_map[ROWS * COLS] = { 0 };

// void test(gpointer data, gpointer user_data) { printf("%zu\n", data); }

void bake_light_map()
{
    size_t* light_sources = calloc(NUM_LIGHTS, sizeof(size_t));

    light_sources[0] = 3 + (2 * COLS); // x: 3, y: 2
    // light_sources[1] = 6 + (2 * COLS); // x: 6, y: 2
    light_sources[1] = 46;
    light_sources[2] = 406;

    // Nodes in graph should be the indices of the light sources
    GQueue* light_queue = g_queue_new();
    Uint8 light_intensities[ROWS * COLS] = { 0 };

    for (int i = 0; i < NUM_LIGHTS; i++) {
        g_queue_push_tail(light_queue, (gpointer)light_sources[i]);
        light_intensities[light_sources[i]] = INITIAL_INTENSITY;
    }

    /**
     * Light propogation using BFS
     */
    while (!g_queue_is_empty(light_queue)) {
        size_t node = GPOINTER_TO_SIZE(g_queue_pop_head(light_queue));
        Uint8 light_int = light_intensities[node];

        size_t x = node % COLS;
        size_t y = node / COLS;

        // Is in bounds of the array
        if (x > 0) {
            size_t left_neighbour = x - 1 + (y * COLS);

            // If there is no wall, the light_intensity has not reached 0,
            // and the light intensity of the neighbour is at least 2 decay values
            // the light intensity of the current node
            if (!world_map[left_neighbour] && light_int > 0
                && light_intensities[left_neighbour] <= light_int - INTENSITY_DIFF) {
                light_intensities[left_neighbour] = light_int - DECAY;
                g_queue_push_tail(light_queue, (gpointer)left_neighbour);
            }
        }

        if (x < COLS - 1) {
            size_t right_neighbour = x + 1 + (y * COLS);

            if (!world_map[right_neighbour] && light_int > 0
                && light_intensities[right_neighbour] <= light_int - INTENSITY_DIFF) {
                light_intensities[right_neighbour] = light_int - DECAY;
                g_queue_push_tail(light_queue, (gpointer)right_neighbour);
            }
        }

        if (y > 0) {
            size_t bottom_neighbour = x + ((y + 1) * COLS);

            if (!world_map[bottom_neighbour] && light_int > 0
                && light_intensities[bottom_neighbour] <= light_int - INTENSITY_DIFF) {
                light_intensities[bottom_neighbour] = light_int - DECAY;
                g_queue_push_tail(light_queue, (gpointer)bottom_neighbour);
            }
        }

        if (y < ROWS - 1) {
            size_t top_neighbour = x + ((y - 1) * COLS);

            if (!world_map[top_neighbour] && light_int > 0
                && light_intensities[top_neighbour] <= light_int - INTENSITY_DIFF) {
                light_intensities[top_neighbour] = light_int - DECAY;
                g_queue_push_tail(light_queue, (gpointer)top_neighbour);
            }
        }
    }

    // printf("map:\n");
    // pretty_print_grid(world_map, COLS, 2);
    // printf("light intensities:\n");
    // pretty_print_grid(light_intensities, COLS, 2);
    // printf("\n\n");

    for (int i = 0; i < ROWS * COLS; i++) {
        Uint8 intensity = light_intensities[i];
        light_map[i] = powf(0.8f, INITIAL_INTENSITY - intensity);
        // printf("%f, ", light_map[i]);
    }

    free(light_sources);
}

float get_wall_light_intensity(int x, int y, Vector2 ray_direction, enum Orientation side_orientation)
{
    // if h and coming from south (negative y direction), then access cell below (y + 1)
    if (side_orientation == HORIZONTAL) {
        if (ray_direction.y < 0)
            return light_map[x + ((y + 1) * COLS)];
        else
            return light_map[x + ((y - 1) * COLS)];
    } else {
        if (ray_direction.x < 0)
            return light_map[x + 1 + (y * COLS)];
        else
            return light_map[x - 1 + (y * COLS)];
    }

    // if vertical, and x is
    return 0;
}