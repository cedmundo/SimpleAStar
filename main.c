#include <stddef.h>
#include <stdlib.h>
#include <raylib.h>
#include "raymath.h"

#define GRID_ROWS 15
#define GRID_COLS 15
#define CELL_SIZE 50

enum cell {
    CELL_EMPTY,
    CELL_OBSTACLE,
};

struct grid {
    enum cell *cells;
    int rows;
    int cols;
    int cell_size;
};

struct node {
    enum cell cell;
    int index;
    int g_score;
    int h_score;
    int f_score;
    struct node *parent;
    struct node *next;
    struct node *prev;
};

struct node_list {
    struct node *head;
    struct node *tail;
    int count;
};

struct node *node_new(enum cell cell, int index, int g_score, int h_score, struct node *parent) {
    struct node *node = calloc(1, sizeof(struct node));
    node->cell = cell;
    node->index = index;
    node->g_score = g_score;
    node->h_score = h_score;
    node->f_score = g_score + h_score;
    node->parent = parent;
    return node;
}

struct grid *grid_new(int rows, int cols, int cell_size) {
    struct grid *grid = calloc(1, sizeof(struct grid));
    grid->cells = calloc(rows * cols, sizeof(enum cell));
    grid->rows = rows;
    grid->cols = cols;
    grid->cell_size = cell_size;
    return grid;
}

struct node_list *node_list_new();

void grid_free(struct grid *grid) {
    if (grid->cells != NULL) {
        free(grid->cells);
    }

    free(grid);
}

struct node_list *node_list_new() {
    return calloc(1, sizeof(struct node_list));
}

void node_list_push(struct node_list *list, struct node *node) {
    if (list->head == NULL) {
        list->head = node;
    }

    if (list->tail != NULL) {
        list->tail->next = node;
    }

    node->prev = list->tail;
    list->tail = node;
    list->count += 1;
}

struct node *node_list_find(struct node_list *list, int index) {
    struct node *cur = list->head;
    while (cur != NULL) {
        if (cur->index == index) {
            return cur;
        }

        cur = cur->next;
    }

    return NULL;
}

struct node *node_list_pop_lowest_f_score(struct node_list *list) {
    struct node *current = list->head;
    struct node *lowest = NULL;

    while (current != NULL) {
        if (lowest == NULL || current->f_score <= lowest->f_score) {
            lowest = current;
        }

        current = current->next;
    }

    if (lowest != NULL) {
        // Delete from list
        struct node *prev = lowest->prev;
        if (lowest->prev != NULL) {
            lowest->prev->next = lowest->next;
        }

        if (lowest->next != NULL) {
            lowest->next->prev = prev;
        }

        if (list->head == lowest) {
            list->head = lowest->next;
        }

        if (list->tail == lowest) {
            list->tail = lowest->prev;
        }

        lowest->prev = NULL;
        lowest->next = NULL;
        list->count--;
    }
    return lowest;
}


void node_list_free(struct node_list *list) {
    struct node *current = list->head;
    struct node *tmp = NULL;
    while (current != NULL) {
        tmp = current->next;
        free(current);
        current = tmp;
    }
    free(list);
}

void node_list_draw(struct grid *grid, struct node_list *list, int origin_index, int target_index, Color color) {
    int cell_size = grid->cell_size;
    struct node *current = list->head;
    while (current != NULL) {
        int i = current->index / grid->cols;
        int j = current->index % grid->rows;
        DrawRectangle(i * cell_size, j * cell_size, cell_size, cell_size, color);
        DrawRectangleLines(i * cell_size, j * cell_size, cell_size, cell_size, BLACK);
        /*const char *caption = TextFormat("h=%d g=%d f=%d", current->h_score, current->g_score,
                                         current->h_score + current->g_score);
        DrawText(caption, i * cell_size + 6, j * cell_size + 10, 10, BLACK);*/
        if (current->index == origin_index) {
            DrawText("A", i * cell_size + cell_size / 2 - 6, j * cell_size + cell_size / 2 - 9, 18, WHITE);
        } else if (current->index == target_index) {
            DrawText("B", i * cell_size + cell_size / 2 - 6, j * cell_size + cell_size / 2 - 9, 18, WHITE);
        }
        current = current->next;
    }
}

