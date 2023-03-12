#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern char *input;

void RuntimeErrorInternal(char *file, int line, char *format, ...){
    va_list arguments;
    va_start(arguments, format);
    fprintf(stderr, "\n[%s:%d] ", file, line);
    vfprintf(stderr, format, arguments);
    fprintf(stderr, "\n\n");
    exit(EXIT_FAILURE);
}

void SyntaxError(char *position, char *format, ...){
    va_list arguments;
    va_start(arguments, format);
    fprintf(stderr, "\n%s\n%*s^ ", input, (int)(position - input), "");
    vfprintf(stderr, format, arguments);
    fprintf(stderr, "\n\n");
    exit(EXIT_FAILURE);
}
