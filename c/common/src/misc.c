#include "misc.h"

#include <stdio.h>
#include <string.h>

void swapi(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int check_result_i(const int actual, const int expected) {
    if (actual != expected) {
        fprintf(stderr, "\033[0;31mGot result %d but expected %d.\033[0m\n", actual, expected);
        return 69;
    }
    return 0;
}

int check_result_s(const char *act, const int len_act, const char *exp, const int len_exp) {
    if (len_act != len_exp)
        return 69;
    return strncmp(act, exp, len_act) ? 69 : 0;
}
