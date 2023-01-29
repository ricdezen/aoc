#define _CRT_SECURE_NO_DEPRECATE

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t uint8;

#define MAX_LINE_LENGTH 128

int freadline(FILE *file, char *line, int max_len) {
    int c = EOF, len = 0;
    while (len < max_len && (c = fgetc(file)) != EOF && c != '\n')
        line[len++] = (char)c;
    line[len] = 0;
    return len;
}

int is_nice(char *line, int length) {
    // This can probably be done better. But the exercise is so simple it is
    // hardly worth extra effort.
    int twice = 0;
    int vowels = 0;

    int i;
    for (i = 0; i < length - 1; i++) {
        int i1 = i + 1;

        // Bad substrings.
        if (line[i] == 'a' && line[i1] == 'b')
            return 0;
        if (line[i] == 'c' && line[i1] == 'd')
            return 0;
        if (line[i] == 'p' && line[i1] == 'q')
            return 0;
        if (line[i] == 'x' && line[i1] == 'y')
            return 0;

        // Same letter twice in a row.
        if (line[i] == line[i1])
            twice = 1;

        // Vowels.
        if (line[i] == 'a' || line[i] == 'e' || line[i] == 'i' || line[i] == 'o' || line[i] == 'u')
            vowels++;
    }

    // Last char (i = length - 1).
    if (line[i] == 'a' || line[i] == 'e' || line[i] == 'i' || line[i] == 'o' || line[i] == 'u')
        vowels++;

    return vowels >= 3 && twice > 0;
}

int is_nice_two(char *line, int length) {
    // Cannot fullfill requirement two.
    if (length < 4)
        return 0;

    // Check for requirement two.
    int between = 0;
    for (int i = 0; i < length - 2; i++)
        if (line[i] == line[i + 2]) {
            between = 1;
            break;
        }

    // No need to check further if one requirement is not met.
    if (between == 0)
        return 0;

    // Special case, >= 4 consec letters.
    int max_cons = 0;
    int consec = 1;
    char c = line[0];
    for (int i = 1; i < length; i++) {
        if (line[i] == c) {
            consec++;
        } else {
            consec = 1;
            c = line[i];
        }
        // Update max consec.
        max_cons = (consec > max_cons) ? consec : max_cons;
    }

    if (max_cons >= 4)
        return 1;

    // An extremely simple hash-table allows us to check in time O(1) if a certain
    // string has been already seen. The hash of a two character substring is just
    // s[0] * 256 + s[1]. If we had wide characters, then a regex would
    // be more convenient to write.
    uint8 hashTable[256][256] = {0};
    uint8 *line8 = (uint8 *)line;
    hashTable[line8[0]][line8[1]] = 1;
    int repeats = 0;

    for (int i = 1; i < length - 1; i++) {
        // This does not contemplate the special case of having >= 4 consec chars.
        // Only pairs of identical letters can overlap.
        if ((line8[i - 1] == line8[i]) && (line8[i] == line8[i + 1]))
            continue;

        // Is pair already found? Then we are done.
        if (hashTable[line8[i]][line8[i + 1]] > 0) {
            repeats = 1;
            break;
        }

        // Mark pair as found.
        hashTable[line8[i]][line8[i + 1]] = 1;
    }

    return repeats > 0;
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_5.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    int nice_strings = 0;
    int nice_strings_two = 0;

    // Read the next line.
    char line[MAX_LINE_LENGTH] = {0};
    int len = freadline(input, line, sizeof(line));

    while (len != 0) {
        if (is_nice(line, len))
            nice_strings++;
        if (is_nice_two(line, len))
            nice_strings_two++;

        // Next line.
        len = freadline(input, line, MAX_LINE_LENGTH);
    }

    // Print result.
    printf("There are %d nice strings.\n", nice_strings);
    printf("There are %d nice strings in part two.\n", nice_strings_two);

    fclose(input);

    return 0;
}