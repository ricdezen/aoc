/**
 * I would normally call a regex-based solution boring, but this just feels like
 * its on purpose. For part 2, on the other hand, I had to parse it by hand, to
 * avoid regex hell.
 */
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileutils.h"
#include "min_max.h"
#include "mystring.h"

int sum_numbers(String *json_document) {
    int result = 0;
    regex_t regex;
    regmatch_t match;
    char msgbuf[100];

    // Compile a regular expression finding all integer numbers.
    if (regcomp(&regex, "-?[0-9]+", REG_EXTENDED)) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    int offset = 0;

    // Find each integer.
    while (regexec(&regex, json_document->raw + offset, 1, &match, 0) != REG_NOMATCH) {
        char tmp[16] = {0};
        int match_len = match.rm_eo - match.rm_so;
        memcpy(tmp, json_document->raw + offset + match.rm_so, match_len);
        offset += match.rm_eo;
        result += atoi(tmp);
    }

    // Free regex.
    regfree(&regex);

    return result;
}

// --- Part two ---

int skip_number(const char *string, const int length, int *size) {
    *size = 0;
    while (*size < length) {
        if (!(string[*size] == '-' || (string[*size] >= '0' && string[*size] <= '9')))
            break;
        (*size)++;
    }
    if (*size == 0)
        return -1;
    return 0;
}

/**
 * Skip to the end of the next string.
 *
 * @param size The number of characters skipped.
 * @return int 0 if all went good, -1 if there was no well-formed list in string.
 */
int skip_string(const char *string, const int length, int *size);

/**
 * Skip to the end of the next list.
 *
 * @param size The number of characters skipped.
 * @return int 0 if all went good, -1 if there was no well-formed list in string.
 */
int skip_list(const char *string, const int length, int *size);

/**
 * Skip to the end of the next dict.
 *
 * @param size The number of characters skipped.
 * @return int 0 if all went good, -1 if there was no well-formed dict in string.
 */
int skip_dict(const char *string, const int length, int *size);

int skip_string(const char *string, const int length, int *size) {
    // Look for end quotes, that's it. We should also check for escaped quotes
    // in a real scenario. Not a big deal here.
    *size = 0;
    while (string[*size] != '"' && *size < length)
        (*size)++;

    if (*size >= length)
        return -1;

    // Include closing quotes.
    (*size)++;

    return 0;
}

int skip_list(const char *string, const int length, int *size) {
    // Skip until end of list.
    *size = 0;
    while (string[*size] != ']') {
        // Increment and quit if necessary.
        (*size)++;
        if (*size >= length)
            break;

        // Check char and skip nested list.
        if (string[*size] == '[') {
            int sub_size = 0;
            skip_list(string + *size, length - *size, &sub_size);
            *size += sub_size;
        }
    }

    if (*size >= length)
        return -1;

    // Include closing ]
    (*size)++;

    return 0;
}

int skip_dict(const char *string, const int length, int *size) {
    // Skip until end of dict.
    *size = 0;
    while (string[*size] != '}') {
        // Increment and quit if necessary.
        (*size)++;
        if (*size >= length)
            break;

        // Check char and skip nested dict.
        if (string[*size] == '{') {
            int sub_size = 0;
            skip_dict(string + *size, length - *size, &sub_size);
            *size += sub_size;
        }
    }

    if (*size >= length)
        return -1;

    // Include closing }
    (*size)++;

    return 0;
}

/**
 * Similar to skip_list but also computes the sum of the integers you find on the way.
 */
int eval_list(const char *string, const int length, int *sum_ptr, int *size_ptr);

/**
 * Similar to skip_dict but also compute the sum of the integers you find on the way.
 */
int eval_dict(const char *string, const int length, int *sum_ptr, int *size_ptr);

