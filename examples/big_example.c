#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Define this macro to include library implementation into this translation
// unit. You probably gonna do it once in one of the units of your program and
// then simply #include "mvec.h" in any other unit to link with the one with
// implementation.
#define MVEC_IMPLEMENTATION
#include "mvec.h"

// shitty randint implementation
static inline int randint(int upper_bound) {
    return rand() % upper_bound;
}

static int int_comp(const void* _a, const void* _b) {
    int a = *(const int*)_a;
    int b = *(const int*)_b;
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

int main(void) {
    // Here's how we declare and initialize a new mvec. `mvdef` does nothing
    // but consider leaving it at mvec type declarations to hint that this
    // pointer is gonna be a pointer to mvec.
    mvdef int* iv = mvalloc(16, sizeof(int));
    // Don't forget to check the results. mvalloc() returns NULL on failure.
    assert(iv);

    // Simply treat the returned pointer as a normal array in terms of
    // accessing its elements.
    iv[0] = 1337;
    // Call mvlen() on the iv to gain access to its length. It returns a
    // pointer which you can use both for reading and writing. Remember: when
    // your length exceeds the capacity, consider resizing the vector,
    // otherwise it's UB.
    *mvlen(iv) += 1;

    fprintf(stderr,
            "Current capacity = %zu\n"
            "Current length = %zu\n"
            "Size of a single element (bytes): %zu\n",
            // Note that mvec's capacity and element size are kinda read-only.
            // It is made to prevent error-prone usage of these fields from
            // being frequent case. Consider resizing mvec (i.e. changing its
            // capacity) via mvresize(). Changing element size is not supported
            // at all. However, you can access mvec's header directly via
            // mvhead() if you know what you do.
            mvcap(iv),
            *mvlen(iv),
            mvelsz(iv)
            );

    // Let's fill the vector up with some values. This is gonna be 1337, 1338,
    // 1339, etc.
    for (int i = iv[0] + 1; *mvlen(iv) < mvcap(iv); i++)
        iv[(*mvlen(iv))++] = i;

    fprintf(stderr,
            "First element [0]: %d\n"
            "Last element [%zu]: %d\n",
            iv[0],
            *mvlen(iv) - 1,
            iv[*mvlen(iv) - 1]
           );

    // You can easily check if the vector is empty by comparing its length
    // with zero.
    bool iv_empty = *mvlen(iv) == 0;

    // Let's implement appending. When the length reaches capacity, we resize
    // the vector by doubling its capacity.
    for (size_t i = 1; i <= 5; i++) {
        if (*mvlen(iv) == mvcap(iv)) {
            // Avoid assigning the result of mvresize() directly to the
            // original pointer. mvresize() returns NULL on failure and if so,
            // the original pointer does not get invalidated.
            mvdef int* new_iv = mvresize(iv, mvcap(iv) * 2);
            // Only after checking that new_iv is actually a thing, we can
            // be sure that mvresize() has done its work...
            assert(new_iv);
            // ...so we can assign its value to the original pointer.
            iv = new_iv;
        }
        iv[(*mvlen(iv))++] = iv[15] + i;
    }

    // Let's say we need to shrink the vector to its logical length. We can
    // also use mvresize() since it supports reallocating to any capacity (even
    // zero - try to figure out why).
    if (*mvlen(iv) < mvcap(iv)) {

        mvdef int* new_iv = mvresize(iv, *mvlen(iv));
        assert(new_iv);
        iv = new_iv;
    }

    *mvlen(iv) = 0;

    assert(mvcap(iv) >= 5);
    for (size_t i = 0; i < 5; i++)
        iv[(*mvlen(iv))++] = i+1;
    assert(*mvlen(iv) + 2 <= mvcap(iv));
    mvshift(iv, 3, +1);
    iv[3] = 1337;
    iv[(*mvlen(iv))++] = iv[0];
    mvshift(iv, 1, -1);
    fputs("Current iv contents: ", stderr);
    for (size_t i = 0; i < *mvlen(iv); i++)
        fprintf(stderr, "%d ", iv[i]);
    fputc('\n', stderr);

    memset(iv, 0, mvcap(iv) * mvelsz(iv));
    for (size_t i = 0; i < mvcap(iv); i++)
        assert(iv[i] == 0);

    if (mvcap(iv) < 16) {
        mvdef int* new_iv = mvresize(iv, 16);
        assert(new_iv);
        iv = new_iv;
    }
    *mvlen(iv) = 0;
    for (size_t i = 0; i < 16; i++)
        iv[(*mvlen(iv))++] = randint(16);
    qsort(iv, *mvlen(iv), mvelsz(iv), int_comp);
    fputs("Current iv contents: ", stderr);
    for (size_t i = 0; i < *mvlen(iv); i++)
        fprintf(stderr, "%d ", iv[i]);
    fputc('\n', stderr);

    mvdef int* another_iv = mvcopy(iv, mvcap(iv));
    assert(another_iv);
    fputs("Copied and modified iv contents: ", stderr);
    for (size_t i = 0; i < *mvlen(iv); i++)
        fprintf(stderr, "%d ", ++another_iv[i]);
    fputc('\n', stderr);

    int a[] = {42, 43, 44, 45};
    size_t al = 4;
    for (size_t i = 0; i < al; i++)
        iv[i] = a[i];
    memcpy(&iv[al], a, al * sizeof(int));
    *mvlen(iv) = 2 * al;
    fputs("Current iv contents: ", stderr);
    for (size_t i = 0; i < *mvlen(iv); i++)
        fprintf(stderr, "%d ", iv[i]);
    fputc('\n', stderr);

    mvfree(iv);
    mvfree(another_iv);
}
