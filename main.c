#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "raylib.h"
#include "utility.h"
#include "defs.h"

// init and generation

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
            
            pieces[position] = (Piece) {
                1, // present
                player,
                pieceDef
            };
        }
    }
}

// Logic
int cellOnBoard(int cell) {
    return cell >= 0 && cell < TOTAL_CELLS;
}

int movingIntoSamePlayerPiece(int from, int to, Piece pieces[TOTAL_CELLS]) {
    return pieces[to].present && pieces[to].player == pieces[from].player;
}


// This needs to return a variable length array
// We could just continue to calculate in the drawMovementHint function BUT this function will be useful for the bot and checking whether a player's move is valid
// BUT if piece will never have more than 8 moves... we could just have an array initialised to size 8, elements at -1?
void validMovesFor(PieceDef pieceDef, int cell, int moves[TOTAL_CELLS], Piece pieces[TOTAL_CELLS]) {
    switch(pieceDef.movementDirection) {
        int target;
        case OMNI: 
        case ORTHOGONAL:
            // up and down
            target = cell - CELLS;
            if (cellOnBoard(target) && !movingIntoSamePlayerPiece(cell, target, pieces)) moves[target] = 1;
            
            target = cell + CELLS;
            if (cellOnBoard(target) && !movingIntoSamePlayerPiece(cell, target, pieces)) moves[target] = 1;
            
            // left and right
            target = cell - 1;
            if (cell % CELLS != 0 && !movingIntoSamePlayerPiece(cell, target, pieces)) moves[target] = 1;
            
            target = cell + 1;
            if ((cell + 1) % CELLS != 0 && !movingIntoSamePlayerPiece(cell, target, pieces)) moves[target] = 1;
            
            if (pieceDef.movementDirection == ORTHOGONAL) break; // fall through if omni
        case DIAGONAL:
            // left-up and left-down
            target = cell - CELLS - 1;
            if (cell % CELLS != 0 && cellOnBoard(target) && !movingIntoSamePlayerPiece(cell, target, pieces)) moves[target] = 1;
            
            target = cell + CELLS - 1;
            if (cell % CELLS != 0 && cellOnBoard(target) && !movingIntoSamePlayerPiece(cell, target, pieces)) moves[target] = 1;
            
            // right-up and right-down
            target = cell - CELLS + 1;
            if ((cell + 1) % CELLS != 0 && cellOnBoard(target) && !movingIntoSamePlayerPiece(cell, target, pieces)) moves[target] = 1;
            
            target = cell + CELLS + 1;
            if ((cell + 1) % CELLS != 0 && cellOnBoard(target) && !movingIntoSamePlayerPiece(cell, target, pieces)) moves[target] = 1;
            
            break;
    }
}

// Updates
int movePiece(int from, int to, Piece pieces[TOTAL_CELLS], Ruleset ruleset) {
    Piece piece = pieces[from];
    PieceDef pieceDef = ruleset.pieceDefs[piece.pieceDef];
    
    int validMoves[TOTAL_CELLS] = { 0 };
    validMovesFor(pieceDef, from, validMoves, pieces);
    
    if (validMoves[to]) {
        pieces[from] = (Piece) {0};
        pieces[to] = piece;
        return 1;
    } else {
        return 0; // move not valid
    }
}


// Drawing

void drawGrid() {
    for (int i = BORDER; i <= BOARD_BOUNDARY; i += CELL_SIZE + 1) {
        DrawLine(i, BORDER, i, BOARD_BOUNDARY, SKYBLUE);
        DrawLine(BORDER, i, BOARD_BOUNDARY - 1, i, SKYBLUE);
    }
}

void drawArrow(Vector2 start, double angle, double length, Color color) {
    float x = (float) (length * cos(angle));
    float y = (float) (length * sin(angle));
    
    // Have to add start vector to computed result because we need to move the result vector to be near the start vector
    Vector2 end = { start.x + x, start.y + y };
    
    DrawLineEx(start, end, 5, color);
    float arrowheadAngle = 30 + (float) (RAD2DEG * angle);
    DrawPoly(end, 3, 10, arrowheadAngle, color);
}

void drawArrowV(Vector2 start, Vector2 end, Color color) {
    DrawLineEx(start, end, 5, color);
    
    double angle = atan2(end.y - start.y, end.x - start.x);
    float arrowheadAngle = 30 + (float) (RAD2DEG * angle);
    
    DrawPoly(end, 3, 10, arrowheadAngle, color);
}

Vector2 cellCenter(Rectangle cellRec) {
    return (Vector2) { cellRec.x + HALF_CELL_SIZE, cellRec.y + HALF_CELL_SIZE };
}

void drawMovementHint(Vector2 start, int moves[TOTAL_CELLS], Rectangle cellRecs[TOTAL_CELLS], int moveTo) {
    for (int c = 0; c < TOTAL_CELLS; c++) {
        Color color;
        if (moves[c]) {
            if (c == moveTo) {
                color = MAROON;
            } else {
                color = LIME;
            }
            drawArrowV(start, cellCenter(cellRecs[c]), color);
        }
    }
}

