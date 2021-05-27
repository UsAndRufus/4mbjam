#include "raylib.h"

#define CELLS 6
#define TOTAL_CELLS (CELLS * CELLS)
#define CELL_SIZE 100
#define BOARD_SIZE ((CELLS * CELL_SIZE) + (CELLS + 1))
#define BOARD_BOUNDARY (BOARD_SIZE + BORDER)
#define BORDER 10

void drawGrid() {
    for (int i = BORDER; i <= BOARD_BOUNDARY; i += CELL_SIZE + 1) {
        DrawLine(i, BORDER, i, BOARD_BOUNDARY, SKYBLUE);
        DrawLine(BORDER, i, BOARD_BOUNDARY - 1, i, SKYBLUE);
    }
}


int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(BOARD_SIZE + BORDER*2, BOARD_SIZE + BORDER*2, "4mb game");

    Rectangle colorsRecs[TOTAL_CELLS] = { 0 };     // Rectangles array

    // Fills colorsRecs data (for every rectangle)
    for (int i = 0; i < TOTAL_CELLS; i++)
    {
        colorsRecs[i].x = BORDER + CELL_SIZE * (i % CELLS) + (i % CELLS);
        colorsRecs[i].y = BORDER + 1 + CELL_SIZE * (i / CELLS) + (i / CELLS);
        colorsRecs[i].width = CELL_SIZE;
        colorsRecs[i].height = CELL_SIZE;
    }
    
    int colorState[TOTAL_CELLS] = { 0 };           // Color state: 0-DEFAULT, 1-MOUSE_HOVER

    Vector2 mousePoint = { 0.0f, 0.0f };

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        mousePoint = GetMousePosition();

        for (int i = 0; i < TOTAL_CELLS; i++)
        {
            if (CheckCollisionPointRec(mousePoint, colorsRecs[i])) colorState[i] = 1;
            else colorState[i] = 0;
        }
        
        // printf("x: %f; y: %f\n", mousePoint.x, mousePoint.y);

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(DARKBLUE);

            drawGrid();
            
            for (int i = 0; i < TOTAL_CELLS; i++) 
            {
                if (colorState[i])
                {
                    DrawRectangleRec(colorsRecs[i], LIME);
                }
            }
            

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}