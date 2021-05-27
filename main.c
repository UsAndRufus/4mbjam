#include "raylib.h"

// Board
#define CELLS 6
#define TOTAL_CELLS (CELLS * CELLS)
#define CELL_SIZE 100
#define BOARD_SIZE ((CELLS * CELL_SIZE) + (CELLS + 1))
#define BOARD_BOUNDARY (BOARD_SIZE + BORDER)
#define BORDER 20

// Sidebar
#define SIDEBAR_WIDTH 200
#define SIDEBAR_HEIGHT BOARD_SIZE
#define SIDEBAR_X (BOARD_BOUNDARY + BORDER)
#define SIDEBAR_Y BORDER
#define PADDING 10
#define SIDEBAR_INNER_X (SIDEBAR_X + PADDING)
#define SIDEBAR_INNER_Y (SIDEBAR_Y + PADDING)

// Window
#define WINDOW_WIDTH (BOARD_SIZE + BORDER*3 + SIDEBAR_WIDTH)
#define WINDOW_HEIGHT (BOARD_SIZE + BORDER*2)

void initBoard(Rectangle cellRecs[TOTAL_CELLS]) {
    // Fills cellRecs data (for every rectangle)
    for (int i = 0; i < TOTAL_CELLS; i++) {
        cellRecs[i].x = BORDER + CELL_SIZE * (i % CELLS) + (i % CELLS);
        cellRecs[i].y = BORDER + 1 + CELL_SIZE * (i / CELLS) + (i / CELLS);
        cellRecs[i].width = CELL_SIZE;
        cellRecs[i].height = CELL_SIZE;
    }
}

void drawGrid() {
    for (int i = BORDER; i <= BOARD_BOUNDARY; i += CELL_SIZE + 1) {
        DrawLine(i, BORDER, i, BOARD_BOUNDARY, SKYBLUE);
        DrawLine(BORDER, i, BOARD_BOUNDARY - 1, i, SKYBLUE);
    }
}

void drawBoard(Rectangle cellRecs[TOTAL_CELLS], int cellState[TOTAL_CELLS]) {
    drawGrid();
  
    for (int i = 0; i < TOTAL_CELLS; i++) {
        if (cellState[i])
        {
            DrawRectangleRec(cellRecs[i], LIME);
        }
    }
}

void drawSidebar() {
    DrawRectangle(SIDEBAR_X, SIDEBAR_Y, SIDEBAR_WIDTH, SIDEBAR_HEIGHT, SKYBLUE);
        
    DrawFPS(SIDEBAR_INNER_X,SIDEBAR_INNER_Y);
}


int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "4mb game");

    Rectangle cellRecs[TOTAL_CELLS] = { 0 };     // Rectangles array

    initBoard(cellRecs);
    
    int cellState[TOTAL_CELLS] = { 0 };          // Cell state: 0-DEFAULT, 1-MOUSE_HOVER

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

            drawBoard(cellRecs, cellState);
            drawSidebar();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}