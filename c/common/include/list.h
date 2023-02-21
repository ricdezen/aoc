#ifndef LIST_H
#define LIST_H

typedef struct {
    int *values;
    int length;
    int capacity;
} IntList;

void IntList_init(IntList *list);

IntList *IntList_new();

void IntList_destroy(IntList *list);

void IntList_free(IntList *list);

void IntList_push(IntList *list, const int value);

/**
 * @brief Remove and return the last value.
 * WARNING: behaviour is undefined on an empty list.
 *
 * @param list A list.
 * @return int The last value in the list, which is also removed.
 */
int IntList_pop(IntList *list);

/**
 * @param value Value to check.
 * @return int The index of the value if it is in the list, or -1 otherwise.
 */
int IntList_contains(const IntList *list, const int value);

#endif