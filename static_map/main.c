#include <stdint.h>
#include <stdio.h>

#include "../helper/panic.h"
#include "../helper/string_check.h"
#include "buf_hpair.h"

uint8_t fnv_8(const char* s, uint8_t prime_0, uint8_t prime_1)
{
    uint8_t h = prime_0;
    for (; *s; s++) {
        h *= prime_1;
        h ^= *s;
    }
    return h;
}

uint16_t fnv_16(const char* s, uint16_t prime_0, uint16_t prime_1)
{
    uint16_t h = prime_0;
    for (; *s; s++) {
        h *= prime_1;
        h ^= *s;
    }
    return h;
}

uint32_t fnv_32(const char* s, uint32_t prime_0, uint32_t prime_1)
{
    uint32_t h = prime_0;
    for (; *s; s++) {
        h *= prime_1;
        h ^= *s;
    }
    return h;
}

uint64_t fnv_64(const char* s, uint64_t prime_0, uint64_t prime_1)
{
    uint64_t h = prime_0;
    for (; *s; s++) {
        h *= prime_1;
        h ^= *s;
    }
    return h;
}

uint64_t fnv_hash(
    const char* s, uint32_t int_size, uint64_t prime_0, uint64_t prime_1)
{
    switch (int_size) {
    case 8:
        return fnv_8(s, (uint8_t)prime_0, (uint8_t)prime_1);
    case 16:
        return fnv_16(s, (uint16_t)prime_0, (uint16_t)prime_1);
    case 32:
        return fnv_32(s, (uint32_t)prime_0, (uint32_t)prime_1);
    case 64:
        return fnv_64(s, (uint64_t)prime_0, (uint64_t)prime_1);
    default:
        __builtin_unreachable();
    }
    __builtin_unreachable();
}

buf_t(hpair_t)
    * fill_buf(
        char** argv, uint32_t int_size, uint64_t prime_0, uint64_t prime_1)
{
    buf_t(hpair_t)* b = buf_create(hpair_t)(1);
    PANIC_IF(!b, "could not allocate buffer: %s", strerror(errno));

    hpair_t* p;

    for (; *argv; ++argv) {
        uint64_t hash = fnv_hash(*argv, int_size, prime_0, prime_1);
        p = buf_more(hpair_t)(b);
        PANIC_IF(!p, "could not allocate buffer: %s", strerror(errno));
        p->s = *argv;
        p->h = hash;
    }
    return b;
}

void print_buf(buf_t(hpair_t) * b)
{
    hpair_t* iter = buf_begin(hpair_t)(b);
    hpair_t* end = buf_end(hpair_t)(b);

    for (; iter != end; ++iter) {
        printf("%s\t\t%lu\n", iter->s, iter->h);
    }
}

uint64_t atou64(const char* s)
{
    uint64_t u = 0;
    for (; *s; ++s) {
        u *= 10;
        u += *s - '0';
    }
    return u;
}

bool is_unique_buf(buf_t(hpair_t) * b)
{
    hpair_t* ia = buf_begin(hpair_t)(b);
    hpair_t* end = buf_end(hpair_t)(b);
    hpair_t* ib;

    for (; ia != end; ++ia) {
        ib = ia + 1;
        for (; ib != end; ++ib) {
            if (ib->h == ia->h) {
                return 0;
            }
        }
    }
    return 1;
}

char* g_int_literals[] = { "uint8_t", "uint16_t", "uint32_t", "uint64_t" };
char* get_int_literal(uint32_t int_size)
{
    switch (int_size) {
    case 8:
        return g_int_literals[0];
    case 16:
        return g_int_literals[1];
    case 32:
        return g_int_literals[2];
    case 64:
        return g_int_literals[3];
    }
    __builtin_unreachable();
}

void generate_code(
    buf_t(hpair_t) * b, uint32_t int_size, uint64_t prime_0, uint64_t prime_1)
{
    hpair_t* iter = buf_begin(hpair_t)(b);
    hpair_t* end = buf_end(hpair_t)(b);

    char* int_literal = get_int_literal(int_size);

    printf("%s hash(const char* s)\n"
           "{\n"
           "   %s h = %lu;\n"
           "   for (; *s; s++) {\n"
           "       h *= %lu;\n"
           "       h ^= *s;\n"
           "   }\n"
           "   return h;\n"
           "}\n\n",
        int_literal, int_literal, prime_0, prime_1);

    printf("%s first_word_hash(char* s, char** start, char** end)\n"
           "{\n"
           "    for(; isspace(*s); ++s) { }\n"
           "    if(!*s) {\n"
           "        *start = NULL;\n"
           "        return -1;\n"
           "    }\n"
           "    %s h = %lu;\n"
           "    for(; *s && !isspace(*s); ++s) {\n"
           "       h *= %lu;\n"
           "       h ^= *s;\n"
           "    }\n"
           "    *end = s;\n"
           "    return h;"
           "}\n",
        int_literal, int_literal, prime_0, prime_1);

    printf("switch(hash(s)) {\n");
    for (; iter != end; ++iter) {
        printf("case %lu:", iter->h);
        printf("if(strcmp(\"%s\", s) == 0) { match_%s }", iter->s, iter->s);
        printf("else { match_error }");
        printf("break;\n");
    }
    printf("default: match_error break;");
    printf("}\n");
}

int main(int argc, char** argv)
{
    PANIC_IF(argc < 5, "wrong number of arguments\n");
    PANIC_IF(!isnumber(argv[1]) || !isnumber(argv[2]) || !isnumber(argv[3]),
        "wrong type of arguments: %s <int_size> <first prime> <second prime> "
        "<identifier_0> <identifier_1> ...",
        argv[0]);

    uint32_t int_size = atou64(argv[1]);
    uint64_t prime_0 = atou64(argv[2]);
    uint64_t prime_1 = atou64(argv[3]);

    PANIC_IF(
        int_size != 8 && int_size != 16 && int_size != 32 && int_size != 64,
        "wrong int size; allowed values: 8, 16, 32, 64");

    buf_t(hpair_t)* b = fill_buf(argv + 4, int_size, prime_0, prime_1);

    PANIC_IF(!is_unique_buf(b),
        "primes given do not produce unique results for given identifiers: "
        "please select different ones");

    generate_code(b, int_size, prime_0, prime_1);

    buf_free(hpair_t)(b);

    return 0;
}
