#include "list.h"

#define LIST_DEFAULT_CAPACITY 8

void IntList_init(IntList *list) {
    list->values = (int *)malloc(sizeof(int) * LIST_DEFAULT_CAPACITY);
    list->length = 0;
    list->capacity = LIST_DEFAULT_CAPACITY;
}

IntList *IntList_new() {
    IntList *new_list = (IntList *)malloc(sizeof(IntList));
    IntList_init(new_list);
    return new_list;
}

void IntList_destroy(IntList *list) {
    free(list->values);
    list->length = -1;
    list->capacity = -1;
}

void IntList_free(IntList *list) {
    IntList_destroy(list);
    free(list);
}

void IntList_push(IntList *list, const int value) {
    // Resize if needed.
    if (list->length == list->capacity) {
        list->values = (int *)realloc(list->values, sizeof(int) * list->capacity * 2);
        list->capacity *= 2;
    }

    // Add the element.
    list->values[list->length++] = value;
}

int IntList_pop(IntList *list) { return list->values[--list->length]; }

int IntList_contains(const IntList *list, const int value) {
    for (int i = 0; i < list->length; i++)
        if (list->values[i] == value)
            return 1;
    return 0;
}