void grid_draw(struct grid *grid, int origin, int target) {
    int cell_size = grid->cell_size;
    for (int i = 0; i < grid->cols; i++) {
        for (int j = 0; j < grid->rows; j++) {
            Color colors[] = {
                    [CELL_EMPTY] = LIGHTGRAY,
                    [CELL_OBSTACLE] = DARKGRAY,
            };
            size_t index = i * grid->cols + j;
            if (index == origin) {
                DrawRectangle(i * cell_size, j * cell_size, cell_size, cell_size, SKYBLUE);
                DrawText("A", i * cell_size + cell_size / 2 - 6, j * cell_size + cell_size / 2 - 9, 18, WHITE);
            } else if (index == target) {
                DrawRectangle(i * cell_size, j * cell_size, cell_size, cell_size, DARKBLUE);
                DrawText("B", i * cell_size + cell_size / 2 - 6, j * cell_size + cell_size / 2 - 9, 18, WHITE);
            } else {
                enum cell cell_type = grid->cells[index];
                DrawRectangle(i * cell_size, j * cell_size, cell_size, cell_size, colors[cell_type]);
            }
            // DrawText(TextFormat("%d:%d,%d", index, j, i), i * cell_size + 2, j * cell_size + 2, 8, BLACK);
            DrawRectangleLines(i * cell_size, j * cell_size, cell_size, cell_size, BLACK);
        }
    }
}

int grid_get_distance(struct grid *grid, int from_index, int to_index) {
    int from_i = from_index / grid->cols;
    int from_j = from_index % grid->rows;
    int to_i = to_index / grid->cols;
    int to_j = to_index % grid->rows;
    return (from_i - to_i) * (from_i - to_i) + (from_j - to_j) * (from_j - to_j);
}

void grid_get_neighbors(struct grid *grid, int index, struct node neighbors[8], int *found_neighbors) {
    int xc = index / grid->cols;
    int yc = index % grid->rows;
    for (int i = (yc == 0 ? 0 : -1); i <= (yc >= grid->rows - 1 ? 0 : 1); i++) {
        for (int j = (xc == 0 ? 0 : -1); j <= (xc == grid->rows - 1 ? 0 : 1); j++) {
            int ni = ((index / grid->cols) + j) * grid->cols + ((index % grid->rows) + i);
            if (i == 0 && j == 0) {
                continue;
            }

            enum cell cell = grid->cells[ni];
            if (cell == CELL_OBSTACLE) {
                continue;
            }

            neighbors[*found_neighbors] = (struct node) {
                    .cell = cell,
                    .index = ni,
            };
            *found_neighbors = *found_neighbors + 1;
        }
    }
}

