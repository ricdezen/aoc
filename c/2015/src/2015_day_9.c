#define EXPECTED_1 141
#define EXPECTED_2 736

/**
 * This is essentially the Traveling Salesman Problem, which is NP-Hard.
 * The complexity of checking every possible road is O(n! * n) because we have
 * that many permutations of the cities. The only way I am aware of to speed
 * this up is formulating the problem as a MIP problem and run it on CPlex, but
 * that's no fun.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comb.h"
#include "fileutils.h"
#include "misc.h"
#include "stringset.h"

#define MAX_LINE_LENGTH 128
#define MAX_CITIES 128
#define STARTING_CAPACITY 8

int path_cost(const int *permutation, const int cities, const int *distances, const int max_cities) {
    int cost = 0;
    for (int i = 0; i < cities - 1; i++)
        cost += distances[permutation[i] * max_cities + permutation[i + 1]];
    return cost;
}

// --- Main ---

int min_array(int *a, int len) {
    int min = a[0];
    for (int i = 1; i < len; i++)
        if (a[i] < min)
            min = a[i];
    return min;
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_9.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    // Set of the cities.
    StringSet cities;
    StringSet_init(&cities);

    // Triangular matrix of distances.
    int distances[MAX_CITIES * MAX_CITIES] = {0};

    // Read the next line.
    char line[MAX_LINE_LENGTH] = {0};
    int len = freadline(input, line, MAX_LINE_LENGTH);

    while (len != 0) {
        // Parse line.
        char *start = strtok(line, " ");
        strtok(NULL, " "); // "to"
        char *destination = strtok(NULL, " ");
        strtok(NULL, " "); // "="
        char *dist = strtok(NULL, " ");

        // Add cities.
        int i = StringSet_add_ptr(&cities, start);
        int j = StringSet_add_ptr(&cities, destination);
        int d = atoi(dist);
        distances[i * MAX_CITIES + j] = d;
        distances[j * MAX_CITIES + i] = d;

        // Next line.
        len = freadline(input, line, MAX_LINE_LENGTH);
    }

    // Find all permutations of the indices. These are all the possible paths.
    int permutation[cities.size];
    for (int i = 0; i < cities.size; i++)
        permutation[i] = i;

    // Find min and max path by enumerating the permutations.
    int min_dist = path_cost(permutation, cities.size, distances, MAX_CITIES);
    int max_dist = 0;
    while (next_permutation(permutation, cities.size) == 0) {
        int dist = path_cost(permutation, cities.size, distances, MAX_CITIES);
        if (dist < min_dist)
            min_dist = dist;
        if (dist > max_dist)
            max_dist = dist;
    }

    // Print result.
    printf("Shortest path: %d\n", min_dist);
    printf("Longest path: %d\n", max_dist);

    fclose(input);
    StringSet_destroy(&cities);

    return check_result_i(min_dist, EXPECTED_1) | check_result_i(max_dist, EXPECTED_2);
}