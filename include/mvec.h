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

// Use this typedef when the elements' type is irrelevant
typedef void mvec_t;

// Use this macro when declaring new vectors, e.g.:
//  mvdef int* iv = mvalloc(12, sizeof(*iv));
// This macro technically does nothing so you don't have to worry about (not)
// using it but still can improve the readability
#define mvdef

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

// Returns capacity of given mvec. Consider modifying it via mvresize().
// UB: mvec == NULL or address of not a valid mvector
static inline size_t mvcap(mvec_t* mvec) {
    return ((size_t*)mvec)[-3];
}

// Returns address of length of the given mvec.
// UB:
//  @ mvec == NULL or address of not a valid mvector
//  @ Setting length larger than capacity
static inline size_t* mvlen(mvec_t* mvec) {
    return (size_t*)mvec - 2;
}

// Returns size of the elements of given mvec in bytes.
// UB: mvec == NULL or address of not a valid mvector
static inline size_t mvelsz(mvec_t* mvec) {
    return ((size_t*)mvec)[-1];
}

// Returns the whole physical size of a given mvec in bytes.
// UB: mvec == NULL or address of not a valid mvector
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

// Allocates a new mvec with given capacity of elements with element_size each.
// Sets length to 0. On success, returns a pointer to newly allocated mvec. On
// failure, returns NULL.
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

// Reallocates given mvec to the given new_capacity. If length exceeds
// new_capacity, it gets leveled to it and all the trimmed data is lost. On
// success, returns a pointer to reallocated mvec; its pointer's previous value
// gets invalidated. On failure, returns NULL; the state and the data of the
// given mvec remains untouched.
// UB: mvec == NULL or address of not a valid mvector
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

// Allocates a new vector with given new_capacity and copies elements from a
// given mvec. If given mvec's length exceeds new_capacity, it gets leveled to
// it in a new vector and all the trimmed data is not copied to it. The state
// and the data of the original mvec remains untouched. On success, returns a
// pointer to newly allocated mvec. On failure, returns NULL.
// UB: mvec == NULL or address of not a valid mvector
mvec_t* mvcopy(mvec_t* mvec, size_t new_capacity) {
    mvec_t* new_mvec = mvalloc(new_capacity, mvelsz(mvec));
    if (!new_mvec) return NULL;
    size_t new_mv_len = *mvlen(mvec) > new_capacity
        ? new_capacity : *mvlen(mvec);
    *mvlen(new_mvec) = new_mv_len;
    memcpy(new_mvec, mvec, new_mv_len * mvelsz(mvec));
}

// Shifts all the elements of the given mvec starting from the given index
// till the logical end of the vector with the given offset so mvec[index]
// becomes mvec[index+offset], mvec[index+1] -> mvec[index+1+offset], ...,
// mvec[*mvlen(mvec)-1] -> mvec[*mvlen(mvec)-1+offset]. The offset can be
// negative so you can shift elements in both directions. When shifting towards
// vector's start, all the data that was stored at
// mvec[index+offset]..mvec[index-1] gets overwritten. When shifting towards
// vector's end, values of the elements stored at
// mvec[index]..mvec[index+offset-1] are implementation-defined (most likely
// remain the same). The function does not resize the given vector.
// UB:
//  @ mvec == NULL or address of not a valid mvector
//  @ !(0 <= index && index < *mvlen(mvec))
//  @ index+offset < 0
//  @ *mvlen(mvec)+offset > mvcap(mvec)
void mvshift(mvec_t* mvec, size_t index, ptrdiff_t offset) {
    memmove(
            (char*)mvec + (index + offset) * mvelsz(mvec),
            (char*)mvec + index * mvelsz(mvec),
            (*mvlen(mvec) - index) * mvelsz(mvec)
           );
    *mvlen(mvec) += offset;
}

// Deallocates given mvec. Note that you cannot use your allocator's free()
// function directly on the mvector since when you allocate it you don't get
// the pointer to the physical head of the structure but rather pointer to
// the first element (i.e. physical head + MVEC_HEADER_SIZE_BYTES).
// UB: mvec == NULL or address of not a valid mvector
void mvfree(mvec_t* mvec) {
    free(mvhead(mvec));
}

#endif // MVEC_IMPLEMENTATION
#endif // !MVEC_H
