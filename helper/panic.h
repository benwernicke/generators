#ifndef PANIC_H
#define PANIC_H

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PANIC(...)                                                             \
    do {                                                                       \
        fprintf(stderr, "\033[31mError: \033[39m");                            \
        fprintf(stderr, __VA_ARGS__);                                          \
        putc('\n', stderr);                                                    \
        exit(1);                                                               \
    } while (0)

#define PANIC_IF(b, ...)                                                       \
    do {                                                                       \
        if (b) {                                                               \
            PANIC(__VA_ARGS__);                                                \
        }                                                                      \
    } while (0)

#endif
