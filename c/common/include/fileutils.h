#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <stdio.h>

/**
 * @brief Read a single line from a file.
 *
 * @param file
 * @param line
 * @param max_len
 * @return int Length of line read.
 */
int freadline(FILE *file, char *line, int max_len);

#endif