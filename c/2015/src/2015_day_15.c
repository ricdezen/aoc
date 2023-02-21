#define EXPECTED_1 21367368
#define EXPECTED_2 1766400

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileutils.h"
#include "list.h"
#include "min_max.h"
#include "misc.h"
#include "stringset.h"

#define MAX_LINE_LENGTH 1024
#define SUM_OF_INGREDIENTS 100
#define N_PROPERTIES 5
#define CALORIES_PART_TWO 500

typedef struct {
    int properties[N_PROPERTIES];
} Ingredient;

int eval_recursive(const Ingredient *ingredients, const int n_ingr, const int left, const int i, int *taken) {
    if (i < n_ingr - 1) {
        // Recursive call. Fixate the i-th ingredient at some value.
        // The rest of the ingredients have 100 - value spoons to fill.
        int max = 0;
        for (int s = 0; s <= left; s++) {
            taken[i] = s;
            max = maxi(max, eval_recursive(ingredients, n_ingr, left - s, i + 1, taken));
        }
        return max;
    } else {
        // Base case, this is the only free ingredient. It must have a specific
        // value, and we can evaluate and compare.
        taken[i] = left;

        // Evaluate score.
        int score = 1;

        // Notice we exclude calories.
        for (int p = 0; p < N_PROPERTIES - 1; p++) {
            int prop = 0;
            for (int x = 0; x < n_ingr; x++)
                prop += ingredients[x].properties[p] * taken[x];
            score *= maxi(0, prop);
        }

        // Return the score.
        return score;
    }
}

int eval_combinations(const Ingredient *ingredients, const int n_ingr, const int total) {
    // For each combination of ingredients, find its score, and if it is better than the maximum, update it.
    int taken[n_ingr];
    return eval_recursive(ingredients, n_ingr, total, 0, taken);
}

// --- Part two ---

int eval_recursive_two(const Ingredient *ingredients, const int n_ingr, const int left, const int i, int *taken) {
    if (i < n_ingr - 1) {
        // Recursive call. Fixate the i-th ingredient at some value.
        // The rest of the ingredients have 100 - value spoons to fill.
        int max = 0;
        for (int s = 0; s <= left; s++) {
            taken[i] = s;
            max = maxi(max, eval_recursive_two(ingredients, n_ingr, left - s, i + 1, taken));
        }
        return max;
    } else {
        // Base case, this is the only free ingredient. It must have a specific
        // value, and we can evaluate and compare.
        taken[i] = left;

        // If calories are not 500, do not evaluate.
        int calories = 0;
        for (int x = 0; x < n_ingr; x++)
            calories += ingredients[x].properties[N_PROPERTIES - 1] * taken[x];
        if (calories != CALORIES_PART_TWO)
            return 0;

        // Evaluate score.
        int score = 1;

        // Notice we exclude calories.
        for (int p = 0; p < N_PROPERTIES - 1; p++) {
            int prop = 0;
            for (int x = 0; x < n_ingr; x++)
                prop += ingredients[x].properties[p] * taken[x];
            score *= maxi(0, prop);
        }

        // Return the score.
        return score;
    }
}

int eval_combinations_two(const Ingredient *ingredients, const int n_ingr, const int total) {
    // For each combination of ingredients, find its score, and if it is better than the maximum, update it.
    int taken[n_ingr];
    return eval_recursive_two(ingredients, n_ingr, total, 0, taken);
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_15.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    // Ingredient names and their stats.
    IntList *properties[N_PROPERTIES] = {0};
    for (int i = 0; i < N_PROPERTIES; i++)
        properties[i] = IntList_new();

    // Temporary storage for the line.
    char line[MAX_LINE_LENGTH] = {0};
    int len = 0;

    while (len = freadline(input, line, sizeof(line))) {
        // Name of the ingredient.
        char *name = strtok(line, ":");

        // Properties.
        for (int i = 0; i < N_PROPERTIES; i++) {
            strtok(NULL, " ");
            char *value = strtok(NULL, " \n");
            if (i < N_PROPERTIES - 1)
                value[strlen(value) - 1] = 0;
            IntList_push(properties[i], atoi(value));
        }
    }

    // Close file.
    fclose(input);

    // I now just need to find the best combination. It would be very wasteful
    // to try all possible combinations of values <= 100. So we will limit
    // ourselves to values == 100.
    Ingredient ingredients[properties[0]->length];
    for (int i = 0; i < properties[0]->length; i++)
        for (int p = 0; p < N_PROPERTIES; p++)
            ingredients[i].properties[p] = properties[p]->values[i];

    int score = eval_combinations(ingredients, properties[0]->length, SUM_OF_INGREDIENTS);

    printf("Part one: %d\n", score);

    // For part two I wanted to compute the only the combinations that have 500
    // calories using the same approach as above, but it is not possible
    // because it becomes the knapsack problem. It would be possible if the
    // ingredients were all worth 1 calory.
    int score_two = eval_combinations_two(ingredients, properties[0]->length, SUM_OF_INGREDIENTS);

    printf("Part two: %d\n", score_two);

    // Free ingredients and properties.
    for (int i = 0; i < N_PROPERTIES; i++)
        IntList_free(properties[i]);

    return check_result_i(score, EXPECTED_1) | check_result_i(score_two, EXPECTED_2);
}