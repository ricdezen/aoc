#define EXPECTED_1 360154
#define EXPECTED_2 5103798

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileutils.h"
#include "min_max.h"
#include "misc.h"
#include "mystring.h"

String *look_and_say(String *input) {
    String *output = String_new_empty();

    int n = 1;
    char c = input->raw[0];
    for (int i = 1; i < input->length; i++) {
        if (input->raw[i] == c)
            n++;
        else {
            // Add number of occurrences.
            // An int will never take more than 10 digits.
            char n_str[16] = {0};
            sprintf(n_str, "%d", n);
            String *n_string = String_new_from(n_str, strlen(n_str));
            String_concat(output, n_string);
            String_free(n_string);

            // Add char.
            String_append(output, c);

            // Start counting new char.
            c = input->raw[i];
            n = 1;
        }
    }

    // Last occurrences (string ended before the char could change).
    char n_str[16] = {0};
    sprintf(n_str, "%d", n);
    String *n_string = String_new_from(n_str, strlen(n_str));
    String_concat(output, n_string);
    String_free(n_string);
    String_append(output, c);

    return output;
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_10.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    // Read file into a string.
    String *content = ftostring(input);

    // Close file.
    fclose(input);

    // Lengths.
    int len_one = 0;
    int len_two = 0;

    // Run method 40 times.
    for (int i = 0; i < 40; i++) {
        String *old = content;
        content = look_and_say(content);
        String_free(old);
    }

    // Print result length.
    len_one = content->length;
    printf("Result is %d chars long.\n", len_one);

    // Run method another 10 times.
    for (int i = 0; i < 10; i++) {
        String *old = content;
        content = look_and_say(content);
        String_free(old);
    }

    // Print result length.
    len_two = content->length;
    printf("Result is %d chars long.\n", len_two);

    String_free(content);

    return check_result_i(len_one, EXPECTED_1) | check_result_i(len_two, EXPECTED_2);
}