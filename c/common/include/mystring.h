#ifndef MYSTRING_H
#define MYSTRING_H

typedef struct {
    char *raw;
    int length;
    int _capacity;
} String;

void String_init_empty(String *string);

void String_init_from(String *string, char *str);

String *String_new_empty();

String *String_new_from(char *str);

void String_destroy(String *string);

void String_free(String *string);

void String_append(String *string, char c);

void String_concat(String *left, String *right);

#endif