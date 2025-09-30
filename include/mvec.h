// mvec.h - Monolithic vector
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

// Size of the vector header
static const size_t MVEC_HEADER_SIZE = 2 * sizeof(size_t);

// Allocates memory for the vector and initializes its header with given
// capacity and zero length. The rest of the memory remains uninitialized.
// Parameters:
//  @ capacity - initial vector capacity
//  @ element_size - size of a single element in bytes
// Return value:
//  On success, returns the pointer to the newly allocated vector. To avoid
//  a memory leak, the returned pointer must be deallocated with current
//  allocator's freeing (e.g. free() for stdlib.h allocator) or reallocating
//  (e.g. realloc() for the same allocator) function.
//  On failure, returns null pointer.
// UB conditions:
//  @ Specified MVEC_MALLOC_FUNCTION does not have the same semantic and amount
//  of parameters as stdlib's malloc(size_t size_in_bytes)
// Note:
//  The vector's initial capacity as well as a single element's size can be 0
//  causing no undefined behavior since the size of the vector header is
//  constant and always non-zero.
mvec_t* mvec_alloc(size_t capacity, size_t element_size);

// Resizes (expands or shrinks) the monolithic vector previously allocated with
// mvec_alloc() to the size that is able to contain new_capacity amount of
// elements with size element_size each. If vector's current capacity is more
// than new_capacity, all the elements located starting from [new_capacity]
// are getting lost and trying to access them is UB. If vector's current
// capacity is less than new_capacity, all the newly allocated data starting
// from [capacity] is uninitialized.
// Parameters:
//  @ mvec_ptr - address of the pointer to the monolithic vector
//  @ new_capacity - the new target vector capacity to allocate of
//  @ element_size - size of a single element in bytes
// Return value:
//  On success, returns 0; the pointer to the vector located at the *mvec_ptr
//  gets overwrited and trying to access by its old value is UB.
//  On failure, returns 1; the pointer to the vector located at the *mvec_ptr
//  remains valid with untouched header and data.
// UB conditions:
//  @ mvec_ptr == NULL or address of not a valid vector pointer
//  @ *mvec_ptr == NULL or address of not a valid vector
//  @ Specified MVEC_REALLOC_FUNCTION does not have the same semantic and
//  amount of parameters as stdlib's realloc(void* ptr, size_t size_in_bytes)
// Note:
//  The vector's new capacity as well as a single element's size can be 0
//  causing no undefined behavior since the size of the vector header is
//  constant and always non-zero.
int mvec_resize(
        mvec_t** mvec_ptr, size_t new_capacity, size_t element_size
        );

void mvec_shift(
        mvec_t* mvec, size_t index, ptrdiff_t offset, size_t element_size
        );

static inline size_t* mvec_capacity(mvec_t* mvec) {
    return mvec;
}

static inline size_t* mvec_length(mvec_t* mvec) {
    return (size_t*)mvec + 1;
}

static inline void* mvec_data(mvec_t* mvec) {
    return (size_t*)mvec + 2;
}

static inline void mvec_ptrs(
        mvec_t* mvec,
        size_t** capacity_ptr, size_t** length_ptr, void** data_ptr
        )
{
    if (capacity_ptr) *capacity_ptr = mvec_capacity(mvec);
    if (length_ptr) *length_ptr = mvec_length(mvec);
    if (data_ptr) *data_ptr = mvec_data(mvec);
}

static inline size_t mvec_size(mvec_t* mvec, size_t element_size) {
    return MVEC_HEADER_SIZE + *mvec_capacity(mvec) * element_size;
}

#ifdef MVEC_IMPLEMENTATION
#undef MVEC_IMPLEMENTATION

#if !defined(MVEC_MALLOC_FUNCTION) || !defined(MVEC_REALLOC_FUNCTION)
#include <stdlib.h> // malloc, realloc
#ifndef MVEC_MALLOC_FUNCTION
#define MVEC_MALLOC_FUNCTION malloc
#endif // !MVEC_MALLOC_FUNCTION
#ifndef MVEC_REALLOC_FUNCTION
#define MVEC_REALLOC_FUNCTION realloc
#endif // !MVEC_REALLOC_FUNCTION
#endif // !defined(MVEC_MALLOC_FUNCTION) || !defined(MVEC_REALLOC_FUNCTION)

#include <string.h> // memmove

mvec_t* mvec_alloc(size_t capacity, size_t element_size) {
    mvec_t* mvec = MVEC_MALLOC_FUNCTION(
            MVEC_HEADER_SIZE + capacity * element_size
            );
    if (mvec) {
        *mvec_capacity(mvec) = capacity;
        *mvec_length(mvec) = 0;
    }
    return mvec;
}

int mvec_resize(
        mvec_t** mvec_ptr, size_t new_capacity, size_t element_size
        )
{
    mvec_t* new_mvec = MVEC_REALLOC_FUNCTION(
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

// Shifts elements starting from given [index] to the current vector [length-1]
// with the given offset. The offset can be negative so the elements can be
// shifted "left" or "right". Each element on some address index <= i < length
// moves to [i + offset].
// Parameters:
//  @ mvec - pointer to the monolithic vector
//  @ index - index of an element to start from
//  @ offset - the offset to shift the vector with (amount of elements, not
//  bytes)
//  @ element_size - size of a single element
// Return value: (none)
// UB conditions:
//  @ mvec == NULL or address of not a valid vector
//  @ index is out of vector logical bounds [0; length)
//  @ index + offset is out of vector memory bounds [0; capacity)
//  @ length + offset is out of vector memory bounds [0; capacity]
// Note:
void mvec_shift(
        mvec_t* mvec, size_t index, ptrdiff_t offset, size_t element_size
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
