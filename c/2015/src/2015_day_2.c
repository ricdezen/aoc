#define EXPECTED_1 1606483
#define EXPECTED_2 3842356

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileutils.h"
#include "misc.h"

#define MAX_LINE_LENGTH 128
#define PACKAGE_SIZES 3

int min_array(int *a, int len) {
    int min = a[0];
    for (int i = 1; i < len; i++)
        if (a[i] < min)
            min = a[i];
    return min;
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_2.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    int total_paper = 0;
    int total_ribbon = 0;

    // Read the next line.
    char line[MAX_LINE_LENGTH] = {0};
    int len = freadline(input, line, MAX_LINE_LENGTH);

    while (len != 0) {
        // Tokenize the line to get the package sizes.
        int sizes[PACKAGE_SIZES] = {0};
        char *token = strtok(line, "x");
        for (int i = 0; i < PACKAGE_SIZES; i++) {
            // Check that we do in fact get 3 package sizes.
            if (token == NULL) {
                fprintf(stderr, "Not enough package sizes.\n");
                return 1;
            }

            // Get size, assume valid int.
            char *endptr = NULL;
            sizes[i] = strtol(token, &endptr, 10);
            token = strtok(NULL, "x");
        }

        // Get total paper.
        // NOTE: This does not work for PACKAGE_SIZES != 3.
        // Kinda pointless now that i think about it.
        int areas[PACKAGE_SIZES] = {0};
        int perim[PACKAGE_SIZES] = {0};
        for (int i = 0; i < PACKAGE_SIZES; i++) {
            areas[i] = sizes[i] * sizes[(i + 1) % PACKAGE_SIZES];
            total_paper += areas[i] * 2;

            // Part two: perimeters.
            perim[i] = 2 * (sizes[i] + sizes[(i + 1) % PACKAGE_SIZES]);
        }
        total_paper += min_array(areas, PACKAGE_SIZES);
        total_ribbon += min_array(perim, PACKAGE_SIZES) + sizes[0] * sizes[1] * sizes[2];

        // Next line.
        len = freadline(input, line, MAX_LINE_LENGTH);
    }

    fclose(input);

    // Print result.
    printf("The elves should buy %d total square feet of paper.\n", total_paper);
    printf("The elves should buy %d total feet of ribbon.\n", total_ribbon);

    // Assert result is correct.
    return check_result_i(total_paper, EXPECTED_1) | check_result_i(total_ribbon, EXPECTED_2);
}