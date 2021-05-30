// Board
#define CELLS 6
#define TOTAL_CELLS (CELLS * CELLS)
#define HOME_CELLS (CELLS * 2);
#define CELL_SIZE 100
#define HALF_CELL_SIZE (CELL_SIZE / 2)
#define BOARD_SIZE ((CELLS * CELL_SIZE) + (CELLS + 1))
#define BOARD_BOUNDARY (BOARD_SIZE + BORDER)
#define BORDER 20

// Pieces
#define N_PIECE_DEFS 3 // at some point this should be dynamic
#define PIECE_RADIUS ((0.8 * CELL_SIZE) - HALF_CELL_SIZE)

// Sidebar
#define SIDEBAR_WIDTH 200
#define SIDEBAR_HEIGHT BOARD_SIZE
#define SIDEBAR_X (BOARD_BOUNDARY + BORDER)
#define SIDEBAR_Y BORDER
#define PADDING 10
#define TEXT_SIZE 20
#define SIDEBAR_INNER_X (SIDEBAR_X + PADDING)
#define SIDEBAR_INNER_Y (SIDEBAR_Y + PADDING)
#define SIDEBAR_LINE_HEIGHT 40

// Window
#define WINDOW_WIDTH (BOARD_SIZE + BORDER*3 + SIDEBAR_WIDTH)
#define WINDOW_HEIGHT (BOARD_SIZE + BORDER*2)

// Utility
#define ARR_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )


// Structs
typedef struct Piece {
    int position;
    int player;
    int pieceDef; // Index into array of piece definitons in Ruleset
} Piece;

typedef struct PieceDef {
    int sides; // polygon sides
} PieceDef;

typedef struct Ruleset {
    int numberOfPieceDefs; 
    int numberOfPieces; // all pieces across both players
    Color colors[2]; // player colours
    PieceDef pieceDefs[N_PIECE_DEFS];
} Ruleset;