/**
 * TSP again, like day 9. Well, it is not technically the TSP, but the reasoning
 * is the same.
 */
#define EXPECTED_1 709
#define EXPECTED_2 668

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comb.h"
#include "fileutils.h"
#include "misc.h"
#include "stringset.h"

#define MAX_LINE_LENGTH 1024
#define MAX_PEOPLE 64

// --- Part one ---

// Cost of a single person (check who is both left and right).
int person_cost(const int *scores, const int max_people, const int i, const int left, const int right) {
    return scores[i * max_people + left] + scores[i * max_people + right];
}

int path_cost(const int *perm, const int people, const int *scores, const int max_people) {
    int cost = 0;
    // Notice we do all people, because we have both the
    // costs of Alice near Bob and Bob near Alice.
    for (int i = 0; i < people; i++) {
        // We need to consider both the person to the left and to the right.
        int left = ((i - 1) + people) % people;
        int right = (i + 1) % people;
        cost += person_cost(scores, max_people, perm[i], perm[left], perm[right]);
    }
    return cost;
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_13.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    // Names of the people at the table.
    StringSet *people = StringSet_new();

    // Preference scores (distances, weights of the edges).
    int distances[MAX_PEOPLE * MAX_PEOPLE] = {0};

    // Temporary storage for the line.
    char line[MAX_LINE_LENGTH] = {0};
    int len = 0;

    while (len = freadline(input, line, sizeof(line))) {
        // First word: name of the starting node of an edge.
        char *start = strtok(line, " ");
        strtok(NULL, " ");
        char *sign = strtok(NULL, " ");
        char *score = strtok(NULL, " ");

        // Skip useless words.
        for (int i = 0; i < 6; i++)
            strtok(NULL, " ");

        // Last word - last char: name of the ending node.
        char *dest = strtok(NULL, " ");
        dest[strlen(dest) - 1] = 0; // Remove final dot.

        // Get numerical values based on the strings above.
        int i = StringSet_add_ptr(people, start);
        int j = StringSet_add_ptr(people, dest);
        int dist = (!strncmp("gain", sign, 5) ? 1 : -1) * atoi(score);

        // Put it in the matrix. Notice problem is asymmetric, so
        // we set distances[i][j] but not distances[j][i].
        distances[i * MAX_PEOPLE + j] = dist;
    }

    // Close file.
    fclose(input);

    // Find all permutations of the indices. These are all the possible paths.
    int permutation[people->size];
    for (int i = 0; i < people->size; i++)
        permutation[i] = i;

    // Find min and max path by enumerating the permutations.
    int max_dist = path_cost(permutation, people->size, distances, MAX_PEOPLE);
    while (next_permutation(permutation, people->size) == 0) {
        int dist = path_cost(permutation, people->size, distances, MAX_PEOPLE);
        if (dist > max_dist)
            max_dist = dist;
    }

    // I misread this initially, and understood to add ourselves to the previous
    // arrangement in the best possible way. Nope, gotta find another permutation.
    // I just have to do it all with size increased by 1, cause the distance matrix
    // is already 0 everywhere.
    int permutation_two[people->size + 1];
    for (int i = 0; i < people->size + 1; i++)
        permutation_two[i] = i;

    // Find min and max path by enumerating the permutations.
    int max_dist_two = path_cost(permutation_two, people->size + 1, distances, MAX_PEOPLE);
    while (next_permutation(permutation_two, people->size + 1) == 0) {
        int dist = path_cost(permutation_two, people->size + 1, distances, MAX_PEOPLE);
        if (dist > max_dist_two)
            max_dist_two = dist;
    }

    StringSet_free(people);

    return check_result_i(max_dist, EXPECTED_1) | check_result_i(max_dist_two, EXPECTED_2);
}