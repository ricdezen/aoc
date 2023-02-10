#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileutils.h"
#include "min_max.h"
#include "mystring.h"
#include "timer.h"

/// @brief Apply a move
///
/// WARNING: _x and _y appear more than once, do NOT put side effect expressions here.
///
/// @param _x Variable storing x pos.
/// @param _y Variable storing y pos.
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

// --- Solution one ---

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

// --- Solution two ---

typedef struct {
    int x;
    int y;
} House;

int house_less_than(House h1, House h2) {
    // Earlier row: true.
    if (h1.y < h2.y)
        return 1;

    // Same row but earlier x: true.
    if (h1.y == h2.y && h1.x < h2.x)
        return 1;

    return 0;
}

int house_equals(House h1, House h2) { return h1.x == h2.x && h1.y == h2.y; }

int sort_visit(const char *moves, const char *end, const int stride, House *houses, int *n_houses) {
    // If no houses in houses, initialize it.
    if (*n_houses == 0) {
        houses[0] = (House){0, 0};
        *n_houses = 1;
    }

    // We still start at 0, 0.
    int x = 0;
    int y = 0;

    for (int i = 0; moves + i < end; i += stride) {
        // Apply move.
        MOVE(moves[i], x, y);

        // Insert the house.
        houses[*n_houses] = (House){x, y};

        // Insertion sort iteration, keep the array sorted.
        for (int i = *n_houses - 1; i >= 0; i--) {
            // Right house lower than left house -> swap.
            if (house_less_than(houses[i + 1], houses[i])) {
                House temp = houses[i];
                houses[i] = houses[i + 1];
                houses[i + 1] = temp;
                continue;
            }

            // Just break because we are done swapping.
            break;
        }

        (*n_houses)++;
    }

    // Count houses skipping duplicates.
    int visited = 1;
    for (int i = 1; i < *n_houses; i++)
        if (!house_equals(houses[i], houses[i - 1]))
            visited++;

    return visited;
}

void solution_two(const char *moves, const int n_moves, int *vis_one, int *vis_two) {
    // Solution involving an insertion sort to find which house has been or
    // has not been visited. This allows to avoid allocating the whole grid.
    // While we do not know the index of each house, we can sort the houses
    // by y and then by x. This uses O(n) memory in the worst case instead
    // of O(n^2), but each house's visit check is O(n).
    int n_houses = 0;
    House *houses = (House *)malloc(sizeof(House) * (n_moves + 1));
    *vis_one = sort_visit(moves, moves + n_moves, 1, houses, &n_houses);

    // Next year, even moves for Santa, odd moves for Robo-Santa.
    n_houses = 0;
    House *new_houses = (House *)malloc(sizeof(House) * (n_moves + 1) * 2);
    int _ = sort_visit(moves, moves + n_moves, 2, new_houses, &n_houses);
    *vis_two = sort_visit(moves + 1, moves + n_moves, 2, new_houses, &n_houses);

    free(houses);
    free(new_houses);
}

// --- Solution three ---

void solution_three(const char *moves, const int n_moves, int *vis_one, int *vis_two) {
    // Solution that keeps the visited houses in a simple hashtable/hashset.
    // This allows for amortized constant time visit check, but still O(n) mem.
    // I do not need to implement this to know that it will be much slower than
    // solution one, although with less space occupied. This method, given a
    // proper hashtable implementation, would be the way to go. I do not find
    // implementing a full hashtable useful for the purpose of this exercise.
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_3.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    // Read file into a string.
    String *content = ftostring(input);

    // Close file.
    fclose(input);

    // Measure time it takes for each solution.
    int visited_one_one = 0, visited_one_two = 0;
    int visited_two_one = 0, visited_two_two = 0;
    double time_one = 0, time_two = 0;
    TIME_MS(solution_one(content->raw, content->length, &visited_one_one, &visited_one_two), time_one);
    TIME_MS(solution_two(content->raw, content->length, &visited_two_one, &visited_two_two), time_two);

    // Print results.
    printf("Solution 1 says:\n\tSanta: %d\n\tSanta & Robo-Santa: %d\n\n", visited_one_one, visited_one_two);
    printf("Solution 2 says:\n\tSanta: %d\n\tSanta & Robo-Santa: %d\n\n", visited_two_one, visited_two_two);
    printf("%f for solution one.\n", time_one);
    printf("%f for solution two.\n", time_two);

    String_free(content);

    return 0;
}