#include <stdlib.h>

// Choose n integers from the range 0..max, ensuring they are unique. 
// chosen is the array to assign the chosen ints to
void choose(int * chosen, int n, int max) {
    int *used;
    used = (int*)calloc(max, sizeof(int));;
    
    int num;
    for (int i = 0; i < n; i++) {
        // Ensure we don't have a duplicate
        for (;;) {
            num = rand() % max;
            if (!used[num]) {
                break;
            }
        }
        chosen[i] = num;
        used[num] = 1;
    }
    
    free(used);
}