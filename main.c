#include <stdlib.h>
#include <stdio.h>

#include "raylib.h"
#include "defs.h"

Ruleset initRuleset() {
    
    static Color playerColors[] = {VIOLET, MAROON, DARKGREEN, PINK, PURPLE, BEIGE};
    
    Ruleset ruleset = {
        3,
        {VIOLET, MAROON}
    };
    
    ruleset.colors[0] = playerColors[rand() % ARR_SIZE(playerColors)];
    
    // ensure player 2's colour is different
    Color color2 = ruleset.colors[0];
    while (ColorToInt(color2) == ColorToInt(ruleset.colors[0])) {
        color2 = playerColors[rand() % ARR_SIZE(playerColors)];
        ruleset.colors[1] = color2;
    }
    
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
    int positions[TOTAL_CELLS] = {0};
    
    for (int p = 0; p < ruleset.numberOfPieces; p++) {
        int sides = (rand() % 3) + 3; // sides in range 3-6
        
        // Get unique position
        int position;
        for (;;) {
            position = rand() % HOME_CELLS;
            if (positions[position] == 0) {
                break;
            }
        }
        positions[position] = 1;
        
        for (int player = 0; player < 2; player++) {
            Piece piece = {
                player,
                ruleset.colors[piece.player],
                sides
            };
            
            // rotate positions
            if (player == 1) {
                position = TOTAL_CELLS - position;
            }
            
            
            cellPieces[position] = piece;
            position++;
        }
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
        if (cellState[i] != 0)
        {
            DrawRectangleRec(cellRecs[i], LIME);
        }
        
        Vector2 center = { cellRecs[i].x + HALF_CELL_SIZE, cellRecs[i].y + HALF_CELL_SIZE };
        DrawPoly(center, cellPieces[i].sides, PIECE_RADIUS, 180 * (cellPieces[i].player), cellPieces[i].color);
    }
}

void drawSidebar(char *seed_str) {
    DrawRectangle(SIDEBAR_X, SIDEBAR_Y, SIDEBAR_WIDTH, SIDEBAR_HEIGHT, SKYBLUE);
    
    int y = SIDEBAR_INNER_Y;
        
    DrawFPS(SIDEBAR_INNER_X, y);
    y += SIDEBAR_LINE_HEIGHT;
    
    DrawText(seed_str, SIDEBAR_INNER_X, y, TEXT_SIZE, LIME); 
}


int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "4mb game");
    
    // seed gen
    
    const int SEED = time(0) % 10000;
    srand(SEED);
    
    int length = snprintf(NULL, 0,"%d", SEED) + 1;
    char seed_str[length];
    snprintf(seed_str, length, "%d", SEED);
    
    // Ruleset
    
    Ruleset ruleset = initRuleset();
    printf("number of pieces: %d\n", ruleset.numberOfPieces);
    
    // Board

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
            
            drawSidebar(seed_str);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}