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
float all_vertices[ALL_VERTICES_WIDTH * ALL_VERTICES_HEIGHT] = { 0 };
static float* vertices;

typedef struct Quadrant {
    float bottom_left;
    float bottom_right;
    float top_right;
    float top_left;
} Quadrant;

static Quadrant get_vertices(int pos, float* vertices)
{
    float tl = vertices[pos * 4];
    float tr = vertices[pos * 4 + 1];
    float bl = vertices[pos * 4 + 2];
    float br = vertices[pos * 4 + 3];

    return (Quadrant) { bl, br, tr, tl };
}

float get_lighting_floor(float x, float y, int pos)
{
    Quadrant quadrant = get_vertices(pos, vertices);
    return bilerp(x, 1.0f - y, quadrant.top_left, quadrant.top_right, quadrant.bottom_left, quadrant.bottom_right);
}

static float get_smooth_light_value(Vector2I coords, Vector2I map_size, float* light_map)
{
    int x = coords.x;
    int y = coords.y;

    // Current cell is taken as bottom-right cell
    float bl = 0.0f;
    if (x > 0 && y < map_size.y) {
        bl = light_map[x - 1 + map_size.x * y];
    }

    float br = 0.0f;
    if (x < map_size.x && y < map_size.y) {
        br = light_map[x + map_size.x * y];
    }

    float tr = 0.0f;
    if (y > 0 && x < map_size.x) {
        tr = light_map[x + map_size.x * (y - 1)];
    }

    float tl = 0.0f;
    if (y > 0 && x > 0) {
        tl = light_map[x - 1 + map_size.x * (y - 1)];
    }

    // Return average light value of the cells that meet at position x, y
    return RAY_CLAMP((br + bl + tr + tl) / 4.0f, MIN_SMOOTH_LIGHT_VAL, MAX_SMOOTH_LIGHT_VAL);
}

void bake_smooth_light_map()
{
    Vector2I map_size = { COLS, ROWS };

    for (int x = 0; x < ALL_VERTICES_WIDTH; x++) {
        for (int y = 0; y < ALL_VERTICES_HEIGHT; y++) {
            Vector2I coords = { x, y };
            all_vertices[x + (y * ALL_VERTICES_WIDTH)] = get_smooth_light_value(coords, map_size, light_map);
        }
    }

    print_array(all_vertices, (Vector2I) { ALL_VERTICES_WIDTH, ALL_VERTICES_HEIGHT });

    int vertices_len = map_size.x * map_size.y;
    int quadrants_length = vertices_len * 4;

    vertices = (float*)calloc(quadrants_length, sizeof(float));

    for (int pos = 0; pos < vertices_len; pos++) {
        int tl_idx = pos + (pos / map_size.x);
        float tl = all_vertices[tl_idx];

        int tr_idx = pos + 1 + (pos / map_size.x);
        float tr = all_vertices[tr_idx];

        int bl_idx = pos + map_size.x + 1 + (pos / map_size.x);
        float bl = all_vertices[bl_idx];

        int br_idx = pos + map_size.x + 2 + (pos / map_size.x);
        float br = all_vertices[br_idx];

        vertices[pos * 4] = tl;
        vertices[pos * 4 + 1] = tr;
        vertices[pos * 4 + 2] = bl;
        vertices[pos * 4 + 3] = br;
    }
}

void bake_light_map()
{
    size_t* light_sources = calloc(NUM_LIGHTS, sizeof(size_t));

    // This is where the light sources are set, this should be changed to be more flexible
    light_sources[0] = 3 + (2 * COLS); // x: 3, y: 2
    light_sources[1] = 2 + (14 * COLS); // x: 2, y: 14
    light_sources[2] = 10 + (16 * COLS); // x: 10, y: 16

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
            // less than the light intensity of the current node
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

    for (int i = 0; i < ROWS * COLS; i++) {
        Uint8 intensity = light_intensities[i];
        light_map[i] = powf(0.8f, INITIAL_INTENSITY - intensity);
    }

    free(light_sources);
}

float get_wall_light_intensity(int x, int y, Vector2 ray_direction, enum Orientation orientation)
{
    // If side orientation is horizontal, and the ray is coming from south (negative y direction),
    // then access the cell below (y + 1)
    if (orientation == SOUTH || orientation == NORTH) {
        if (ray_direction.y < 0)
            return all_vertices[x + (y + 1) * ALL_VERTICES_WIDTH]; // FACING SOUTH
        else
            return all_vertices[x + (y - 1) * ALL_VERTICES_WIDTH]; // FACING NORTH
    } else {
        if (ray_direction.x < 0)
            return all_vertices[x + 1 + (y * ALL_VERTICES_WIDTH)]; // FACING EAST
        else
            return all_vertices[x - 1 + (y * ALL_VERTICES_WIDTH)]; // FACING WEST
    }

    return 0.0f;
}

float get_lighting_wall(float x, float y, int pos, enum Orientation orientation)
{
    Quadrant quadrant;
    switch (orientation) {
    case NORTH:
        quadrant = get_vertices(pos - COLS, vertices);

        if (y > 2.0f) {
            return bilerp(
                x, 3.0f - y, quadrant.top_left, quadrant.top_right, quadrant.bottom_left, quadrant.bottom_right);
        } else if (y > 1.0f) {
            return bilerp(
                x, 2.0f - y, quadrant.top_left, quadrant.top_right, quadrant.top_left, quadrant.top_right);
        } else {
            return bilerp(
                x, 1.0f - y, quadrant.bottom_left, quadrant.bottom_right, quadrant.top_left, quadrant.top_right);
        }
    case SOUTH:
        quadrant = get_vertices(pos + COLS, vertices);

        if (y > 2.0f) {
            return bilerp(
                1.0f - x, 3.0f - y, quadrant.bottom_left, quadrant.bottom_right, quadrant.top_left, quadrant.top_right);
        } else if (y > 1.0f) {
            return bilerp(
                1.0f - x, 2.0f - y, quadrant.bottom_left, quadrant.bottom_right, quadrant.bottom_left, quadrant.bottom_right);
        } else {
            return bilerp(
                1.0f - x, 1.0f - y, quadrant.top_left, quadrant.top_right, quadrant.bottom_left, quadrant.bottom_right);
        }
    case EAST:
        quadrant = get_vertices(pos - 1, vertices);

        if (y > 2.0f) {
            return bilerp(
                1.0f - x, 3.0f - y, quadrant.top_left, quadrant.bottom_left, quadrant.top_right, quadrant.bottom_right);
        } else if (y > 1.0f) {
            return bilerp(
                1.0f - x, 2.0f - y, quadrant.top_left, quadrant.bottom_left, quadrant.top_left, quadrant.bottom_left);
        } else {
            return bilerp(
                1.0f - x, 1.0f - y, quadrant.top_right, quadrant.bottom_right, quadrant.top_left, quadrant.bottom_left);
        }
    case WEST:
        quadrant = get_vertices(pos + 1, vertices);

        if (y > 2.0f) {
            return bilerp(
                1.0f - x, 3.0f - y, quadrant.bottom_right, quadrant.top_right, quadrant.bottom_left, quadrant.top_left);
        } else if (y > 1.0f) {
            return bilerp(
                1.0f - x, 2.0f - y, quadrant.bottom_right, quadrant.top_right, quadrant.bottom_right, quadrant.top_right);
        } else {
            return bilerp(
                1.0f - x, 1.0f - y, quadrant.bottom_left, quadrant.top_left, quadrant.bottom_right, quadrant.top_right);
        }
    }

    return 0.0f;
}