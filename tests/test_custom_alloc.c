#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#define MVEC_CUSTOM_ALLOCATORS
#define MVEC_IMPLEMENTATION
#include "mvec.h"

int main(void) {
    mvec_setAllocator(malloc, realloc, free);
    mvdef int* iv = mvalloc(23, sizeof(int));
    assert(iv);
    assert(mvelsz(iv));
    assert(mvcap(iv) == 23);
    for (size_t i = 0; i < 10; i++) {
        iv[i] = i+11;
        *mvlen(iv) += 1;
    }
    assert(*mvlen(iv) == 10);
    assert(iv[9] == 20);

    for (size_t i = 0; i < 32; i++) {
        if (*mvlen(iv) == mvcap(iv)) {
            mvdef int* new_iv;
            assert((new_iv = mvresize(iv, mvcap(iv) * 3 / 2)));
            iv = new_iv;
            fprintf(stderr, "mvec resized to capacity %zu\n", mvcap(iv));
        }
        iv[*mvlen(iv)] = 32-i;
        *mvlen(iv) += 1;
    }

    fprintf(
            stderr,
            "Size of size_t: %zu\n"
            "Size of int: %zu\n"
            "Address of mvec: %p\n"
            "Its capacity: %zu\n"
            "Address of its length: %p\n"
            "Its whole size: %zu\n",
            sizeof(size_t), sizeof(int),
            iv, mvcap(iv), mvlen(iv), mvsize(iv)
            );

    mvfree(iv);
}
