#ifndef BUF_HPAIR_H
#define BUF_HPAIR_H

#include <stdint.h>
#include "../helper/genbuf.h"

typedef struct hpair_t hpair_t;
struct hpair_t {
    char* s;
    uint64_t h;
};

GENBUF_MAKE_HEADER(hpair_t)

#endif
