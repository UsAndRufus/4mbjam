#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "raylib.h"
#include "utility.h"
#include "defs.h"

void generatePieceDefs(Ruleset *ruleset) {
    int chosenSides[ruleset->numberOfPieceDefs];
    choose(chosenSides, ruleset->numberOfPieceDefs, 4);
    
    for (int i = 0; i < ruleset->numberOfPieceDefs; i++) {
        // could define an alternate version of choose that defines a floor, but this is fine
        int sides = chosenSides[i] + 3; // sides in range 3-6
        MovementDirection movementDirection = rand() % MOVEMENT_DIRECTION_COUNT;
        
        PieceDef pieceDef = {
            sides,
            movementDirection
        };
        ruleset->pieceDefs[i] = pieceDef;
    }
}

Ruleset initRuleset() {
    
    static Color playerColors[] = {VIOLET, MAROON, DARKGREEN, PINK, PURPLE, BEIGE};
    
    Ruleset ruleset = {
        N_PIECE_DEFS,
        N_PIECE_DEFS * 2, // total number of pieces
        {VIOLET, MAROON},
        {}
    };
    
    // Can implement a chooseColors function at some point if we need to do more of this
    int colorIds[2];
    choose(colorIds, 2, ARR_SIZE(playerColors));
    
    ruleset.colors[0] = playerColors[colorIds[0]];
    ruleset.colors[1] = playerColors[colorIds[1]];
       
    generatePieceDefs(&ruleset);
    
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

void initPieces(Ruleset ruleset, Piece pieces[]) {    
    int positions[ruleset.numberOfPieceDefs];
    choose(positions, ruleset.numberOfPieceDefs, HOME_CELLS);
    
    for (int pieceDef = 0; pieceDef < ruleset.numberOfPieceDefs; pieceDef++) {
        int position = positions[pieceDef];
        
        for (int player = 0; player < 2; player++) {
            // rotate positions
            if (player == 1) {
                position = TOTAL_CELLS - position - 1;
            }
            
            Piece piece = {
                position,
                player,
                pieceDef
            };
            
            int i = pieceDef + (player * ruleset.numberOfPieceDefs);
            
            
            pieces[i] = piece;
        }
    }
}

void drawGrid() {
    for (int i = BORDER; i <= BOARD_BOUNDARY; i += CELL_SIZE + 1) {
        DrawLine(i, BORDER, i, BOARD_BOUNDARY, SKYBLUE);
        DrawLine(BORDER, i, BOARD_BOUNDARY - 1, i, SKYBLUE);
    }
}

// will have to do some maths anyway to work out what angle to draw arrowhead
void drawArrow(Vector2 start, int xAdd, int yAdd, Color color) {
    Vector2 end = { start.x + xAdd, start.y + yAdd };
    
    DrawLineEx(start, end, 5, LIME);
}

void drawMovementHint(PieceDef pieceDef, Vector2 center) {
    switch(pieceDef.movementDirection) {
        case OMNI: 
        case ORTHOGONAL:
            drawArrow(center, 0, CELL_SIZE, LIME);
            drawArrow(center, 0, -CELL_SIZE, LIME);
            drawArrow(center, CELL_SIZE, 0, LIME);
            drawArrow(center, -CELL_SIZE, 0, LIME);
            if (pieceDef.movementDirection == ORTHOGONAL) break; // fall through if omni
        case DIAGONAL:
            drawArrow(center, CELL_SIZE, CELL_SIZE, LIME);
            drawArrow(center, CELL_SIZE, -CELL_SIZE, LIME);
            drawArrow(center, -CELL_SIZE, CELL_SIZE, LIME);
            drawArrow(center, -CELL_SIZE, -CELL_SIZE, LIME);
            break;
    }
}

void drawBoard(Rectangle cellRecs[TOTAL_CELLS], int cellState[TOTAL_CELLS], Piece pieces[], Ruleset ruleset) {
    drawGrid();
  
    for (int i = 0; i < TOTAL_CELLS; i++) {
        if (cellState[i] != 0)
        {
            DrawRectangleRec(cellRecs[i], LIME);
        }
    }
    
    for (int p = 0; p < ruleset.numberOfPieces; p++) {
        Piece piece = pieces[p];
        int cell = piece.position;
        PieceDef pieceDef = ruleset.pieceDefs[piece.pieceDef];
        
        Vector2 center = { cellRecs[cell].x + HALF_CELL_SIZE, cellRecs[cell].y + HALF_CELL_SIZE };
        
        if (cellState[cell] != 0) {
            drawMovementHint(pieceDef, center);
        }
        
        DrawPoly(center, pieceDef.sides, PIECE_RADIUS, 180 * (piece.player), ruleset.colors[piece.player]);
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
    
    int length = snprintf(NULL, 0,"SEED: %d", SEED) + 1;
    char seed_str[length];
    snprintf(seed_str, length, "SEED: %d", SEED);
    
    // Ruleset
    
    Ruleset ruleset = initRuleset();
    
    // Board

    Rectangle cellRecs[TOTAL_CELLS] = { 0 };     // Rectangles array
    int cellState[TOTAL_CELLS] = { 0 };          // Cell state: 0-DEFAULT, 1-MOUSE_HOVER

    initBoard(cellRecs);
    
    Piece pieces[ruleset.numberOfPieces];
    initPieces(ruleset, pieces);

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

            drawBoard(cellRecs, cellState, pieces, ruleset);
            
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