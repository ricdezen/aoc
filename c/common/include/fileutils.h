#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <stdio.h>

#include "mystring.h"

/**
 * @brief Read a single line from a file.
 *
 * @param file
 * @param line
 * @param max_len
 * @return int Length of line read.
 */
int freadline(FILE *file, char *line, int max_len);

/**
 * @brief Put the content of a file into a string.
 *
 * @param file The file to read.
 * @return String The content of the file. NULL if an error happened.
 */
String *ftostring(FILE *file);

#endif