void drawValidMoves(Piece piece, int from, int target, Rectangle cellRecs[TOTAL_CELLS], Piece pieces[], Ruleset ruleset) {
    PieceDef pieceDef = ruleset.pieceDefs[piece.pieceDef];
    int validMoves[TOTAL_CELLS] = { 0 };
    validMovesFor(pieceDef, from, validMoves, pieces);
    
    Vector2 center = cellCenter(cellRecs[from]);
    drawMovementHint(center, validMoves, cellRecs, target);
}

void drawPiece(Piece piece, Vector2 center, Ruleset ruleset) {
    int sides = ruleset.pieceDefs[piece.pieceDef].sides;
    
    DrawPoly(center, sides, PIECE_RADIUS * 1.1, 180 * (piece.player), BLACK);
    DrawPoly(center, sides, PIECE_RADIUS, 180 * (piece.player), ruleset.colors[piece.player]);
}

void drawBoard(Rectangle cellRecs[TOTAL_CELLS], Piece pieces[TOTAL_CELLS], Ruleset ruleset, MouseState mouseState) {
    drawGrid();
  
    for (int cell = 0; cell < TOTAL_CELLS; cell++) {
        Piece piece = pieces[cell];
        
        Vector2 center = cellCenter(cellRecs[cell]);
        
        if (piece.present && cell != mouseState.selectedPiece) drawPiece(piece, center, ruleset);
        
        // Draw cell numbers
        // int length = snprintf(NULL, 0,"%d", cell) + 1;
        // char cell_str[length];
        // snprintf(cell_str, length, "%d", cell);
        // DrawText(cell_str, center.x, center.y, TEXT_SIZE, WHITE); 
    }
    
    // Draw mouseover piece so we can draw hints over the top of other pieces
    if (mouseState.cell != -1) {        
        if (mouseState.selectedPiece != -1) {
            Piece piece = pieces[mouseState.selectedPiece];
            
            drawValidMoves(piece, mouseState.selectedPiece, mouseState.cell, cellRecs, pieces, ruleset);
            
            drawPiece(piece, mouseState.position, ruleset);
        } else if (pieces[mouseState.cell].present) {
            Piece piece = pieces[mouseState.cell];
            
            drawValidMoves(piece, mouseState.cell, -1, cellRecs, pieces, ruleset);
            
            Vector2 center = cellCenter(cellRecs[mouseState.cell]);
            drawPiece(piece, center, ruleset);
        }
    }
}

void drawSidebar(char *seed_str, MouseState mouseState) {
    DrawRectangle(SIDEBAR_X, SIDEBAR_Y, SIDEBAR_WIDTH, SIDEBAR_HEIGHT, SKYBLUE);
    
    int y = SIDEBAR_INNER_Y;
        
    DrawFPS(SIDEBAR_INNER_X, y);
    y += SIDEBAR_LINE_HEIGHT;
    
    DrawText(seed_str, SIDEBAR_INNER_X, y, TEXT_SIZE, LIME); 
    y += SIDEBAR_LINE_HEIGHT;
    
    int length = snprintf(NULL, 0, "Selected: %d", mouseState.selectedPiece) + 1;
    char selectedPiece_str[length];
    snprintf(selectedPiece_str, length, "Selected: %d", mouseState.selectedPiece);
    
    DrawText(selectedPiece_str, SIDEBAR_INNER_X, y, TEXT_SIZE, LIME); 
}


int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "4mb game");
    
    // seed gen
    
    const int SEED = time(0) % 10000;
    // const int SEED = 6274;
    srand(SEED);
    
    int length = snprintf(NULL, 0, "SEED: %d", SEED) + 1;
    char seed_str[length];
    snprintf(seed_str, length, "SEED: %d", SEED);
    
    // Ruleset
    
    Ruleset ruleset = initRuleset();
    
    // Board

    Rectangle cellRecs[TOTAL_CELLS] = { 0 };     // Rectangles array

    initBoard(cellRecs);
    
    Piece pieces[TOTAL_CELLS] = {0};
    initPieces(ruleset, pieces);
    
    MouseState mouseState = { -1, -1, (Vector2) { 0.0f, 0.0f } };

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------

        mouseState.position = GetMousePosition();

        for (int i = 0; i < TOTAL_CELLS; i++) {
            if (CheckCollisionPointRec(mouseState.position, cellRecs[i])) {
                mouseState.cell = i;
                break;
            }
            mouseState.cell = -1;
        }
        
        if (pieces[mouseState.cell].present || mouseState.selectedPiece != -1) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        } else {
            SetMouseCursor(MOUSE_CURSOR_ARROW);
        }
        
        
        if (mouseState.selectedPiece == -1 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && pieces[mouseState.cell].present) {
            mouseState.selectedPiece = mouseState.cell;
        } else if (mouseState.selectedPiece != -1 && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            movePiece(mouseState.selectedPiece, mouseState.cell, pieces, ruleset);
            mouseState.selectedPiece = -1;
        }
        

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(DARKBLUE);

            drawBoard(cellRecs, pieces, ruleset, mouseState);
            
            drawSidebar(seed_str, mouseState);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}