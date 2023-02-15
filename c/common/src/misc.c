#include "misc.h"

void swapi(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}