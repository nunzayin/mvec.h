// mvec.h - Monolithic vector

// MIT License
//
// Copyright (c) 2025 nunzayin
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef MVEC_H
#define MVEC_H

#include <stddef.h> // size_t, ptrdiff_t

typedef void mvec_t;

static const size_t MVEC_HEADER_SIZE_BYTES = 3 * sizeof(size_t);

mvec_t* mvalloc(size_t capacity, size_t element_size);
mvec_t* mvresize(mvec_t* mvec, size_t new_capacity);
mvec_t* mvcopy(mvec_t* mvec, size_t new_capacity);
void mvshift(mvec_t* mvec, size_t index, ptrdiff_t offset);
void mvfree(mvec_t* mvec);
static inline size_t mvcap(mvec_t* mvec);
static inline size_t* mvlen(mvec_t* mvec);
static inline size_t mvelsz(mvec_t* mvec);
static inline size_t mvsize(mvec_t* mvec);

static inline size_t mvcap(mvec_t* mvec) {
    return ((size_t*)mvec)[-3];
}

static inline size_t* mvlen(mvec_t* mvec) {
    return (size_t*)mvec - 2;
}

static inline size_t mvelsz(mvec_t* mvec) {
    return ((size_t*)mvec)[-1];
}

static inline size_t mvsize(mvec_t* mvec) {
    return MVEC_HEADER_SIZE_BYTES + mvcap(mvec) * mvelsz(mvec);
}

#ifdef MVEC_IMPLEMENTATION
#undef MVEC_IMPLEMENTATION

#include <stdlib.h> // malloc, realloc, free
#include <string.h> // memcpy, memmove

static inline size_t* mvcap_ptr(mvec_t* mvec) {
    return (size_t*)mvec - 3;
}

static inline size_t* mvelsz_ptr(mvec_t* mvec) {
    return (size_t*)mvec - 1;
}

static inline mvec_t* mvdata(void* head) {
    return (size_t*)head + 3;
}

static inline void* mvhead(mvec_t* mvec) {
    return (size_t*)mvec - 3;
}

mvec_t* mvalloc(size_t capacity, size_t element_size) {
    void* head = malloc(
            MVEC_HEADER_SIZE_BYTES + capacity * element_size
            );
    if (!head) return NULL;
    mvec_t* mvec = mvdata(head);
    *mvcap_ptr(mvec) = capacity;
    *mvlen(mvec) = 0;
    *mvelsz_ptr(mvec) = element_size;
    return mvec;
}

mvec_t* mvresize(mvec_t* mvec, size_t new_capacity) {
    void* new_head = realloc(
            mvhead(mvec),
            MVEC_HEADER_SIZE_BYTES + new_capacity * mvelsz(mvec)
            );
    if (!new_head) return NULL;
    mvec_t* new_mvec = mvdata(new_head);
    *mvcap_ptr(new_mvec) = new_capacity;
    if (*mvlen(new_mvec) > new_capacity) *mvlen(new_mvec) = new_capacity;
    return new_mvec;
}

mvec_t* mvcopy(mvec_t* mvec, size_t new_capacity) {
    mvec_t* new_mvec = mvalloc(new_capacity, mvelsz(mvec));
    if (!new_mvec) return NULL;
    size_t new_mv_len = *mvlen(mvec) > new_capacity
        ? new_capacity : *mvlen(mvec);
    *mvlen(new_mvec) = new_mv_len;
    memcpy(new_mvec, mvec, new_mv_len * mvelsz(mvec));
}

void mvshift(mvec_t* mvec, size_t index, ptrdiff_t offset) {
    memmove(
            (char*)mvec + (index + offset) * mvelsz(mvec),
            (char*)mvec + index * mvelsz(mvec),
            (*mvlen(mvec) - index) * mvelsz(mvec)
           );
    *mvlen(mvec) += offset;
}

void mvfree(mvec_t* mvec) {
    free(mvhead(mvec));
}

#endif // MVEC_IMPLEMENTATION
#endif // !MVEC_H
