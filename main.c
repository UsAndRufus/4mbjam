#include "raylib.h"

#define CELLS 6
#define CELL_SIZE 100
#define BOARD_SIZE ((CELLS * CELL_SIZE) + (CELLS * 1))
#define BOARD_BOUNDARY (BOARD_SIZE + BORDER)
#define BORDER 10

void drawGrid() {
    for (int i = BORDER; i <= BOARD_BOUNDARY; i += CELL_SIZE + 1) {
        DrawLine(i, BORDER, i, BOARD_BOUNDARY, SKYBLUE);
        DrawLine(BORDER, i, BOARD_BOUNDARY, i, SKYBLUE);
    }
}


int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(BOARD_SIZE + BORDER*2, BOARD_SIZE + BORDER*2, "4mb game");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(DARKBLUE);

            drawGrid();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}