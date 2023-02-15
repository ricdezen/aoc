#ifndef MYSTRING_H
#define MYSTRING_H

typedef struct {
    char *raw;
    int length;
    int _capacity;
} String;

void String_init_empty(String *string);

void String_init_from(String *string, const char *str, const int len);

String *String_new_empty();

String *String_new_from(const char *str, const int len);

void String_destroy(String *string);

void String_free(String *string);

void String_append(String *string, char c);

void String_concat(String *left, const String *right);

/**
 * @return int 1 if the strings are equal, 0 if not.
 */
int String_equals(const String *left, const String *right);

#endif