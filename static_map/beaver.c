#include "lib/beaver.h"

#define FLAGS "-g -Wall -Werror -Og -pipe"
#define FAST_FLAGS "-O2 -march=native -pipe"

module_t modules[] = {
    { .name = "main", .src = "main.c" },
    { .name = "main", .src = "../helper/string_check.c" },
    { .name = "main", .src = "buf_hpair.c" },
};

uint32_t modules_len = sizeof(modules) / sizeof(*modules);

char* program[] = { "main", NULL };

int main(int argc, char** argv)
{
    auto_update(argv);
    if (argc == 1) {
        compile(program, FLAGS);
    } else if (strcmp(argv[1], "clean") == 0) {
        rm("$(find build -type f)");
        rm("out");
    } else if (strcmp(argv[1], "fast") == 0) {
        compile(program, FAST_FLAGS);
    } else if (strcmp(argv[1], "install") == 0) {
        call_or_panic("cp out /usr/bin/gen_static_map");
    } else {
        fprintf(stderr, "\033[31mError:\033[39m unknown option: '%s'\n", argv[1]);
        exit(1);
    }
    return 0;
}
