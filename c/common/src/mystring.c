#include "mystring.h"

#include <stdlib.h>
#include <string.h>

#include "min_max.h"

#define STRING_INITIAL_CAPACITY 8

void String_init_empty(String *string) {
    string->raw = (char *)malloc(STRING_INITIAL_CAPACITY);
    memset(string->raw, 0, STRING_INITIAL_CAPACITY);
    string->_capacity = STRING_INITIAL_CAPACITY;
    string->length = 0;
}

void String_init_from(String *string, const char *str, const int len) {
    string->length = len;
    string->raw = (char *)malloc(string->length + 1);
    memcpy(string->raw, str, string->length);
    string->raw[string->length] = 0;
    string->_capacity = string->length + 1;
}

String *String_new_empty() {
    String *string = (String *)malloc(sizeof(String));
    String_init_empty(string);
    return string;
}

String *String_new_from(const char *str, const int len) {
    String *string = (String *)malloc(sizeof(String));
    String_init_from(string, str, len);
    return string;
}

void String_destroy(String *string) {
    string->length = -1;
    string->_capacity = -1;
    free(string->raw);
    string->raw = NULL;
}

void String_free(String *string) {
    String_destroy(string);
    free(string);
}

void String_append(String *string, char c) {
    // Resize if needed.
    if (string->length == string->_capacity - 1) {
        string->raw = (char *)realloc(string->raw, string->_capacity * 2);
        string->_capacity *= 2;
    }

    // Append char.
    string->raw[string->length++] = c;
    string->raw[string->length] = 0;
}

void String_concat(String *left, const String *right) {
    // I could just append every character but that is no fun.
    int tot_len = left->length + right->length;

    // Resize if needed.
    if (tot_len + 1 > left->_capacity) {
        int new_cap = maxi(left->_capacity * 2, tot_len + 1);
        left->raw = (char *)realloc(left->raw, new_cap);
        left->_capacity = new_cap;
    }

    // Copy the right string to the end of the first one.
    memcpy(left->raw + left->length, right->raw, right->length);
    left->length = tot_len;
    left->raw[tot_len] = 0;
}

int String_equals(const String *left, const String *right) {
    if (left->length != right->length)
        return 1;

    return strncmp(left->raw, right->raw, left->length) ? 0 : 1;
}
