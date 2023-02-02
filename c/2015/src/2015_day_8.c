#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 128

int freadline(FILE *file, char *line, int max_len) {
    int c = EOF, len = 0;
    while (len < max_len && (c = fgetc(file)) != EOF && c != '\n')
        line[len++] = (char)c;
    line[len] = 0;
    return len;
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_8.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    int total_literal_size = 0;
    int total_memory_size = 0;
    int total_escape_size = 0;

    // Read the next line.
    char line[MAX_LINE_LENGTH] = {0};
    int len = freadline(input, line, MAX_LINE_LENGTH);

    while (len != 0) {
        // Compute length of the string (just skip char after each backslash).
        int c = 0;
        for (int i = 1; i < len - 1; i++) {
            if (line[i] == '\\' && line[i + 1] == 'x')
                i += 3;
            else if (line[i] == '\\')
                i += 1;
            c++;
        }

        // Part two, escape. Opposite of above.
        int e = 0;
        for (int i = 1; i < len - 1; i++) {
            if (line[i] == '\\' || line[i] == '\"')
                e++;
            e++;
        }

        total_literal_size += len;
        total_memory_size += c;
        total_escape_size += e + 6;

        printf("Length of %s: %d / %d\n", line, len, c);

        // Next line.
        len = freadline(input, line, MAX_LINE_LENGTH);
    }

    // Print result.
    printf("1. Total literal size: %d\n", total_literal_size);
    printf("2. Total memory size: %d\n", total_memory_size);
    printf("3. Total escaped size: %d\n", total_escape_size);
    printf("1 - 2 Difference: %d\n", total_literal_size - total_memory_size);
    printf("3 - 1 Difference: %d\n", total_escape_size - total_literal_size);

    fclose(input);

    return 0;
}