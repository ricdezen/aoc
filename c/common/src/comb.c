#include "comb.h"
#include "misc.h"

int next_permutation(int *array, int length) {
    int i = length - 1;
    while (array[i - 1] >= array[i]) {
        i--;
        // No more elements, no more permutations.
        if (i < 0)
            return -1;
    }

    int j = length;
    while (array[j - 1] <= array[i - 1])
        j--;

    // Swap.
    swapi(&array[i - 1], &array[j - 1]);

    i++;
    j = length;
    while (i < j) {
        swapi(&array[i - 1], &array[j - 1]);
        i++;
        j--;
    }

    return 0;
}