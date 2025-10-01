#include <assert.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"
#include <stdio.h>

int main(void) {
    int* iv = mvalloc(23, sizeof(int));
    assert(iv);
    assert(mvelsz(iv));
    assert(mvcap(iv) == 23);
    for (size_t i = 0; i < 10; i++) {
        iv[i] = i+11;
        *mvlen(iv) += 1;
    }
    assert(*mvlen(iv) == 10);
    assert(iv[9] == 20);

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
