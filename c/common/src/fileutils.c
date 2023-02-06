#include "fileutils.h"

#include <stdlib.h>

int freadline(FILE *file, char *line, int max_len) {
    int c = EOF, len = 0;
    while (len < max_len && (c = fgetc(file)) != EOF && c != '\n')
        line[len++] = (char)c;
    line[len] = 0;
    return len;
}

String *ftostring(FILE *file) {
    int file_size = 0;

    // Get size of the file.
    if (fseek(file, 0, SEEK_END) < 0)
        return NULL;
    if ((file_size = ftell(file)) < 0)
        return NULL;
    if (fseek(file, 0, SEEK_SET) < 0)
        return NULL;

    // Read file one char at a time.
    String *string = (String *)malloc(sizeof(String));
    String_init_empty(string);
    int c = EOF;
    while ((c = fgetc(file)) != EOF)
        String_append(string, (char)c);

    // Check that there was not an error before the end.
    if (string->length < file_size) {
        String_free(string);
        return NULL;
    }

    return string;
}
