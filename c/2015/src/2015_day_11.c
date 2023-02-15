#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileutils.h"
#include "min_max.h"
#include "mystring.h"

void increment(String *string) {
    const char wrap = 'z' + 1;
    int carry = 1;

    for (int i = string->length - 1; i >= 0 && carry; i--) {
        char newc = string->raw[i] + carry;

        // Wrap back to 'a'.
        // I am trying to avoid a modulo.
        if (newc >= wrap) {
            newc -= 26;
            carry = 1;
        } else {
            carry = 0;
        }

        // Skip o, i, l.
        if (newc == 'o' || newc == 'i' || newc == 'l')
            newc++;

        // Apply char.
        string->raw[i] = newc;
    }
}

int has_straight(String *string) {
    for (int i = 0; i < string->length - 2; i++)
        if (string->raw[i] == string->raw[i + 1] - 1 && string->raw[i] == string->raw[i + 2] - 2)
            return 1;
    return 0;
}

int has_pairs(String *string) {
    int pairs = 0;
    for (int i = 0; i < string->length - 1; i++) {
        if (string->raw[i] == string->raw[i + 1]) {
            pairs++;
            i += 1;
        }
    }
    return pairs >= 2;
}

String *get_next_password(String *input) {
    String *next = String_new_from(input->raw, input->length);

    // Increment as if it were a number.
    while (1) {
        // Increment.
        increment(next);

        // If the password meets the requirements, we can quit.
        if (has_straight(next) && has_pairs(next))
            break;
    }

    return next;
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_11.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    // Read file into a string.
    String *content = ftostring(input);

    // Close file.
    fclose(input);

    // Get the next password.
    String *next = get_next_password(content);

    printf("Next password: %s\n", next->raw);

    String *next2 = get_next_password(next);

    printf("Nexter password idk: %s\n", next2->raw);

    String_free(content);
    String_free(next);
    String_free(next2);

    return 0;
}