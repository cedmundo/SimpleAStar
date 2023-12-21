#include <raylib.h>

int main() {
    InitWindow(800, 800, "SimpleAStar");
    SetWindowMonitor(0);

    while(!WindowShouldClose()) {
        BeginDrawing();
        {
            DrawText("Hello world!", 100, 100, 18, WHITE);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
