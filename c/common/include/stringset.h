#ifndef STRINGSET_H
#define STRINGSET_H

#include "mystring.h"

/**
 * This is NOT a hash set. This is a set, but it performs linear search.
 * It is used for cases where you need to assing a set of strings a number.
 * Examples are days 9 and 13, where you have a set of names to which you may
 * want to give an index to build the graph's edge matrix.
 */
typedef struct {
    String **strings;
    int capacity;
    int size;
} StringSet;

void StringSet_init(StringSet *set);

StringSet *StringSet_new();

void StringSet_destroy(StringSet *set);

void StringSet_free(StringSet *set);

/**
 * @param string String to check.
 * @return int The index of the string if it is in the set, or -1 otherwise.
 */
int StringSet_contains(const StringSet *set, const String *string);

/**
 * @param string String to add. A copy is made.
 * @return int The index of the string. Cannot distinguish between the string being added or being already present.
 */
int StringSet_add(StringSet *set, const String *string);

int StringSet_add_ptr(StringSet *set, const char *ptr);

#endif