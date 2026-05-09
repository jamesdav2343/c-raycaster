#include "debug.h"
#include "map.h"
#include <limits.h>

#define BUFFER_MAX 1024

static int num_places(int n)
{
    if (n < 0)
        return num_places((n == INT_MIN) ? INT_MAX : -n);
    if (n < 10)
        return 1;
    return 1 + num_places(n / 10);
}

void pretty_print_grid(Uint8* grid, int row_length, int max_gap)
{
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            Uint8 val = grid[x + (y * COLS)];
            Uint8 num_digits = (Uint8)num_places(val);

            char grid_item[BUFFER_MAX] = "";
            char space[] = " ";

            for (int i = 0; i < max_gap - num_digits; i++) {
                strncat(grid_item, space, BUFFER_MAX);
            }

            char val_buffer[BUFFER_MAX];
            snprintf(val_buffer, BUFFER_MAX, "%d", val);
            strncat(grid_item, val_buffer, BUFFER_MAX);

            printf("%s, ", grid_item);
        }
        printf("\n");
    }
}