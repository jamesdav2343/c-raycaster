#include "lighting.h"
#include "debug.h"
#include <glib.h>

#define DECAY 0.3f
#define NUM_DIRECTIONS 4
#define NUM_LIGHTS 3
#define INITIAL_INTENSITY 15

// redundant
Uint8 light_map[ROWS * COLS] = { 0 };

void test(gpointer data, gpointer user_data) { printf("%zu\n", data); }

void bake_light_map()
{
    light_sources = calloc(NUM_LIGHTS, sizeof(size_t));

    light_sources[0] = 27;
    light_sources[1] = 44;
    light_sources[2] = 406;

    // Nodes in graph should be the indices of the light sources
    GQueue* light_queue = g_queue_new();
    Uint8 light_intensities[ROWS * COLS] = { 0 };

    for (int i = 0; i < NUM_LIGHTS; i++) {
        g_queue_push_tail(light_queue, light_sources[i]);
        light_intensities[light_sources[i]] = INITIAL_INTENSITY;
    }

    // Start of BFS for light propogation
    while (!g_queue_is_empty(light_queue)) {
        // printf("light intensity:\n");
        // pretty_print_grid(light_intensities, COLS, 2);

        size_t node = GPOINTER_TO_SIZE(g_queue_pop_head(light_queue));
        Uint8 light_int = light_intensities[node];

        size_t x = node % COLS;
        size_t y = node / COLS;

        // printf("node: %zu\n", node);
        // printf("light_int: %d\n", light_int);
        // printf("x: %zu, y: %zu\n", x, y);

        // Is in bounds of the array's width
        if (x > 0) {
            // Node to the left
            size_t left_neighbour = x - 1 + (y * COLS);

            // If there is no wall, the light_intensity has not reached 0,
            // and the light intensity of the neighbour is at least 2 decay values
            // the light intensity of the current node
            if (!world_map[left_neighbour] && light_int > 0 && light_intensities[left_neighbour] <= light_int - 2) {
                light_intensities[left_neighbour] = light_int - 1;
                // Add to back of queue to be visited later
                g_queue_push_tail(light_queue, left_neighbour);
            }
        }

        if (x < COLS - 1) {
            // Node to the right
            size_t right_neighbour = x + 1 + (y * COLS);

            if (!world_map[right_neighbour] && light_int > 0 && light_intensities[right_neighbour] <= light_int - 2) {
                light_intensities[right_neighbour] = light_int - 1;
                g_queue_push_tail(light_queue, right_neighbour);
            }
        }

        if (y > 0) {
            // Node to the bottom
            size_t bottom_neighbour = x + ((y + 1) * COLS);

            if (!world_map[bottom_neighbour] && light_int > 0 && light_intensities[bottom_neighbour] <= light_int - 2) {
                light_intensities[bottom_neighbour] = light_int - 1;
                g_queue_push_tail(light_queue, bottom_neighbour);
            }
        }

        if (y < ROWS - 1) {
            // Node to the top
            size_t top_neighbour = x + ((y - 1) * COLS);

            if (!world_map[top_neighbour] && light_int > 0 && light_intensities[top_neighbour] <= light_int - 2) {
                light_intensities[top_neighbour] = light_int - 1;
                g_queue_push_tail(light_queue, top_neighbour);
            }
        }
    }

    printf("map:\n");
    pretty_print_grid(world_map, COLS, 2);
    printf("light intensities:\n");
    pretty_print_grid(light_intensities, COLS, 2);

    free(light_sources);
}