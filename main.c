#include <stdio.h>

#include "raylib.h"
#include "defs.h"

Ruleset initRuleset() {
    Ruleset ruleset = {
        2,
        {VIOLET, MAROON}
    };
    return ruleset;
}

void initBoard(Rectangle cellRecs[TOTAL_CELLS]) {
    // Fills cellRecs data (for every rectangle)
    for (int i = 0; i < TOTAL_CELLS; i++) {
        cellRecs[i].x = BORDER + CELL_SIZE * (i % CELLS) + (i % CELLS);
        cellRecs[i].y = BORDER + 1 + CELL_SIZE * (i / CELLS) + (i / CELLS);
        cellRecs[i].width = CELL_SIZE;
        cellRecs[i].height = CELL_SIZE;
    }
}

void initPieces(Ruleset ruleset, Piece cellPieces[TOTAL_CELLS]) {
    for (int p = 0; p < 2 * ruleset.numberOfPieces; p++) {
        Piece piece;
        piece.player = p % 2;
        piece.color = ruleset.colors[piece.player];
        cellPieces[p] = piece;
    }
}

void drawGrid() {
    for (int i = BORDER; i <= BOARD_BOUNDARY; i += CELL_SIZE + 1) {
        DrawLine(i, BORDER, i, BOARD_BOUNDARY, SKYBLUE);
        DrawLine(BORDER, i, BOARD_BOUNDARY - 1, i, SKYBLUE);
    }
}

void drawBoard(Rectangle cellRecs[TOTAL_CELLS], int cellState[TOTAL_CELLS], Piece cellPieces[TOTAL_CELLS]) {
    drawGrid();
  
    for (int i = 0; i < TOTAL_CELLS; i++) {
        DrawRectangleRec(cellRecs[i], cellPieces[i].color);
        
        if (cellState[i] != 0)
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
    
    Ruleset ruleset = initRuleset();
    printf("number of pieces: %d\n", ruleset.numberOfPieces);

    Rectangle cellRecs[TOTAL_CELLS] = { 0 };     // Rectangles array
    int cellState[TOTAL_CELLS] = { 0 };          // Cell state: 0-DEFAULT, 1-MOUSE_HOVER

    initBoard(cellRecs);
    
    Piece cellPieces[TOTAL_CELLS] = { 0 };
    initPieces(ruleset, cellPieces);

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
        

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(DARKBLUE);

            drawBoard(cellRecs, cellState, cellPieces);
            
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