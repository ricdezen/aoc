#include <min_max.h>

int mini(const int a, const int b) { return (a < b) ? a : b; }

int array_mini(const int *array, const int length) {
    int min = array[0];
    for (int i = 1; i < length; i++)
        if (array[i] < min)
            min = array[i];
    return min;
}

int argmini(const int *array, const int length) {
    int min_i = 0;
    for (int i = 1; i < length; i++)
        if (array[i] < array[min_i])
            min_i = i;
    return min_i;
}

int maxi(const int a, const int b) { return (a > b) ? a : b; }

int array_maxi(const int *array, const int length) {
    int max = array[0];
    for (int i = 1; i < length; i++)
        if (array[i] > max)
            max = array[i];
    return max;
}

int argmaxi(const int *array, const int length) {
    int max_i = 0;
    for (int i = 1; i < length; i++)
        if (array[i] > array[max_i])
            max_i = i;
    return max_i;
}
