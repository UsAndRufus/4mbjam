#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
// maybe shouldn't include this
#include <string.h>

#include "raylib.h"
#include "utility.h"
#include "defs.h"

// init and generation

void generatePieceDefs(Ruleset *ruleset) {
    int chosenSides[ruleset->numberOfPieceDefs];
    choose(chosenSides, ruleset->numberOfPieceDefs, N_PIECE_DEFS);
    
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

void generateRules(Ruleset *ruleset) {
    Rule rule = { 0 };
    rule.appliesToCount = rand() % ruleset->numberOfPieceDefs + 1;
    
    choose(rule.appliesTo, rule.appliesToCount, ruleset->numberOfPieceDefs);
    
    rule.condition = (Condition) {
        PIECE_ON_CELL_TYPE,
        STONE
    };
    
    rule.effectsCount = 2;
    Effect effects[2] = { REMOVE_PIECE, ADD_POINT };
    memcpy(rule.effects, effects, sizeof(effects)); // probably bad use of this, idk
    
   
    ruleset->rule = rule;
}

void generateCellTypes(Ruleset *ruleset) {
    for (int i = 0; i < TOTAL_CELLS; i++) {
        ruleset->cellTypes[i] = NONE;
    }
    
    int positions[2];
    choose(positions, 2, HOME_CELLS);
    
    int position;
    for (int i = 0; i < 2; i++) {
        position = positions[i] + HOME_CELLS;
        if (i % 2 == 0) {
            ruleset->cellTypes[position] = LAVA;
            ruleset->cellTypes[ROTATE(position)] = LAVA;
        } else {
            ruleset->cellTypes[position] = STONE;
            ruleset->cellTypes[ROTATE(position)] = STONE;
        }
    }
}

Ruleset generateRuleset(int seed) {
    static Color playerColors[] = {VIOLET, MAROON, DARKGREEN, PINK, PURPLE, BEIGE};
    
    Ruleset ruleset = {
        seed,
        N_PIECE_DEFS,
        N_PIECE_DEFS * 2, // total number of pieces
        {VIOLET, MAROON},
        {}
    };
    
    int colorIds[2];
    choose(colorIds, 2, ARR_SIZE(playerColors));
    
    ruleset.playerColors[0] = playerColors[colorIds[0]];
    ruleset.playerColors[1] = playerColors[colorIds[1]];
       
    generatePieceDefs(&ruleset);
    generateRules(&ruleset);
    generateCellTypes(&ruleset);
    
    
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
                position = ROTATE(position);
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


// Assume we have max 8 valid moves
// Doesn't care about turn - just shows possible moves for piece on owner's turn
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
Move movePiece(int from, int to, Piece pieces[TOTAL_CELLS], Ruleset ruleset, Turn turn) {
    Piece piece = pieces[from];
    PieceDef pieceDef = ruleset.pieceDefs[piece.pieceDef];
    
    int validMoves[TOTAL_CELLS] = { 0 };
    validMovesFor(pieceDef, from, validMoves, pieces);
    
    if (validMoves[to] && turn.player == piece.player) {
        Move result = pieces[to].present ? CAPTURE : MOVE;
        pieces[from] = (Piece) {0};
        pieces[to] = piece;
        return result;
    } else {
        return NONE; // move not valid
    }
}

void nextTurn(Turn * turn) {
    turn->player = turn->count % 2; // This seems like the wrong order to do it in, but we start at Turn 1, but players are represented as 0 & 1
    turn->count = turn->count + 1;
}


// Drawing

void drawGrid(Rectangle cellRecs[TOTAL_CELLS], Ruleset ruleset) {
    
    
    for (int i = 0; i <= TOTAL_CELLS; i++) {
        switch(ruleset.cellTypes[i]) {
            case STONE:
                DrawRectangleRec(cellRecs[i], GRAY);
                break;
            case LAVA:
                DrawRectangleRec(cellRecs[i], RED);
                break;
            default:
                break;
        }   
    }
    
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

void drawMovementHint(Vector2 start, int moves[TOTAL_CELLS], Rectangle cellRecs[TOTAL_CELLS], int moveTo, int available) {
    for (int c = 0; c < TOTAL_CELLS; c++) {
        Color color;
        if (moves[c]) {
            if (!available) {
                color = SKYBLUE;
            } else if (c == moveTo) {
                color = MAROON;
            } else {
                color = LIME;
            }
            drawArrowV(start, cellCenter(cellRecs[c]), color);
        }
    }
}

void drawValidMoves(Piece piece, int from, int target, Rectangle cellRecs[TOTAL_CELLS], Piece pieces[], Ruleset ruleset, Turn turn) {
    PieceDef pieceDef = ruleset.pieceDefs[piece.pieceDef];
    int validMoves[TOTAL_CELLS] = { 0 };
    validMovesFor(pieceDef, from, validMoves, pieces);
    
    Vector2 center = cellCenter(cellRecs[from]);
    int belongsToCurrentPlayer = turn.player == piece.player;
    drawMovementHint(center, validMoves, cellRecs, target, belongsToCurrentPlayer);
}

void drawPieceDef(PieceDef pieceDef, Vector2 center, int radius, int angle, Color color) {
    int sides = pieceDef.sides;
    
    DrawPoly(center, sides, radius * 1.1, angle, BLACK);
    DrawPoly(center, sides, radius, angle, color);
}

void drawPiece(Piece piece, Vector2 center, Ruleset ruleset) {
    PieceDef pieceDef = ruleset.pieceDefs[piece.pieceDef];
    int angle = 180 * (piece.player);
    Color color = ruleset.playerColors[piece.player];
    
    drawPieceDef(pieceDef, center, PIECE_RADIUS, angle, color);
}

void drawBoard(Rectangle cellRecs[TOTAL_CELLS], Piece pieces[TOTAL_CELLS], Ruleset ruleset, MouseState mouseState, Turn turn) {
    drawGrid(cellRecs, ruleset);
  
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
            
            drawValidMoves(piece, mouseState.selectedPiece, mouseState.cell, cellRecs, pieces, ruleset, turn);
            
            drawPiece(piece, mouseState.position, ruleset);
        } else if (pieces[mouseState.cell].present) {
            Piece piece = pieces[mouseState.cell];
            
            drawValidMoves(piece, mouseState.cell, -1, cellRecs, pieces, ruleset, turn);
            
            Vector2 center = cellCenter(cellRecs[mouseState.cell]);
            drawPiece(piece, center, ruleset);
        }
    }
}

int drawSidebarString(char * string, int n, Color color, int y) {
    int length = snprintf(NULL, 0, string, n) + 1;
    char str[length];
    snprintf(str, length, string, n);
    
    DrawText(str, SIDEBAR_INNER_X, y, TEXT_SIZE, color); 
    return y + SIDEBAR_LINE_HEIGHT;
}

int explainRule(Rule rule, Ruleset ruleset, Color color, int y) {
    DrawText("RULE 1", SIDEBAR_INNER_X, y, TEXT_SIZE, color); 
    y += SIDEBAR_LINE_HEIGHT;
    
    DrawText("When ", SIDEBAR_INNER_X, y, TEXT_SIZE, color); 
    
    int radius = 10;
    int x = SIDEBAR_INNER_X + MeasureText("When ", TEXT_SIZE) + radius;
    Vector2 center = { x, y + radius };
    for (int i = 0; i < rule.appliesToCount; i++) {
        PieceDef pieceDef = ruleset.pieceDefs[rule.appliesTo[i]];
        drawPieceDef(pieceDef, center, radius, 180, BEIGE);
        center.x += radius * 3;
    }
    
    y += SIDEBAR_LINE_HEIGHT;
    
    DrawText("is ", SIDEBAR_INNER_X, y, TEXT_SIZE, color);
    x = SIDEBAR_INNER_X + MeasureText("is ", TEXT_SIZE);
    
    switch(rule.condition.condition) {
        case PIECE_ON_CELL_TYPE:
            switch(rule.condition.appliesOn) {
                case STONE:
                    DrawText("on stone cell:", x, y, TEXT_SIZE, color);
                    break;
                case LAVA:
                    DrawText("on lava cell:", x, y, TEXT_SIZE, color);
                    break;
            }
            break;
        default:
            DrawText("BAD CONDITION", x, y, TEXT_SIZE, color);
    }
    
    y += SIDEBAR_LINE_HEIGHT;
    
    
    for (int i = 0; i < rule.effectsCount; i++) {
        char * effect;
        switch (rule.effects[i]) {
            case REMOVE_PIECE:
                effect = " - Remove piece";
                break;
            case ADD_POINT:
                effect = " - Add point";
                break;
            case REMOVE_POINT:
                effect = " - Lose point";
                break;
            default:
                effect = "BAD EFFECT";
        }
        
        DrawText(effect, x, y, TEXT_SIZE, color);
        y += SIDEBAR_LINE_HEIGHT;
    }
    
    return y;
    
}

int drawRules(Ruleset ruleset, Color color, int y) {
    DrawText("RULES", SIDEBAR_INNER_X, y, TEXT_SIZE, color); 
    y += SIDEBAR_LINE_HEIGHT;
    
    y = explainRule(ruleset.rule, ruleset, color, y);
    
    return y;
}

void drawSidebar(Ruleset ruleset, int score[2], Turn turn) {
    DrawRectangle(SIDEBAR_X, SIDEBAR_Y, SIDEBAR_WIDTH, SIDEBAR_HEIGHT, SKYBLUE);
    
    int y = SIDEBAR_INNER_Y;
        
    DrawFPS(SIDEBAR_INNER_X, y);
    y += SIDEBAR_LINE_HEIGHT;
    
    y = drawSidebarString("SEED: %d", ruleset.seed, LIME, y);
    
    y = drawSidebarString("Turn %d", turn.count, LIME, y);
    
    // Highlight whose turn it is
    DrawRectangle(SIDEBAR_X, y + (turn.player % 2) * SIDEBAR_LINE_HEIGHT, SIDEBAR_WIDTH, SIDEBAR_LINE_HEIGHT, LIME);
    y = drawSidebarString("Player 1: %d", score[0], ruleset.playerColors[0], y);
    y = drawSidebarString("Player 2: %d", score[1], ruleset.playerColors[1], y);
    
    y = drawRules(ruleset, LIME, y);
}

Piece mouseover(MouseState * mouseState, Rectangle * cellRecs, Piece * pieces, Turn turn) {
    for (int i = 0; i < TOTAL_CELLS; i++) {
        if (CheckCollisionPointRec(mouseState->position, cellRecs[i])) {
            mouseState->cell = i;
            break;
        }
        mouseState->cell = -1;
    }
    
    Piece mousePiece = pieces[mouseState->cell];
    
    if (mouseState->selectedPiece != -1 || (mousePiece.present && mousePiece.player == turn.player)) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else if (mousePiece.present && mousePiece.player != turn.player) {
        SetMouseCursor(MOUSE_CURSOR_NOT_ALLOWED);  
    } else {
        SetMouseCursor(MOUSE_CURSOR_ARROW);
    }
    
    return mousePiece;
}


int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "4mb game");
    
    // seed gen
    
    const int SEED = time(0) % 10000;
    // const int SEED = 6274;
    srand(SEED);
    
    // Ruleset
    
    Ruleset ruleset = generateRuleset(SEED);
    
    // Board

    Rectangle cellRecs[TOTAL_CELLS] = { 0 };     // Rectangles array

    initBoard(cellRecs);
    
    Piece pieces[TOTAL_CELLS] = { 0 };
    initPieces(ruleset, pieces);
    
    int score[2] = { 0 };
    
    Turn turn = { 1, 0 };
    
    MouseState mouseState = { -1, -1, (Vector2) { 0.0f, 0.0f } };
    
    Piece mousePiece;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------

        mouseState.position = GetMousePosition();

        
        mousePiece = mouseover(&mouseState, cellRecs, pieces, turn);
        
        // Piece move
        if (mouseState.selectedPiece == -1 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mousePiece.present && mousePiece.player == turn.player) {
            mouseState.selectedPiece = mouseState.cell;
        } else if (mouseState.selectedPiece != -1 && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            Move move = movePiece(mouseState.selectedPiece, mouseState.cell, pieces, ruleset, turn);
            
            switch(move) {
                case CAPTURE:
                    score[turn.player] += 1;
                case MOVE:
                    nextTurn(&turn);
                default:
                    break;
            }
            
            mouseState.selectedPiece = -1;
        }
        

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(DARKBLUE);

            drawBoard(cellRecs, pieces, ruleset, mouseState, turn);
            
            drawSidebar(ruleset, score, turn);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}