void grid_find_path(struct grid *grid, int origin_index, int target_index, struct node_list *open_list,
                    struct node_list *closed_list) {
    // Put the starting node into open list
    int starting_h_score = grid_get_distance(grid, origin_index, target_index);
    struct node *starting_node = node_new(grid->cells[origin_index], origin_index, 0, starting_h_score, NULL);
    node_list_push(open_list, starting_node);

    while (open_list->count > 0) {
        struct node *q = node_list_pop_lowest_f_score(open_list);
        struct node neighbors[8] = {0};
        int neighbors_count = 0;

        // Iterate over q's neighbors:
        grid_get_neighbors(grid, q->index, neighbors, &neighbors_count);
        for (int i = 0; i < neighbors_count; i++) {
            struct node neighbor = neighbors[i];
            neighbor.parent = q;
            neighbor.h_score = grid_get_distance(grid, neighbor.index, target_index);
            neighbor.g_score = q->g_score + grid_get_distance(grid, neighbor.index, q->index);
            neighbor.f_score = neighbor.h_score + neighbor.g_score;
            if (neighbor.index == target_index) {
                node_list_push(closed_list, q);
                return;
            }

            struct node *lookup = NULL;
            lookup = node_list_find(open_list, neighbor.index);
            if (lookup != NULL && lookup->f_score <= neighbor.f_score) {
                continue;
            }

            lookup = node_list_find(closed_list, neighbor.index);
            if (lookup != NULL && lookup->f_score <= neighbor.f_score) {
                continue;
            }

            struct node *cpy = node_new(neighbor.cell, neighbor.index, neighbor.g_score, neighbor.h_score,
                                        neighbor.parent);
            node_list_push(open_list, cpy);
        }

        node_list_push(closed_list, q);
    }
}

int grid_world_to_index(struct grid *grid, Vector2 world) {
    Vector2 xy = Vector2Scale(world, 1.0f / (float) grid->cell_size);
    return grid->cols * (int) xy.x + (int) xy.y;
}

int main() {
    InitWindow(GRID_COLS * CELL_SIZE, GRID_ROWS * CELL_SIZE, "SimpleAStar");
    SetWindowMonitor(0);
    SetTargetFPS(60);

    struct grid *grid = grid_new(GRID_ROWS, GRID_COLS, CELL_SIZE);
    int origin_index = -1;
    int target_index = -1;

    struct node_list *open_list = NULL;
    struct node_list *closed_list = NULL;

    bool drawing_obstacles = true;
    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);
            if (IsKeyPressed(KEY_R)) {
                for (int i=0;i<grid->rows * grid->cols;i++) {
                    grid->cells[i] = CELL_EMPTY;
                }

                origin_index = -1;
                target_index = -1;
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                origin_index = grid_world_to_index(grid, GetMousePosition());
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                target_index = grid_world_to_index(grid, GetMousePosition());
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) && drawing_obstacles) {
                int index = grid_world_to_index(grid, GetMousePosition());
                grid->cells[index] = CELL_OBSTACLE;
            } else if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) && !drawing_obstacles) {
                int index = grid_world_to_index(grid, GetMousePosition());
                grid->cells[index] = CELL_EMPTY;
            }

            if (IsKeyPressed(KEY_ONE)) {
                drawing_obstacles = true;
            } else if (IsKeyPressed(KEY_TWO)) {
                drawing_obstacles = false;
            }

            // Find path and draw
            if (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_R) || IsKeyPressed(KEY_SPACE)
                || IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
                || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                if (open_list != NULL) {
                    node_list_free(open_list);
                }

                if (closed_list != NULL) {
                    node_list_free(closed_list);
                }

                open_list = NULL;
                closed_list = NULL;
            }

            if (IsKeyPressed(KEY_SPACE) && origin_index != -1 && target_index != -1) {
                open_list = node_list_new();
                closed_list = node_list_new();
                grid_find_path(grid, origin_index, target_index, open_list, closed_list);
            }

            grid_draw(grid, origin_index, target_index);
            if (open_list != NULL) {
                node_list_draw(grid, open_list, origin_index, target_index, GREEN);
            }

            if (closed_list != NULL) {
                node_list_draw(grid, closed_list, origin_index, target_index, RED);
            }

            if (open_list != NULL && closed_list != NULL) {
                DrawText(TextFormat("open_list=%d closed_list=%d", open_list->count, closed_list->count), 0, 30, 18, PURPLE);
            }
            DrawFPS(0, 0);
        }
        EndDrawing();
    }

    if (open_list != NULL) {
        node_list_free(open_list);
    }

    if (closed_list != NULL) {
        node_list_free(closed_list);
    }

    grid_free(grid);
    CloseWindow();
    return 0;
}
