// Utility
#define ARR_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )

int * choose(int * chosen, int n, int max);

#define ROTATE(position) ( TOTAL_CELLS - position - 1 )