#include <assert.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"
#include <stdio.h>

int main(void) {
    void* int_vec = mvec_alloc(23, sizeof(int));
    assert(int_vec);
    int* iv; size_t* il; size_t* ic;
    mvec_ptrs(int_vec, &ic, &il, (void**)&iv);
    assert(*ic == 23);
    for (size_t i = 0; i < 10; i++) {
        iv[i] = i+11;
        *il += 1;
    }
    assert(*il == 10);
    assert(iv[9] == 20);

    fprintf(
            stderr,
            "Size of size_t: %zu\n"
            "Size of int: %zu\n"
            "Address of mvec: %p\n"
            "Address of its capacity: %p\n"
            "Address of its length: %p\n"
            "Address of head of its data: %p\n"
            "Its whole size: %zu\n",
            sizeof(size_t), sizeof(int),
            int_vec, ic, il, iv, mvec_size(int_vec, sizeof(int))
            );

    free(int_vec);
}
