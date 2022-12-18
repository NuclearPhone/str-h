#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef STR_INLINE
#define STR_EXPORT static inline
#else
#define STR_EXPORT extern
#endif

typedef struct {
    char* ptr;

    uint32_t capacity;
    uint32_t len;
} str_t;

STR_EXPORT str_t str_create();
STR_EXPORT str_t str_from_cstr_move(char**);
STR_EXPORT str_t str_from_cstr_clone(const char*);
STR_EXPORT const char* str_cstr(const str_t*);
STR_EXPORT void str_destroy(str_t*);
STR_EXPORT bool str_cmp(const str_t*, const str_t*);
STR_EXPORT void str_append_char(str_t*, char);
STR_EXPORT void str_append(str_t*, str_t* from);
STR_EXPORT void str_insert_char(str_t*, char, uint32_t idx);
STR_EXPORT void str_insert(str_t*, str_t* from, uint32_t idx);
STR_EXPORT char str_pop(str_t*);

#ifndef DEFAULT_STR_CAPACITY
#define DEFAULT_STR_CAPACITY 32
#endif

#ifndef DEFAULT_STR_GROWTH_FACTOR
#define DEFAULT_STR_GROWTH_FACTOR 1.5f
#endif

#ifdef STR_IMPLEMENTATION

#define STR_UNUSED(x) (void)x

static inline uint32_t str_cstrlen(const char* in) {
    uint32_t len = 0;
    while (in[len] != 0)
        len += 1;
    return len;
}

// continuously resizes a strings capacity until it is >= than fit_cap
static inline void str_fit(str_t* str, uint32_t fit_cap) {
    uint32_t cap = str->capacity;

    while (cap <= fit_cap) {
        cap *= DEFAULT_STR_GROWTH_FACTOR;
    }

    str->capacity = cap;
    str->ptr = realloc(str->ptr, cap);
}

STR_EXPORT str_t str_create() {
    const str_t out = {
        .ptr = malloc(DEFAULT_STR_CAPACITY),
        .capacity = DEFAULT_STR_CAPACITY,
        .len = 0,
    };

    return out;
}

STR_EXPORT const char* str_cstr(const str_t* in) {
    if (in->ptr[in->len] != 0)
        in->ptr[in->len] = 0;

    return in->ptr;
}

// clones the cstr
STR_EXPORT str_t str_from_cstr_clone(const char* in) {
    const uint32_t len = str_cstrlen(in);

    char* clone = malloc(len);
    for (uint32_t i = 0; i < len; i++)
        clone[i] = in[i];

    const str_t out = {
        .ptr = clone,
        .capacity = len,
        .len = len,
    };

    return out;
}

STR_EXPORT str_t str_from_cstr_move(char** in) {
    char* cstr = *in;
    *in = NULL;

    const uint32_t len = str_cstrlen(cstr);
    const str_t out = {
        .ptr = cstr,
        .capacity = len,
        .len = len,
    };

    return out;
}

STR_EXPORT void str_destroy(str_t* str) {
    if (str->ptr)
        free(str->ptr);
}

STR_EXPORT bool str_cmp(const str_t* left, const str_t* right) {
    if (left->len != right->len)
        return false;

    for (uint32_t i = 0; i < left->len; i++)
        if (left->ptr[i] != right->ptr[i])
            return false;

    return true;
}

STR_EXPORT void str_append_char(str_t* str, char c) {
    str_fit(str, str->len + 1);
    str->ptr[str->len] = c;
    str->len += 1;
}

STR_EXPORT void str_append(str_t* str, str_t* from) {
    const uint32_t final_len = str->len + from->len;

    str_fit(str, final_len);

    for (uint32_t i = 0; i < from->len; i++)
        str->ptr[i + str->len] = from->ptr[i];

    str->len += from->len;
}

STR_EXPORT void str_insert_char(str_t* str, char c, uint32_t idx) {
    str_fit(str, str->len += 1);

    for (uint32_t i = str->len; i > idx; i--)
        str->ptr[i] = str->ptr[i - 1];

    str->ptr[idx] = c;
}

STR_EXPORT void str_insert(str_t* str, str_t* from, uint32_t idx) {
    const uint32_t final_len = str->len + from->len;

    str_fit(str, final_len);

    // move all of the characters that are displaced over
    for (uint32_t i = final_len; i >= idx; i--)
        str->ptr[i] = str->ptr[i - from->len];

    // now copy the other string into its new place
    for (uint32_t i = str->len; i < final_len; i++)
        str->ptr[i] = from->ptr[i];

    str->len = final_len;
}

#endif