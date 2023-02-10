/**
 * This is essentially the Traveling Salesman Problem, which is NP-Hard.
 * The complexity of checking every possible road is O(n!) because we have
 * that many permutations of the cities. The only way I am aware of to speed
 * this up is formulating the problem as a MIP problem and run it on CPlex, but
 * that's no fun.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileutils.h"

#define MAX_LINE_LENGTH 128
#define MAX_CITIES 128
#define STARTING_CAPACITY 8

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

/// @brief Next lexicographic permutation by Dijkstra from "A Discipline of Programming".
/// @return 0 if the array was permutated, -1 if there is no next permutation.
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
    swap(&array[i - 1], &array[j - 1]);

    i++;
    j = length;
    while (i < j) {
        swap(&array[i - 1], &array[j - 1]);
        i++;
        j--;
    }

    return 0;
}

int path_cost(const int *permutation, const int cities, const int *distances, const int max_cities) {
    int cost = 0;
    for (int i = 0; i < cities - 1; i++)
        cost += distances[permutation[i] * max_cities + permutation[i + 1]];
    return cost;
}

// --- String set ---

// Struct keeping a set of unique strings.
// No particular hashing is done, this is not for performance but for
// comfort later.
typedef struct {
    char **strings;
    int capacity;
    int size;
} StringSet;

void StringSet_new(StringSet *set) {
    set->strings = (char **)malloc(STARTING_CAPACITY * sizeof(char *));
    set->capacity = STARTING_CAPACITY;
    set->size = 0;
}

void StringSet_free(StringSet *set) {
    for (int i = 0; i < set->size; i++)
        free(set->strings[i]);
    free(set->strings);
    set->capacity = -1;
    set->size = -1;
}

/**
 * @brief Add a string, makes a copy. Does not add if already in the set.
 * Only pass 0-terminated strings plz.
 * @return index at which the string is (either added or already present).
 */
int StringSet_add(StringSet *set, const char *string) {
    // Only add if not already present.
    for (int i = 0; i < set->size; i++)
        if (!strcmp(set->strings[i], string))
            return i;

    // Not present already, add it.
    // Resize if needed.
    if (set->size == set->capacity) {
        set->strings = (char **)realloc(set->strings, sizeof(char *) * set->capacity * 2);
        set->capacity *= 2;
    }

    // Copy the string.
    int len = strlen(string);
    set->strings[set->size] = (char *)malloc(len + 1);
    set->strings[set->size][len] = 0;
    memcpy(set->strings[set->size], string, len);
    set->size++;

    return set->size - 1;
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
    StringSet_new(&cities);

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
        int i = StringSet_add(&cities, start);
        int j = StringSet_add(&cities, destination);
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
    StringSet_free(&cities);

    return 0;
}