#ifndef GENBUF_H
#define GENBUF_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define buf_t(T) buf_t_##T

#define buf_size(T) buf_size_##T
#define buf_index_from_ptr(T) buf_index_from_ptr_##T

#define buf_create(T) buf_create_##T
#define buf_free(T) buf_free_##T

#define buf_more(T) buf_more_##T

#define buf_shrink_to_fit(T) buf_shrink_to_fit_##T
#define buf_reserve(T) buf_reserve_##T

#define buf_get(T) buf_get_##T
#define buf_get_copy(T) buf_get_copy_##T

#define buf_begin(T) buf_begin_##T
#define buf_end(T) buf_end_##T

#define GENBUF_MAKE_HEADER(T)                                                  \
    typedef struct buf_t(T) buf_t(T);                                          \
                                                                               \
    buf_t(T) * buf_create(T)(uint32_t init_size);                              \
    void buf_free(T)(buf_t(T) * b);                                            \
                                                                               \
    T* buf_more(T)(buf_t(T) * b);                                              \
                                                                               \
    int buf_shrink_to_fit(T)(buf_t(T) * b);                                    \
    int buf_reserve(T)(buf_t(T) * b, uint32_t to_reserve);                     \
                                                                               \
    T* buf_get(T)(buf_t(T) * b, uint32_t index);                               \
    T buf_get_copy(T)(buf_t(T) * b, uint32_t index);                           \
                                                                               \
    T* buf_begin(T)(buf_t(T) * b);                                             \
    T* buf_end(T)(buf_t(T) * b);                                               \
    uint32_t buf_index_from_ptr(T)(buf_t(T) * b, T * p);                       \
    uint32_t buf_size(T)(buf_t(T) * b);

#define GENBUF_MAKE_IMPL(T)                                                    \
    typedef struct buf_t(T) buf_t(T);                                          \
    struct buf_t(T)                                                            \
    {                                                                          \
        uint32_t used;                                                         \
        uint32_t alloced;                                                      \
        T* buf;                                                                \
    };                                                                         \
                                                                               \
    buf_t(T) * buf_create(T)(uint32_t init_size)                               \
    {                                                                          \
        buf_t(T)* b = malloc(sizeof(*b));                                      \
        if (b == NULL) {                                                       \
            return NULL;                                                       \
        }                                                                      \
        b->alloced = init_size;                                                \
        b->used = 0;                                                           \
        b->buf = malloc(sizeof(*b->buf) * b->alloced);                         \
        if (b->buf == NULL) {                                                  \
            free(b);                                                           \
            return NULL;                                                       \
        }                                                                      \
        return b;                                                              \
    }                                                                          \
                                                                               \
    void buf_free(T)(buf_t(T) * b)                                             \
    {                                                                          \
        if (b == NULL) {                                                       \
            return;                                                            \
        }                                                                      \
        free(b->buf);                                                          \
        free(b);                                                               \
    }                                                                          \
                                                                               \
    static int try_realloc(buf_t(T) * b, uint32_t new_size)                    \
    {                                                                          \
        T* new_buf = realloc(b->buf, sizeof(*new_buf) * new_size);             \
        if (new_buf == NULL) {                                                 \
            return -1;                                                         \
        }                                                                      \
        b->buf = new_buf;                                                      \
        b->alloced = new_size;                                                 \
        return 0;                                                              \
    }                                                                          \
                                                                               \
    T* buf_more(T)(buf_t(T) * b)                                               \
    {                                                                          \
        if (b->used >= b->alloced) {                                           \
            if (try_realloc(b, b->alloced << 1) != 0) {                        \
                return NULL;                                                   \
            }                                                                  \
        }                                                                      \
        return &b->buf[b->used++];                                             \
    }                                                                          \
                                                                               \
    int buf_shrink_to_fit(T)(buf_t(T) * b) { return try_realloc(b, b->used); } \
                                                                               \
    int buf_reserve(T)(buf_t(T) * b, uint32_t to_reserve)                      \
    {                                                                          \
        if (to_reserve > b->alloced) {                                         \
            return try_realloc(b, to_reserve);                                 \
        }                                                                      \
        return 1;                                                              \
    }                                                                          \
                                                                               \
    T* buf_get(T)(buf_t(T) * b, uint32_t index) { return &b->buf[index]; }     \
                                                                               \
    T buf_get_copy(T)(buf_t(T) * b, uint32_t index) { return b->buf[index]; }  \
                                                                               \
    T* buf_begin(T)(buf_t(T) * b) { return b->buf; }                           \
                                                                               \
    T* buf_end(T)(buf_t(T) * b) { return &b->buf[b->used]; }                   \
                                                                               \
    uint32_t buf_index_from_ptr(T)(buf_t(T) * b, T * p) { return p - b->buf; } \
    uint32_t buf_size(T)(buf_t(T) * b) { return b->used; }

#endif
