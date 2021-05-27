#include "raylib.h"

#define CELLS 6
#define TOTAL_CELLS (CELLS * CELLS)
#define CELL_SIZE 100
#define BOARD_SIZE ((CELLS * CELL_SIZE) + (CELLS + 1))
#define BOARD_BOUNDARY (BOARD_SIZE + BORDER)
#define BORDER 20

void drawGrid() {
    for (int i = BORDER; i <= BOARD_BOUNDARY; i += CELL_SIZE + 1) {
        DrawLine(i, BORDER, i, BOARD_BOUNDARY, SKYBLUE);
        DrawLine(BORDER, i, BOARD_BOUNDARY - 1, i, SKYBLUE);
    }
}


int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(BOARD_SIZE + BORDER*2, BOARD_SIZE + BORDER*2, "4mb game");

    Rectangle cellRecs[TOTAL_CELLS] = { 0 };     // Rectangles array

    // Fills cellRecs data (for every rectangle)
    for (int i = 0; i < TOTAL_CELLS; i++) {
        cellRecs[i].x = BORDER + CELL_SIZE * (i % CELLS) + (i % CELLS);
        cellRecs[i].y = BORDER + 1 + CELL_SIZE * (i / CELLS) + (i / CELLS);
        cellRecs[i].width = CELL_SIZE;
        cellRecs[i].height = CELL_SIZE;
    }
    
    int cellState[TOTAL_CELLS] = { 0 };           // Cell state: 0-DEFAULT, 1-MOUSE_HOVER

    Vector2 mousePoint = { 0.0f, 0.0f };

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------

        mousePoint = GetMousePosition();

        for (int i = 0; i < TOTAL_CELLS; i++) {
            if (CheckCollisionPointRec(mousePoint, cellRecs[i])) cellState[i] = 1;
            else cellState[i] = 0;
        }
        
        // printf("x: %f; y: %f\n", mousePoint.x, mousePoint.y);

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(DARKBLUE);

            drawGrid();
            
            for (int i = 0; i < TOTAL_CELLS; i++) {
                if (cellState[i])
                {
                    DrawRectangleRec(cellRecs[i], LIME);
                }
            }
            
            DrawFPS(1,1);
            

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}