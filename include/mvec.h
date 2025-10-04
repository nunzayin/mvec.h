// mvec.h - Monolithic vector

/* MIT License

Copyright (c) 2025 nunzayin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#ifndef MVEC_H
#define MVEC_H

#include <stddef.h> // size_t, ptrdiff_t

// Use this typedef when the elements' type is irrelevant
typedef void mvec_t;

// Use this macro when declaring new vectors, e.g.:
//  mvdef int* iv = mvalloc(12, sizeof(int));
// This macro technically does nothing so you don't have to worry about (not)
// using it but still can improve the readability
#define mvdef

#ifdef MVEC_CUSTOM_ALLOCATORS
// Types for allocator function pointers
typedef void* (*allocfunc_t)(size_t);
typedef void* (*reallocfunc_t)(void*, size_t);
typedef void (*freefunc_t)(void*);
#endif // MVEC_CUSTOM_ALLOCATORS

#ifdef MVEC_CUSTOM_MEMFUNCS
// Types for custom memory functions
typedef void* (*memcpyfunc_t)(
        void* restrict dest,
        const void* restrict src,
        size_t bytes
        );
typedef void* (*memmovefunc_t)(void* dest, const void* src, size_t bytes);
#endif // MVEC_CUSTOM_MEMFUNCS

// Mvec's header differs in size depending on whether custom allocator support
// is enabled or disabled
typedef struct mvec_header_t {
#ifdef MVEC_CUSTOM_ALLOCATORS
    reallocfunc_t realloc;
    freefunc_t free;
#endif // MVEC_CUSTOM_ALLOCATORS
    size_t length;
    size_t capacity;
    size_t element_size;
} MvecHeader;

// Functions cheatsheet. Check out their implementations for detailed
// descriptions

mvec_t* mvalloc(size_t capacity, size_t element_size);
mvec_t* mvresize(mvec_t* mvec, size_t new_capacity);
mvec_t* mvcopy(mvec_t* mvec, size_t new_capacity);
void mvshift(mvec_t* mvec, size_t index, ptrdiff_t offset);
void mvfree(mvec_t* mvec);
static inline size_t* mvlen(mvec_t* mvec);
static inline size_t mvcap(mvec_t* mvec);
static inline size_t mvelsz(mvec_t* mvec);
static inline size_t mvsize(mvec_t* mvec);

mvec_t* mvec_fromNormal(void* data, size_t length, size_t element_size);
void* mvec_toNormal(
        mvec_t* mvec,
        size_t* length_out,
        size_t* element_size_out
        );

static inline MvecHeader* mvhead(mvec_t* mvec);

#ifdef MVEC_CUSTOM_ALLOCATORS
void mvec_setAllocator(
        allocfunc_t malloc_f, reallocfunc_t realloc_f, freefunc_t free_f
        );
#endif // MVEC_CUSTOM_ALLOCATORS

#ifdef MVEC_CUSTOM_MEMFUNCS
void mvec_setMemcpy(memcpyfunc_t memcpy_f);
void mvec_setMemmove(memmovefunc_t memmove_f);
#endif // MVEC_CUSTOM_MEMFUNCS

// Returns address of length of the given mvec.
// UB:
//  @ mvec == NULL or address of not a valid mvector
//  @ Setting length larger than capacity
static inline size_t* mvlen(mvec_t* mvec) {
    return &mvhead(mvec)->length;
}

// Returns capacity of given mvec. Consider modifying it via mvresize().
// UB: mvec == NULL or address of not a valid mvector
static inline size_t mvcap(mvec_t* mvec) {
    return mvhead(mvec)->capacity;
}

// Returns size of a single element of given mvec in bytes.
// UB: mvec == NULL or address of not a valid mvector
static inline size_t mvelsz(mvec_t* mvec) {
    return mvhead(mvec)->element_size;
}

// Returns the whole physical size of a given mvec in bytes.
// UB: mvec == NULL or address of not a valid mvector
static inline size_t mvsize(mvec_t* mvec) {
    return sizeof(MvecHeader) + mvcap(mvec) * mvelsz(mvec);
}

// Returns a pointer to the header of a given mvec. Use it if you know what you
// do. Here be dragons.
// UB: mvec == NULL or address of not a valid mvector
static inline MvecHeader* mvhead(mvec_t* mvec) {
    return (MvecHeader*)mvec - 1;
}

#ifdef MVEC_IMPLEMENTATION
#undef MVEC_IMPLEMENTATION

#ifndef MVEC_CUSTOM_ALLOCATORS
#include <stdlib.h> // malloc, realloc, free
#endif // !MVEC_CUSTOM_ALLOCATORS

#ifdef MVEC_CUSTOM_MEMFUNCS
#define MVEC_MEMCPY_FUNCTION mvec_current_memcpy
#define MVEC_MEMMOVE_FUNCTION mvec_current_memmove
#else // !MVEC_CUSTOM_MEMFUNCS
#include <string.h> // memcpy, memmove
#define MVEC_MEMCPY_FUNCTION memcpy
#define MVEC_MEMMOVE_FUNCTION memmove
#endif // MVEC_CUSTOM_MEMFUNCS

#ifdef MVEC_CUSTOM_ALLOCATORS
static struct allocator {
    allocfunc_t malloc;
    reallocfunc_t realloc;
    freefunc_t free;
} mvec_current_allocator = {0};

// Set the library's current allocator. Its functions will be used in the
// library functions that perform (re)allocating/freeing memory. Remember:
// vectors store function pointers of the allocator they were allocated with.
// UB:
//  @ any of the arguments are NULL or invalid function pointers
//  @ given function pointers refer to different allocators
//  @ functions' semantics are not the same as the ones of stdlib's allocator
void mvec_setAllocator(
        allocfunc_t malloc_f, reallocfunc_t realloc_f, freefunc_t free_f
        )
{
    mvec_current_allocator.malloc = malloc_f;
    mvec_current_allocator.realloc = realloc_f;
    mvec_current_allocator.free = free_f;
}
#endif // MVEC_CUSTOM_ALLOCATORS

#ifdef MVEC_CUSTOM_MEMFUNCS
static memcpyfunc_t mvec_current_memcpy = NULL;
static memmovefunc_t mvec_current_memmove = NULL;

// Set the library's current memcpy() implementation.
// UB:
//  @ memcpy_f == NULL or invalid function pointer
//  @ function's semantic is not the same as the one of string.h's memcpy()
void mvec_setMemcpy(memcpyfunc_t memcpy_f) {
    mvec_current_memcpy = memcpy_f;
}

// Set the library's current memmove() implementation.
// UB:
//  @ memmove_f == NULL or invalid function pointer
//  @ function's semantic is not the same as the one of string.h's memmove()
void mvec_setMemmove(memmovefunc_t memmove_f) {
    mvec_current_memmove = memmove_f;
}
#endif // MVEC_CUSTOM_MEMFUNCS

static inline mvec_t* mvec_fromHeader(MvecHeader* mvec_header) {
    return mvec_header + 1;
}

// Allocates a new mvec with given capacity of elements with element_size each.
// Sets length to 0. [Uses current allocator and stores its realloc() and
// free() function pointers in the newly allocated mvec's header]. On success,
// returns a pointer to newly allocated mvec. On failure, returns NULL.
// UB:
//  @ reading from returned vector's contents before initialization
//  @ accessing vector's contents beyond its capacity
//  [@ current allocator is not set with mvec_setAllocator()]
mvec_t* mvalloc(size_t capacity, size_t element_size) {
    MvecHeader* head =
#ifdef MVEC_CUSTOM_ALLOCATORS
        mvec_current_allocator.
#endif // MVEC_CUSTOM_ALLOCATORS
        malloc(sizeof(MvecHeader) + capacity * element_size);
    if (!head) return NULL;
    mvec_t* mvec = mvec_fromHeader(head);
#ifdef MVEC_CUSTOM_ALLOCATORS
    mvhead(mvec)->realloc = mvec_current_allocator.realloc;
    mvhead(mvec)->free = mvec_current_allocator.free;
#endif // MVEC_CUSTOM_ALLOCATORS
    *mvlen(mvec) = 0;
    mvhead(mvec)->capacity = capacity;
    mvhead(mvec)->element_size = element_size;
    return mvec;
}

// Reallocates given mvec to the given new_capacity [using realloc() function
// pointer stored in mvec]. If length exceeds new_capacity, it gets leveled
// to it and all the trimmed data is lost. On success, returns a pointer to
// reallocated mvec; its pointer's previous value gets invalidated. On failure,
// returns NULL; the state and the data of the given mvec remains untouched.
// UB: mvec == NULL or address of not a valid mvector
mvec_t* mvresize(mvec_t* mvec, size_t new_capacity) {
    MvecHeader* new_head =
#ifdef MVEC_CUSTOM_ALLOCATORS
        mvhead(mvec)->
#endif // MVEC_CUSTOM_ALLOCATORS
        realloc(
            mvhead(mvec),
            sizeof(MvecHeader) + new_capacity * mvelsz(mvec)
            );
    if (!new_head) return NULL;
    mvec_t* new_mvec = mvec_fromHeader(new_head);
    mvhead(new_mvec)->capacity = new_capacity;
    if (*mvlen(new_mvec) > new_capacity) *mvlen(new_mvec) = new_capacity;
    return new_mvec;
}

// Allocates a new vector [using current allocator] with given new_capacity and
// copies elements from a given mvec [using current memcpy() function]. If
// given mvec's length exceeds new_capacity, it gets leveled to it in a new
// vector and all the trimmed data is not copied to it. The state and the data
// of the original mvec remains untouched. On success, returns a pointer to
// newly allocated mvec. On failure, returns NULL.
// UB:
//  @ mvec == NULL or address of not a valid mvector
//  [@ current allocator is not set with mvec_setAllocator()]
//  [@ current memcpy() function is not set with mvec_setMemcpy()]
mvec_t* mvcopy(mvec_t* mvec, size_t new_capacity) {
    mvec_t* new_mvec = mvalloc(new_capacity, mvelsz(mvec));
    if (!new_mvec) return NULL;
    size_t new_mv_len = *mvlen(mvec) > new_capacity
        ? new_capacity : *mvlen(mvec);
    *mvlen(new_mvec) = new_mv_len;
    MVEC_MEMCPY_FUNCTION(new_mvec, mvec, new_mv_len * mvelsz(mvec));
}

// Shifts all the elements of the given mvec starting from the given index
// till the logical end of the vector with the given offset so mvec[index]
// becomes mvec[index+offset], mvec[index+1] -> mvec[index+1+offset], ...,
// mvec[*mvlen(mvec)-1] -> mvec[*mvlen(mvec)-1+offset]. [Uses currently
// installed memmove()]. The offset can be negative so you can shift elements
// in both directions. When shifting towards vector's start, all the data that
// was stored at mvec[index+offset]..mvec[index-1] gets overwritten. When
// shifting towards vector's end, values of the elements stored at
// mvec[index]..mvec[index+offset-1] are implementation-defined (most likely
// remain the same). The function does not resize the given vector. Adds offset
// to the length.
// UB:
//  @ mvec == NULL or address of not a valid mvector
//  @ !(0 <= index && index < *mvlen(mvec))
//  @ index+offset < 0
//  @ *mvlen(mvec)+offset > mvcap(mvec)
//  [@ current memmove() function is not set with mvec_setMemmove()]
void mvshift(mvec_t* mvec, size_t index, ptrdiff_t offset) {
    MVEC_MEMMOVE_FUNCTION(
            (char*)mvec + (index + offset) * mvelsz(mvec),
            (char*)mvec + index * mvelsz(mvec),
            (*mvlen(mvec) - index) * mvelsz(mvec)
           );
    *mvlen(mvec) += offset;
}

// Deallocates given mvec [with the free() function pointer stored in the
// mvec's header]. Note that you cannot use [your allocator's] free() function
// directly on the mvector since when you allocate it you don't get the pointer
// to the physical head of the structure but rather pointer to the first
// element (i.e. physical head + sizeof(MvecHeader)).
// UB: mvec == NULL or address of not a valid mvector
void mvfree(mvec_t* mvec) {
#ifdef MVEC_CUSTOM_ALLOCATORS
    mvhead(mvec)->
#endif // MVEC_CUSTOM_ALLOCATORS
    free(mvhead(mvec));
}

// Converts dynamically allocated [with currently installed allocator] memory
// chunk of given quantity of elements with given element_size in bytes each
// into a monolithic vector. Sets its length and capacity to the given
// quantity. [Stores current allocator's realloc() and free() function pointers
// in mvec's header]. [Uses currently installed memmove() implementation]. On
// success, returns pointer to resulting mvec; the former pointer to the data
// gets invalidated. On failure, returns NULL; all the data remains in the same
// state as before calling the function.
// UB: data is not NULL yet still not a pointer returned from [current
// allocator's] malloc()/realloc() function
// Note: quantity and element_size may not correspond to real size of given
// data - these arguments technically work the same as capacity and
// element_size for mvalloc() except both mvec's length and capacity are set to
// given quantity.
mvec_t* mvec_fromNormal(void* data, size_t quantity, size_t element_size) {
    void* head =
#ifdef MVEC_CUSTOM_ALLOCATORS
        mvec_current_allocator.
#endif // MVEC_CUSTOM_ALLOCATORS
        realloc(data, sizeof(MvecHeader) + quantity * element_size);
    if (!head) return NULL;
    mvec_t* mvec = MVEC_MEMMOVE_FUNCTION(
                (char*)head + sizeof(MvecHeader),
                head,
                quantity * element_size
                );
#ifdef MVEC_CUSTOM_ALLOCATORS
    mvhead(mvec)->realloc = mvec_current_allocator.realloc;
    mvhead(mvec)->free = mvec_current_allocator.free;
#endif // MVEC_CUSTOM_ALLOCATORS
    *mvlen(mvec) = quantity;
    mvhead(mvec)->capacity = quantity;
    mvhead(mvec)->element_size = element_size;
    return mvec;
}

// Converts given mvec to a header-less chunk of memory [using realloc()
// function pointer stored in mvec's header]. Trims the vector's physical size
// to its length. [Uses currently installed memmove() implementation]. On
// success, returns pointer to resulting memory chunk; sets the quantity and
// element size at the variables addressed by given quantity_out and
// element_size_out, respectively. On failure, returns NULL; reverts all the
// changes of mvec to the state before calling the function.
// UB:
//  @ mvec == NULL or address of not a valid mvector
//  @ quantity_out is not NULL yet still invalid
//  @ element_size_out is not NULL yet still invalid
//  [@ current memmove() function is not set with mvec_setMemmove()]
void* mvec_toNormal(
        mvec_t* mvec,
        size_t* quantity_out,
        size_t* element_size_out
        )
{
    MvecHeader header_backup = *mvhead(mvec);
    void* mvec_shifted = MVEC_MEMMOVE_FUNCTION(
            mvhead(mvec),
            mvec,
            *mvlen(mvec) * mvelsz(mvec)
            );
    void* out =
#ifdef MVEC_CUSTOM_ALLOCATORS
        header_backup.
#endif // MVEC_CUSTOM_ALLOCATORS
        realloc(
            mvec_shifted,
            header_backup.length * header_backup.element_size
            );
    if (!out) {
        MVEC_MEMMOVE_FUNCTION(
            (char*)mvec_shifted + sizeof(MvecHeader),
            mvec_shifted,
            header_backup.length * header_backup.element_size
            );
        *mvhead(mvec) = header_backup;
        return NULL;
    }
    if (quantity_out) *quantity_out = header_backup.length;
    if (element_size_out) *element_size_out = header_backup.element_size;
    return out;
}

#endif // MVEC_IMPLEMENTATION
#endif // !MVEC_H
