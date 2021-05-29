typedef struct Ruleset {
    int numberOfPieces; // per player - symmetric game
    Color colors[2]; // player colours
} Ruleset;

typedef struct Piece {
    int player;
    Color color;
} Piece;

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