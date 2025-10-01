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

// You can use void if you care to be straightforward
typedef void mvec_t;
// Or this one if you care to be informative
#define mvec_(T) mvec_t

// Size of the vector header
static const size_t MVEC_HEADER_SIZE = 2 * sizeof(size_t);

// Function prototypes are left as a dense list to provide a compact
// cheatsheet. See the implementation section for detailed descriptions.

mvec_t* mvec_alloc(size_t capacity, size_t element_size);
int mvec_resize(mvec_t** mvec_ptr, size_t new_capacity, size_t element_size);
void mvec_shift(
        mvec_t* mvec, size_t index, ptrdiff_t offset, size_t element_size
        );
static inline size_t* mvec_capacity(mvec_t* mvec);
static inline size_t* mvec_length(mvec_t* mvec);
static inline void* mvec_data(mvec_t* mvec);
static inline void mvec_ptrs(
        mvec_t* mvec,
        size_t** capacity_ptr, size_t** length_ptr, void** data_ptr
        );
static inline size_t mvec_size(mvec_t* mvec, size_t element_size);

// Inline functions implementation

// Returns a pointer to vector's capacity.
// Parameters:
//  @ mvec - pointer to the monolithic vector
// Return value:
//  Pointer to the mvec's capacity
// UB conditions:
//  @ mvec == NULL or address of not a valid vector
//  @ modifying capacity manually (consider using mvec_resize() instead)
static inline size_t* mvec_capacity(mvec_t* mvec) {
    return mvec;
}

// Returns a pointer to vector's length.
// Parameters:
//  @ mvec - pointer to the monolithic vector
// Return value:
//  Pointer to the mvec's length
// UB conditions:
//  @ mvec == NULL or address of not a valid vector
//  @ setting length larger than mvec's capacity
static inline size_t* mvec_length(mvec_t* mvec) {
    return (size_t*)mvec + 1;
}

// Returns a pointer to the start of the vector's data (i.e. the 0th element).
// Parameters:
//  @ mvec - pointer to the monolithic vector
// Return value:
//  Pointer to the start of the vector's data (i.e. the 0th element).
// UB conditions:
//  @ mvec == NULL or address of not a valid vector
// Note:
//  Avoid inconsistent pointer typecasting (i.e. treating vector of some type
//  T1 as a vector of some other type T2, especially when sizeof(T1) is less
//  than sizeof(T2)). It CAN produce UB if you are not careful but also can be
//  useful if you discard current vector's elements and gonna use it for a new
//  set of elements of another type.
static inline void* mvec_data(mvec_t* mvec) {
    return (size_t*)mvec + 2;
}

// Passes all the mvec's characteristics to correspoinding pointers. For each
// address of a pointer, if address is NULL, it's ignored.
// Parameters:
//  @ mvec - pointer to the monolithic vector
//  @ capacity_ptr - address of a pointer to mvec's capacity to pass to
//  @ length_ptr - address of a pointer to mvec's length to pass to
//  @ data_ptr - address of a pointer to the start of the mvec's data to pass
//  to
// Return value: (none)
// UB conditions:
//  @ mvec == NULL or address of not a valid vector
//  @ capacity_ptr is not a valid pointer to size_t*
//  @ length_ptr is not a valid pointer to size_t*
//  @ data_ptr is not a valid pointer to void*
// Note:
//  Use this function as a safe shorthand for calling mvec_capacity(),
//  mvec_length() and mvec_data() all at once for necessary parameters only.
static inline void mvec_ptrs(
        mvec_t* mvec,
        size_t** capacity_ptr, size_t** length_ptr, void** data_ptr
        )
{
    if (capacity_ptr) *capacity_ptr = mvec_capacity(mvec);
    if (length_ptr) *length_ptr = mvec_length(mvec);
    if (data_ptr) *data_ptr = mvec_data(mvec);
}

