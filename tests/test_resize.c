#include <assert.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"
#include <stdio.h>

int main(void) {
    mvec_t* int_vec = mvec_alloc(10, sizeof(int));
    assert(int_vec);
    mvec_t* initial_ptr = int_vec;
    size_t* ic;
    size_t* il;
    int* iv;
    mvec_ptrs(int_vec, &ic, &il, (void**)&iv);

    for (size_t i = 0; i < 32; i++) {
        if (*il == *ic) {
            assert(!mvec_resize(&int_vec, *ic * 3 / 2, sizeof(int)));
            mvec_ptrs(int_vec, &ic, &il, (void**)&iv);
            fprintf(stderr, "mvec resized to capacity %zu\n", *ic);
        }
        iv[*il] = 32-i;
        *il += 1;
    }
    assert(*ic == 33);

    fprintf(
            stderr,
            "Initial pointer: %p\n"
            "Current pointer: %p\n",
            initial_ptr, int_vec
            );

    for (size_t i = 0; i < *il; i++)
        fprintf(stderr, "%d ", iv[i]);
    fprintf(stderr, "\n");

    free(int_vec);
}
