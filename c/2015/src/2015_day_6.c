#define EXPECTED_1 569999
#define EXPECTED_2 17836115

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileutils.h"
#include "misc.h"

typedef uint8_t uint8;

#define MAX_LINE_LENGTH 128
#define GRID_SIZE 1000
#define TURN_ON 0
#define TURN_OFF 1
#define TOGGLE 2

int parse_command(char *line, int length, int *start_x, int *start_y, int *end_x, int *end_y) {
    char *token = strtok(line, " ");
    int command = -1;

    // Parse the command type.
    if (!strncmp(token, "toggle", 7))
        command = TOGGLE;
    else if (!strncmp(token, "turn", 5)) {
        token = strtok(NULL, " ");
        if (!strncmp(token, "on", 3))
            command = TURN_ON;
        else if (!strncmp(token, "off", 4))
            command = TURN_OFF;
    }
    if (command == -1)
        return -1;

    // Parse coordinates.
    token = strtok(NULL, ",");
    *start_x = atoi(token);
    token = strtok(NULL, " ");
    *start_y = atoi(token);

    // Check for "through".
    token = strtok(NULL, " ");
    if (strncmp(token, "through", 8))
        return -1;

    token = strtok(NULL, ",");
    *end_x = atoi(token);
    token = strtok(NULL, " ");
    *end_y = atoi(token);

    return command;
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_6.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    // Read the next line.
    char line[MAX_LINE_LENGTH] = {0};
    int len = freadline(input, line, MAX_LINE_LENGTH);

    // Grid of lights.
    char *grid = (char *)malloc(GRID_SIZE * GRID_SIZE);
    char *grid_two = (char *)malloc(GRID_SIZE * GRID_SIZE);
    memset(grid, 0, GRID_SIZE * GRID_SIZE);
    memset(grid_two, 0, GRID_SIZE * GRID_SIZE);

    while (len != 0) {
        // Get command from line.
        int start_x, start_y, end_x, end_y;
        int command = parse_command(line, len, &start_x, &start_y, &end_x, &end_y);

        // Command could not be parsed.
        if (command == -1) {
            printf("Ayo wtf\n");
            exit(1);
        }

        // Execute the command.
        switch (command) {
        case TURN_ON: {
            for (int x = start_x; x <= end_x; x++)
                for (int y = start_y; y <= end_y; y++) {
                    // Part one: turn on.
                    grid[y * GRID_SIZE + x] = 1;

                    // Part two: increase brightness.
                    grid_two[y * GRID_SIZE + x]++;
                }
            break;
        }
        case TURN_OFF: {
            for (int x = start_x; x <= end_x; x++)
                for (int y = start_y; y <= end_y; y++) {
                    // Part one: turn off.
                    grid[y * GRID_SIZE + x] = 0;

                    // Part two: decrease brightness.
                    if (grid_two[y * GRID_SIZE + x] > 0)
                        grid_two[y * GRID_SIZE + x]--;
                }
            break;
        }
        case TOGGLE: {
            for (int x = start_x; x <= end_x; x++)
                for (int y = start_y; y <= end_y; y++) {
                    // Part one: toggle.
                    grid[y * GRID_SIZE + x] = (~grid[y * GRID_SIZE + x]) & 1;

                    // Part two: increase brightness by two.
                    grid_two[y * GRID_SIZE + x] += 2;
                }
            break;
        }
        }

        // Next line.
        len = freadline(input, line, MAX_LINE_LENGTH);
    }

    // Print result.
    int lit = 0, brightness = 0;
    for (int x = 0; x < GRID_SIZE; x++)
        for (int y = 0; y < GRID_SIZE; y++) {
            lit += grid[y * GRID_SIZE + x];
            brightness += grid_two[y * GRID_SIZE + x];
        }

    printf("There are %d lit lights.\n", lit);
    printf("There is %d total brightness.\n", brightness);

    fclose(input);
    free(grid);

    return check_result_i(lit, EXPECTED_1) | check_result_i(brightness, EXPECTED_2);
}