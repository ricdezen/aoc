#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIME(foo)                                                                                                      \
    { foo; }

int imax(int a, int b) { return (a > b) ? a : b; }

int imin(int a, int b) { return (a < b) ? a : b; }

/// @brief Visit houses.
/// @param last Byte after the last move.
/// @param stride How many moves to skip.
/// @param x_ptr Initial x, will be updated.
/// @param y_ptr Initial y, will be updated.
/// @return How many houses were not already visited.
int visit(const char *moves, const char *end, const int stride, char *houses, const int width, int *x_ptr, int *y_ptr) {
    // Visit houses and return how many were visited.
    // Modifies x, y and houses.
    int x = *x_ptr;
    int y = *y_ptr;

#define HOUSE(_x, _y) houses[_y * width + _x]
#define MOVE(_m, _x, _y)                                                                                               \
    switch (_m) {                                                                                                      \
    case '>':                                                                                                          \
        ++_x;                                                                                                          \
        break;                                                                                                         \
    case 'v':                                                                                                          \
        ++_y;                                                                                                          \
        break;                                                                                                         \
    case '<':                                                                                                          \
        --_x;                                                                                                          \
        break;                                                                                                         \
    case '^':                                                                                                          \
        --_y;                                                                                                          \
        break;                                                                                                         \
    }

    // Check if first house is already visited.
    int visited = HOUSE(x, y) ? 0 : 1;
    HOUSE(x, y) = 1;

    // Visit all houses.
    for (int i = 0; moves + i < end; i += stride) {
        // Apply move.
        MOVE(moves[i], x, y);

        // Check if house is new.
        if (HOUSE(x, y) == 0)
            visited++;

        // Visit house.
        HOUSE(x, y) = 1;
    }

#undef HOUSE
#undef MOVE

    *x_ptr = x;
    *y_ptr = y;

    return visited;
}

void solution_one(char *moves, int n_moves) {
    // Very simple solution, just make a 2D grid for the houses.
    // First we have to figure out the sizes of the grid.
    // We assume to start at 0,0. Worst case is something similar to:
    // go up for half the moves, then go right. Total memory used is thus
    // O(n^2).

    int min_x = 0;
    int max_x = 0;
    int min_y = 0;
    int max_y = 0;
    int x = 0, y = 0;

    for (int i = 0; i < n_moves; i++) {
        switch (moves[i]) {
        case '>':
            max_x = imax(max_x, ++x);
            break;
        case 'v':
            max_y = imax(max_y, ++y);
            break;
        case '<':
            min_x = imin(min_x, --x);
            break;
        case '^':
            min_y = imin(min_y, --y);
            break;
        }
    }

    // Size of the grid, and starting cell.
    int width = max_x - min_x + 1;
    int height = max_y - min_y + 1;
    x = -min_x;
    y = -min_y;

    char *houses = (char *)malloc(width * height);
    memset(houses, width * height, 0);

    // Year one, visit all houses with Santa.
    int visited_one = visit(moves, moves + n_moves, 1, houses, width, &x, &y);

    // Do the same next year with two Santas.
    memset(houses, 0, width * height);
    x = -min_x;
    y = -min_y;
    int rx = -min_x;
    int ry = -min_y;

    // Even moves with Santa.
    int visited_two = visit(moves, moves + n_moves, 2, houses, width, &x, &y);
    visited_two += visit(moves + 1, moves + n_moves, 2, houses, width, &rx, &ry);

    printf("%d houses received at least one present.\n", visited_one);
    printf("%d houses received at least one present the next year.\n", visited_two);

    free(houses);
}

void solution_two(char *moves, int n_moves) {
    // Solution involving an insertion sort to find which house has been or
    // has not been visited. This allows to avoid allocating the whole grid.
    // While we do not know the index of each house, we can sort the houses
    // by y and then by x. This uses O(n) memory in the worst case instead
    // of O(n^2), but each house's visit check is O(n).
}

void solution_three(char *moves, int n_moves) {
    // Solution that keeps the visited houses in a simple hashtable.
    // This allows for amortized constant time visit check, but still O(n) mem.
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_3.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    // Read file into a string.
    char *content = NULL;

    fseek(input, 0L, SEEK_END);
    int size = ftell(input);
    fseek(input, 0L, SEEK_SET);
    content = (char *)malloc(size + 1);
    content[size] = 0;
    size_t read = fread(content, 1, size, input);

    // Measure time it takes for each solution.
    TIME(solution_one(content, size));

    free(content);

    return 0;
}