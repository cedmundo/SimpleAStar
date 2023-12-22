#include <stddef.h>
#include <stdlib.h>
#include <raylib.h>
#include "raymath.h"

#define GRID_ROWS 10
#define GRID_COLS 10
#define CELL_SIZE 80

enum cell {
    CELL_EMPTY,
    CELL_OBSTACLE,
    CELL_ORIGIN,
    CELL_TARGET,
    CELL_TYPE_COUNT,
};

struct grid {
    enum cell *cells;
    int rows;
    int cols;
    int cell_size;
};

struct grid *grid_new(int rows, int cols, int cell_size) {
    struct grid *grid = calloc(1, sizeof(struct grid));
    grid->cells = calloc(rows * cols, sizeof(enum cell));
    grid->rows = rows;
    grid->cols = cols;
    grid->cell_size = cell_size;
    return grid;
}

void grid_free(struct grid *grid) {
    if (grid->cells != NULL) {
        free(grid->cells);
    }

    free(grid);
}

void grid_draw(struct grid *grid) {
    int cell_size = grid->cell_size;
    for(int i=0;i<grid->cols;i++) {
        for(int j=0;j<grid->rows;j++) {
            Color colors[] = {
                    [CELL_EMPTY] = LIGHTGRAY,
                    [CELL_OBSTACLE] = DARKGRAY,
                    [CELL_ORIGIN] = SKYBLUE,
                    [CELL_TARGET] = BLUE,
            };
            enum cell cell_type = grid->cells[i * grid->cols + j];
            DrawRectangle(i*cell_size, j*cell_size, cell_size, cell_size, colors[cell_type]);
            DrawRectangleLines(i*cell_size, j*cell_size, cell_size, cell_size, BLACK);
            if (cell_type == CELL_ORIGIN) {
                DrawText("A", i*cell_size + cell_size/2 - 6, j*cell_size + cell_size/2 - 9, 18, WHITE);
            } else if (cell_type == CELL_TARGET) {
                DrawText("B", i*cell_size + cell_size/2 - 6, j*cell_size + cell_size/2 - 9, 18, WHITE);
            }
        }
    }
}

int main() {
    InitWindow(GRID_COLS * CELL_SIZE, GRID_ROWS * CELL_SIZE, "SimpleAStar");
    SetWindowMonitor(0);

    struct grid *grid = grid_new(GRID_ROWS, GRID_COLS, CELL_SIZE);
    while(!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 sel_coords = Vector2Scale(GetMousePosition(), 1.0f/CELL_SIZE);
                size_t index = grid->cols * (int)sel_coords.x + (int)sel_coords.y;
                grid->cells[index] = (grid->cells[index] + 1) % CELL_TYPE_COUNT;
            }
            grid_draw(grid);
        }
        EndDrawing();
    }

    grid_free(grid);
    CloseWindow();
    return 0;
}
