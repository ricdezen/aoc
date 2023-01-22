#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 128
#define PACKAGE_SIZES 3

int min_array(int *a, int len) {
    int min = a[0];
    for (int i = 1; i < len; i++)
        if (a[i] < min)
            min = a[i];
    return min;
}

int freadline(FILE *file, char *line, int max_len) {
    int c = EOF, len = 0;
    while (len < max_len && (c = fgetc(file)) != EOF && c != '\n')
        line[len++] = (char)c;
    return len;
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("day_2.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    int total_paper = 0;
    int total_ribbon = 0;

    // Read the next line.
    char line[MAX_LINE_LENGTH] = {0};
    int len = freadline(input, line, sizeof(line));

    while (len != 0) {
        printf("line: %s\n", line);

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
            perim[i] = 2 * (sizes[i] + sizes[(i + 1) % PACKAGE_SIZES]);
            total_paper += areas[i] * 2;
        }
        total_paper += min_array(areas, PACKAGE_SIZES);
        total_ribbon += min_array(perim, PACKAGE_SIZES) + sizes[0] * sizes[1] * sizes[2];

        // Next line.
        len = freadline(input, line, MAX_LINE_LENGTH);
    }

    // Print result.
    printf("The elves should buy %d total square feet of paper.\n", total_paper);
    printf("The elves should buy %d total feet of ribbon.\n", total_ribbon);

    return 0;
}