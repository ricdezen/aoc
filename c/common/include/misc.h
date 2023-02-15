#ifndef MISC_H
#define MISC_H

void swapi(int *a, int *b);

int check_result_i(const int actual, const int expected);

int check_result_s(const char *act, const int len_act, const char *exp, const int len_exp);

#endif