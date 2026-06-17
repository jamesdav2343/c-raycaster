#include "lighting.h"
#include "general_utils.h"
#include "math_utils.h"
#include <glib.h>
#include <math.h>

#define NUM_DIRECTIONS 4
#define NUM_LIGHTS 3
#define INITIAL_INTENSITY 15

#define INTENSITY_DIFF 2
#define DECAY 1

#define MIN_SMOOTH_LIGHT_VAL 0.0f
#define MAX_SMOOTH_LIGHT_VAL 1.0f

float light_map[ROWS * COLS] = { 0 };
float smooth_light_map[ROWS * COLS] = { 0 };
static float* quadrants;

typedef struct Quadrant {
    float bottom_left;
    float bottom_right;
    float top_right;
    float top_left;
} Quadrant;

Quadrant get_vertices(int pos, float* quadrants)
{
    float tl = quadrants[pos * 4];
    float tr = quadrants[pos * 4 + 1];
    float bl = quadrants[pos * 4 + 2];
    float br = quadrants[pos * 4 + 3];

    Quadrant q = { 0 };
    q.top_left = tl;
    q.top_right = tr;
    q.bottom_left = bl;
    q.bottom_right = br;
    return q;
}

float get_lighting_floor(float x, float y, int pos)
{
    Quadrant quadrant = get_vertices(pos, quadrants);
    // printf("tl: %f, tr: %f, bl: %f, br: %f\n", quadrant.top_left, quadrant.top_right, quadrant.bottom_left, quadrant.bottom_right);
    return bilerp(x, 1.0f - y, quadrant.top_left, quadrant.top_right, quadrant.bottom_left, quadrant.bottom_right);
}

float get_smooth_light_value(Vector2I coords, Vector2I map_size, float* light_map)
{
    // Current cell is taken as bottom-right cell

    float bottom_left = 0.0f;
    if (coords.x > 0 && coords.y < map_size.y) {
        bottom_left = light_map[coords.x - 1 + map_size.x * coords.y];
    }

    float bottom_right = 0.0f;
    if (coords.x < map_size.x && coords.y < map_size.y) {
        bottom_right = light_map[coords.x + map_size.x * coords.y];
    }

    float top_right = 0.0f;
    if (coords.y > 0 && coords.x < map_size.x) {
        top_right = light_map[coords.x + map_size.x * (coords.y - 1)];
    }

    float top_left = 0.0f;
    if (coords.y > 0 && coords.x > 0) {
        top_left = light_map[coords.x - 1 + map_size.x * (coords.y - 1)];
    }

    // Return average light value of the cells that meet at position x, y
    return RAY_CLAMP(
        (bottom_right + bottom_left + top_right + top_left) / 4.0f, MIN_SMOOTH_LIGHT_VAL, MAX_SMOOTH_LIGHT_VAL);
}

void bake_smooth_light_map()
{
    Vector2I map_size = { COLS, ROWS };

    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            Vector2I coords = { x, y };
            smooth_light_map[x + (y * COLS)] = get_smooth_light_value(coords, map_size, light_map);
        }
    }

    int slm_length = COLS * ROWS;
    int quadrants_length = COLS * ROWS * 4;
    printf("length, x = %d * y = %d * 4, len = %d\n", map_size.x, map_size.y, quadrants_length);

    quadrants = (float*)calloc(quadrants_length, sizeof(float));

    // iterate over in chunks
    int quad_pos = 0;

    // smooth light map (slm) pos
    int slm_pos = 0;
    const int chunk_size = 4;

    while (quad_pos < quadrants_length) {
        float tl = 0.0f;

        if (slm_pos + (slm_pos / map_size.x) < slm_length) {
            tl = smooth_light_map[slm_pos + (slm_pos / map_size.x)];
        }

        float tr = 0.0f;

        if (slm_pos + 1 + (slm_pos / map_size.x) < slm_length) {
            tr = smooth_light_map[slm_pos + 1 + (slm_pos / map_size.x)];
        }

        float bl = 0.0f;

        if (slm_pos + map_size.x + 1 + (slm_pos / map_size.x) < slm_length) {
            bl = smooth_light_map[slm_pos + map_size.x + 1 + (slm_pos / map_size.x)];
        }

        float br = 0.0f;

        if (slm_pos + map_size.x + 2 + (slm_pos / map_size.x) < slm_length) {
            br = smooth_light_map[slm_pos + map_size.x + 2 + (slm_pos / map_size.x)];
        }

        quadrants[quad_pos] = tl;
        quadrants[quad_pos + 1] = tr;
        quadrants[quad_pos + 2] = bl;
        quadrants[quad_pos + 3] = br;

        // if (slm_pos <= 2) {
        //     printf("current pos: %d\n", slm_pos);
        //     printf("tl: %d\n", slm_pos + (slm_pos / map_size.x));
        //     printf("tr: %d\n", slm_pos + 1 + (slm_pos / map_size.x));
        //     printf("bl: %d\n", slm_pos + map_size.x + 1 + (slm_pos / map_size.x));
        //     printf("br: %d\n", slm_pos + map_size.x + 2 + (slm_pos / map_size.x));
        //     printf("values: %f, %f, %f, %f\n", tl, tr, bl, br);
        //     printf("map_size, x: %d, y: %d\n", map_size.x, map_size.y);
        //     printf("offset value used: %d\n", slm_pos / map_size.x);
        // }

        quad_pos += chunk_size;
        slm_pos++;
    }

    printf("vertices array info after chunks_mut\n");
    for (int i = 0; i < quadrants_length; i++) {
        printf("%f, ", quadrants[i]);
    }
    printf("\nend of array here\n");
}

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

    printf("map:\n");
    pretty_print_grid(world_map, COLS, 2);
    printf("light intensities:\n");
    pretty_print_grid(light_intensities, COLS, 2);
    printf("\n\n");

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
            return smooth_light_map[x + ((y + 1) * COLS)];
        else
            return smooth_light_map[x + ((y - 1) * COLS)];
    } else {
        if (ray_direction.x < 0)
            return smooth_light_map[x + 1 + (y * COLS)];
        else
            return smooth_light_map[x - 1 + (y * COLS)];
    }

    // if vertical, and x is
    return 0;
}