#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "min_max.h"
#include "timer.h"

/// @brief Compute min and max coordinates, starting from 0, 0.
void bounds(
    const char *moves, const char *end, const int stride, int *min_x_ptr, int *max_x_ptr, int *min_y_ptr, int *max_y_ptr
) {
    int min_x = 0;
    int max_x = 0;
    int min_y = 0;
    int max_y = 0;
    int x = 0, y = 0;

    for (int i = 0; moves + i < end; i += stride) {
        switch (moves[i]) {
        case '>':
            max_x = maxi(max_x, ++x);
            break;
        case 'v':
            max_y = maxi(max_y, ++y);
            break;
        case '<':
            min_x = mini(min_x, --x);
            break;
        case '^':
            min_y = mini(min_y, --y);
            break;
        }
    }

    *min_x_ptr = min_x;
    *max_x_ptr = max_x;
    *min_y_ptr = min_y;
    *max_y_ptr = max_y;
}

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

void solution_one(const char *moves, const int n_moves, int *vis_one, int *vis_two) {
    // Very simple solution, just make a 2D grid for the houses.
    // First we have to figure out the sizes of the grid.
    // We assume to start at 0,0. Worst case is something similar to:
    // go up for half the moves, then go right. Total memory used is thus
    // O(n^2).
    int min_x, max_x, min_y, max_y;
    bounds(moves, moves + n_moves, 1, &min_x, &max_x, &min_y, &max_y);

    // Size of the grid, and starting cell.
    int width = max_x - min_x + 1;
    int height = max_y - min_y + 1;
    int x = -min_x;
    int y = -min_y;

    char *houses = (char *)malloc(width * height);
    memset(houses, 0, width * height);

    // Year one, visit all houses with Santa.
    *vis_one = visit(moves, moves + n_moves, 1, houses, width, &x, &y);

    // The next year, it is not as simple, because Santa and Robo-Santa could
    // visit entirely different houses. So I must compute the bounds twice to
    // find the actual ones.
    int min_x_s, max_x_s, min_y_s, max_y_s;
    int min_x_r, max_x_r, min_y_r, max_y_r;
    bounds(moves, moves + n_moves, 2, &min_x_s, &max_x_s, &min_y_s, &max_y_s);
    bounds(moves + 1, moves + n_moves, 2, &min_x_r, &max_x_r, &min_y_r, &max_y_r);

    // Actual min-max bounds.
    min_x = mini(min_x_s, min_x_r);
    max_x = maxi(max_x_s, max_x_r);
    min_y = mini(min_y_s, min_y_r);
    max_y = maxi(max_y_s, max_y_r);

    width = max_x - min_x + 1;
    height = max_y - min_y + 1;

    char *houses_new = (char *)malloc(width * height);
    memset(houses_new, 0, width * height);

    // Even moves with Santa.
    x = -min_x;
    y = -min_y;
    *vis_two = visit(moves, moves + n_moves, 2, houses_new, width, &x, &y);

    // Odd moves with Robo-Santa.
    x = -min_x;
    y = -min_y;
    *vis_two += visit(moves + 1, moves + n_moves, 2, houses_new, width, &x, &y);

    free(houses);
    free(houses_new);
}

void solution_two(const char *moves, const int n_moves, int *vis_one, int *vis_two) {
    // Solution involving an insertion sort to find which house has been or
    // has not been visited. This allows to avoid allocating the whole grid.
    // While we do not know the index of each house, we can sort the houses
    // by y and then by x. This uses O(n) memory in the worst case instead
    // of O(n^2), but each house's visit check is O(n).
}

void solution_three(const char *moves, const int n_moves, int *vis_one, int *vis_two) {
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
    int visited_one_one = 0, visited_one_two = 0;
    int visited_two_one = 0, visited_two_two = 0;
    int visited_three_one = 0, visited_three_two = 0;
    double time_one = 0, time_two = 0, time_three = 0;
    TIME_MS(solution_one(content, size, &visited_one_one, &visited_one_two), time_one);
    TIME_MS(solution_one(content, size, &visited_two_one, &visited_two_two), time_two);
    TIME_MS(solution_one(content, size, &visited_three_one, &visited_three_two), time_three);

    // Print results.
    printf("Solution 1 says:\n\tSanta: %d\n\tSanta & Robo-Santa: %d\n\n", visited_one_one, visited_one_two);
    printf("Solution 2 says:\n\tSanta: %d\n\tSanta & Robo-Santa: %d\n\n", visited_two_one, visited_two_two);
    printf("Solution 3 says:\n\tSanta: %d\n\tSanta & Robo-Santa: %d\n\n", visited_three_one, visited_three_two);
    printf("%f for solution one.\n", time_one);
    printf("%f for solution one.\n", time_two);
    printf("%f for solution one.\n", time_three);

    free(content);

    return 0;
}