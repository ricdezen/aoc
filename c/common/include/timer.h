#ifndef TIMER_H
#define TIMER_H

#include <time.h>

/**
 * @brief Measure execution time.
 *
 * @param __foo__ The expression for which to compute the execution time.
 * @param __out__ Reference to a double variable which will contain the time.
 */
#define TIME_MS(__foo__, __out__)                                                                                      \
    {                                                                                                                  \
        clock_t __begin__ = clock();                                                                                   \
        __foo__;                                                                                                       \
        clock_t __end__ = clock();                                                                                     \
        __out__ = (double)(__end__ - __begin__) * 1000 / CLOCKS_PER_SEC;                                               \
    }

#endif