int eval_list(const char *string, const int length, int *sum_ptr, int *size_ptr) {
    // Since the dictionary could be very deep, I wanted to check for "red" values
    // before starting evaluation, but then I realized that I was just running through
    // the whole parsing twice, and it made no sense.
    int sum = 0;
    int len = 0;
    int tmp = 0;

    len = 0;
    while (len < length && string[len] != ']') {
        // Try to consume comma.
        if (len > 0 && string[len++] != ',')
            return -1;

        // The list can start with anything.
        if (string[len] == '"') {
            // Skip strings.
            if (skip_string(string + len + 1, length - len - 1, &tmp))
                return -1;
            len += tmp + 1;
        } else if (string[len] == '-' || (string[len] >= '0' && string[len] <= '9')) {
            // Number, add it to sum.
            if (skip_number(string + len, length - len, &tmp))
                return -1;
            char num[16] = {0};
            memcpy(num, string + len, tmp);
            sum += atoi(num);
            len += tmp;
        } else if (string[len] == '{') {
            // Parse sub dict.
            int sub_sum = 0;
            int sub_size = 0;
            if (eval_dict(string + len + 1, length - len - 1, &sub_sum, &sub_size))
                return -1;
            sum += sub_sum;
            len += sub_size + 1;
        } else if (string[len] == '[') {
            // Parse sub list.
            int sub_sum = 0;
            int sub_size = 0;
            if (eval_list(string + len + 1, length - len - 1, &sub_sum, &sub_size))
                return -1;
            sum += sub_sum;
            len += sub_size + 1;
        }
    }

    if (len >= length)
        return -1;

    // Include closing ]
    len++;

    *sum_ptr = sum;
    *size_ptr = len;
    return 0;
}

int eval_dict(const char *string, const int length, int *sum_ptr, int *size_ptr) {
    // Since the dictionary could be very deep, I wanted to check for "red" values
    // before starting evaluation, but then I realized that I was just running through
    // the whole parsing twice, and it made no sense.
    int sum = 0;
    int len = 0;
    int tmp = 0;

    while (len < length && string[len] != '}') {
        // Skip comma.
        if (len > 0 && string[len++] != ',')
            return -1;

        // I expect a string key.
        if (string[len++] != '"')
            return -1;

        // Skip the key.
        if (skip_string(string + len, length - len, &tmp))
            return -1;
        len += tmp;

        // Then I expect column :
        if (string[len++] != ':')
            return -1;

        // Then, we have to quit if we find a value that is the string "red".
        if (string[len] == '"') {
            // Check if string key is "red".
            if (skip_string(string + len + 1, length - len - 1, &tmp))
                return -1;
            if (!strncmp("red\"", string + len + 1, tmp)) {
                // Found "red", skip rest of dict.
                sum = 0;
                skip_dict(string + len + 1, length - len - 1, &tmp);
                tmp--; // Leave the closing bracket for while condition above.
            }
            // Would be tmp + 1 but we don't skip the closing bracket
            // to use the loop condition to quit.
            len += tmp + 1;
        } else if (string[len] == '-' || (string[len] >= '0' && string[len] <= '9')) {
            // Number, add it to sum.
            if (skip_number(string + len, length - len, &tmp))
                return -1;
            char num[16] = {0};
            memcpy(num, string + len, tmp);
            sum += atoi(num);
            len += tmp;
        } else if (string[len] == '{') {
            // Parse sub dict.
            int sub_sum = 0;
            int sub_size = 0;
            if (eval_dict(string + len + 1, length - len - 1, &sub_sum, &sub_size))
                return -1;
            sum += sub_sum;
            len += sub_size + 1;
        } else if (string[len] == '[') {
            // Parse sub list.
            int sub_sum = 0;
            int sub_size = 0;
            if (eval_list(string + len + 1, length - len - 1, &sub_sum, &sub_size))
                return -1;
            sum += sub_sum;
            len += sub_size + 1;
        }
    }

    if (len >= length)
        return -1;

    // Include closing }
    len++;

    *sum_ptr = sum;
    *size_ptr = len;
    return 0;
}

int sum_numbers_two(String *json_document) {
    int result = 0;
    int size = 0;

    // Breathe in, breathe out. This will not be fun. We need to recursively
    // parse each sub-element. For each dictionary we have to detect if it
    // contains a value of "red" for one of its keys. Each number we find must
    // be added.
    if (eval_dict(json_document->raw + 1, json_document->length - 1, &result, &size)) {
        fprintf(stderr, "Could not evaluate dict.\n");
        exit(1);
    }

    return result;
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_12.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    // Read file into a string.
    String *content = ftostring(input);

    // Close file.
    fclose(input);

    // Print result.
    printf("Sum of numbers: %d\n", sum_numbers(content));

    // Print result of part two.
    printf("Sum (except \"red\"): %d\n", sum_numbers_two(content));

    String_free(content);

    return 0;
}