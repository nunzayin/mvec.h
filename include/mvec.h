#ifndef MVEC_H
#define MVEC_H

#include <stddef.h> // size_t, ptrdiff_t

static const size_t MVEC_HEADER_SIZE = 2 * sizeof(size_t);

void* mvec_alloc(size_t capacity, size_t element_size);
int mvec_resize(
        void** mvec_ptr, size_t new_capacity, size_t element_size
        );
void mvec_shift(
        void* mvec, size_t index, ptrdiff_t offset, size_t element_size
        );

static inline size_t* mvec_capacity(void* mvec) {
    return mvec;
}

static inline size_t* mvec_length(void* mvec) {
    return (size_t*)mvec + 1;
}

static inline void* mvec_data(void* mvec) {
    return (size_t*)mvec + 2;
}

static inline void mvec_ptrs(
        void* mvec,
        size_t** capacity_ptr, size_t** length_ptr, void** data_ptr
        )
{
    if (capacity_ptr) *capacity_ptr = mvec_capacity(mvec);
    if (length_ptr) *length_ptr = mvec_length(mvec);
    if (data_ptr) *data_ptr = mvec_data(mvec);
}

static inline size_t mvec_size(void* mvec, size_t element_size) {
    return MVEC_HEADER_SIZE + *mvec_capacity(mvec) * element_size;
}

#ifdef MVEC_IMPLEMENTATION
#undef MVEC_IMPLEMENTATION

#include <stdlib.h> // malloc, realloc
#include <string.h> // memmove

void* mvec_alloc(size_t capacity, size_t element_size) {
    void* mvec = malloc(MVEC_HEADER_SIZE + capacity * element_size);
    if (mvec) {
        *mvec_capacity(mvec) = capacity;
        *mvec_length(mvec) = 0;
    }
    return mvec;
}

int mvec_resize(
        void** mvec_ptr, size_t new_capacity, size_t element_size
        )
{
    void* new_mvec = realloc(
            *mvec_ptr,
            MVEC_HEADER_SIZE + new_capacity * element_size
            );
    if (!new_mvec) return 1;
    *mvec_ptr = new_mvec;
    *mvec_capacity(new_mvec) = new_capacity;
    size_t* mv_len = mvec_length(new_mvec);
    if (*mv_len > new_capacity) *mv_len = new_capacity;
    return 0;
}

void mvec_shift(
        void* mvec, size_t index, ptrdiff_t offset, size_t element_size
        )
{
    char* mv_data = mvec_data(mvec);
    size_t* mv_len = mvec_length(mvec);
    memmove(
            mv_data + (index + offset) * element_size,
            mv_data + index * element_size,
            (*mv_len - index) * element_size
           );
    *mv_len += offset;
}

#endif // MVEC_IMPLEMENTATION
#endif // !MVEC_H
