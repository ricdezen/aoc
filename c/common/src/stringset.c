#include "stringset.h"

#include <stdlib.h>
#include <string.h>

#define SET_INITIAL_CAPACITY 8

void StringSet_init(StringSet *set) {
    set->strings = (String **)malloc(sizeof(String *) * SET_INITIAL_CAPACITY);
    set->capacity = SET_INITIAL_CAPACITY;
    set->size = 0;
}

StringSet *StringSet_new() {
    StringSet *set = (StringSet *)malloc(sizeof(StringSet));
    StringSet_init(set);
    return set;
}

void StringSet_destroy(StringSet *set) {
    for (int i = 0; i < set->size; i++)
        String_free(set->strings[i]);
    free(set->strings);
    set->strings = NULL;
    set->capacity = -1;
    set->size = -1;
}

void StringSet_free(StringSet *set) {
    StringSet_destroy(set);
    free(set);
}

int StringSet_contains(const StringSet *set, const String *string) {
    for (int i = 0; i < set->size; i++)
        if (String_equals(set->strings[i], string))
            return i;
    return -1;
}

int StringSet_add(StringSet *set, const String *string) {
    int index;

    // String is already in set, return its index.
    if ((index = StringSet_contains(set, string)) >= 0)
        return index;

    // String is not in set, add it.
    // Resize if needed.
    if (set->size == set->capacity) {
        set->strings = (String **)realloc(set->strings, sizeof(String *) * set->capacity * 2);
        set->capacity *= 2;
    }

    // Copy the string.
    String *str = String_new_from(string->raw, string->length);
    set->strings[set->size++] = str;

    return set->size - 1;
}

int StringSet_add_ptr(StringSet *set, const char *ptr) {
    String *str = String_new_from(ptr, strlen(ptr));
    int i = StringSet_add(set, str);
    String_free(str);
    return i;
}
