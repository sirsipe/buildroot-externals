#include "raylib.h"

int main(void)
{
    InitWindow(800, 450, "raylib [core] example - basic window");

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(BLANK);
	    DrawText("Valtteri on kikkelipää!", 190, 200, 20, WHITE);
	    DrawText("Valtteri on kikkelipää!", 191, 201, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
