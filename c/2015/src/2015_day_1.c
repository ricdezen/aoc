#define EXPECTED_1 138
#define EXPECTED_2 1771

#include <stdio.h>
#include <stdlib.h>

#include "misc.h"

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_1.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    int c = EOF;
    int len = 0;
    int floor = 0;
    int basement = -1;
    while ((c = fgetc(input)) != EOF) {
        // Count characters.
        len++;

        if (c == '(')
            // Go up one floor.
            floor++;
        else if (c == ')')
            // Go down one floor.
            floor--;
        else {
            // Quit on wrong char.
            fprintf(stderr, "Unknown character in input file: %c\n", (char)c);
            return 1;
        }

        // Part two: also check when it goes negative.
        if (basement == -1 && floor < 0)
            basement = len;
    }

    fclose(input);

    // Print result.
    printf("Santa ends up at floor: %d\n", floor);
    printf("Reaches the basement at char: %d\n", basement);

    // Assert result is correct.
    return check_result_i(floor, EXPECTED_1) | check_result_i(basement, EXPECTED_2);
}