// Returns physical size of a vector, i.e. number of bytes currently allocated
// for the whole data structure and its elements.
// Parameters:
//  @ mvec - pointer to the monolithic vector
//  @ element_size - size of a single element in bytes
// Return value:
//  Physical size of a vector, i.e. number of bytes currently allocated
//  for the whole data structure and its elements.
// UB conditions:
//  @ mvec == NULL or address of not a valid vector
//  @ passing the element size not equal to the one used to (re)allocate this
//  vector (will return a misleading value not corresponding to real size)
static inline size_t mvec_size(mvec_t* mvec, size_t element_size) {
    return MVEC_HEADER_SIZE + *mvec_capacity(mvec) * element_size;
}

// Define MVEC_IMPLEMENTATION before #include'ing this library to include the
// implementation of given functions to the current translation unit. Once
// you've made that, all the translation units to be linked with this one may
// not include the implementation (but still need to include the header itself
// though). Common practice for such cases is just to create a separate
// translation unit (let's call it "mvec.c") and leave there only these
// directives:
//  #define MVEC_IMPLEMENTATION
//  #include "mvec.h"
//  (EOF)
#ifdef MVEC_IMPLEMENTATION
#undef MVEC_IMPLEMENTATION

// You can use custom allocator's malloc() and realloc() functions by defining
// the corresponding macros. Note that custom functions should have the same
// semantics and argument sets as of the ones provided by <stdlib.h>. For each
// allocating function, if its custom version is not provided, <stdlib.h> is
// included in the translation unit and its corresponding function is used.
// That means partial setting of custom allocating functions is technically
// possible, though not recommended (will likely lead to UB).
#if !defined(MVEC_MALLOC_FUNCTION) || !defined(MVEC_REALLOC_FUNCTION)
#include <stdlib.h> // malloc, realloc
#ifndef MVEC_MALLOC_FUNCTION
#define MVEC_MALLOC_FUNCTION malloc
#endif // !MVEC_MALLOC_FUNCTION
#ifndef MVEC_REALLOC_FUNCTION
#define MVEC_REALLOC_FUNCTION realloc
#endif // !MVEC_REALLOC_FUNCTION
#endif // !defined(MVEC_MALLOC_FUNCTION) || !defined(MVEC_REALLOC_FUNCTION)

// You can use custom memmove() implementation by defining the corresponding
// macros. Note that custom function should have the same semantic and argument
// set as of the one provided by <string.h>. If the custom version is not
// provided, <string.h> is included in the translation unit and its
// corresponding function is used.
#ifndef MVEC_MEMMOVE_FUNCTION
#include <string.h> // memmove
#define MVEC_MEMMOVE_FUNCTION memmove
#endif // !MVEC_MEMMOVE_FUNCTION

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
int mvec_resize(mvec_t** mvec_ptr, size_t new_capacity, size_t element_size) {
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
// moves to [i + offset]. Sets length to length + offset.
// Parameters:
//  @ mvec - pointer to the monolithic vector
//  @ index - index of an element to start from
//  @ offset - the offset to shift the vector with (amount of elements, not
//  bytes)
//  @ element_size - size of a single element in bytes
// Return value: (none)
// UB conditions:
//  @ mvec == NULL or address of not a valid vector
//  @ index is out of vector logical bounds [0; length)
//  @ index + offset is out of vector memory bounds [0; capacity)
//  @ length + offset is out of vector memory bounds [0; capacity]
// Note:
//  If offset < 0, all the elements located at [index + offset] up to [index]
//  exclusive get overwrited. If offset > 0, all the elements' values located
//  at [index] up to [index + offset] exclusive are implementation-defined
//  (most likely remain the same as before calling mvec_shift()).
//  If element_size == 0, the behavior is implementation-defined (most likely
//  will do nothing).
void mvec_shift(
        mvec_t* mvec, size_t index, ptrdiff_t offset, size_t element_size
        )
{
    char* mv_data = mvec_data(mvec);
    size_t* mv_len = mvec_length(mvec);
    MVEC_MEMMOVE_FUNCTION(
            mv_data + (index + offset) * element_size,
            mv_data + index * element_size,
            (*mv_len - index) * element_size
           );
    *mv_len += offset;
}

#undef MVEC_MALLOC_FUNCTION
#undef MVEC_REALLOC_FUNCTION
#undef MVEC_MEMMOVE_FUNCTION

#endif // MVEC_IMPLEMENTATION
#endif // !MVEC_H
