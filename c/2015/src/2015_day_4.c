#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_1.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    // Read file into a string.
    fseek(input, 0, SEEK_END);
    int size = ftell(input);
    fseek(input, 0, SEEK_SET);
    char content[size + 1];
    content[size] = 0;
    size_t read = fread(content, 1, size, input);

    // Close file.
    fclose(input);

    return 0;
}