#include <stdio.h>
#include <assert.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"

int main(void) {
    mvec_t* int_vec = mvec_alloc(10, sizeof(int));
    assert(int_vec);
    int* iv; size_t* il; size_t* ic;
    mvec_ptrs(int_vec, &ic, &il, (void**)&iv);

    // Let's try to use mvec as a queue.
    // We'll prepend a new element to enqueue and
    // take the last element to dequeue.

    for (size_t i = 0; i < 20; i++) {
        if (*il == *ic) {
            assert(!mvec_resize(&int_vec, *ic * 3 / 2, sizeof(int)));
            mvec_ptrs(int_vec, &ic, &il, (void**)&iv);
        }
        mvec_shift(int_vec, 0, +1, sizeof(int));
        iv[0] = i;
        // Unnecessary since mvec_shift performs that
        // *il += 1;
    }
    assert(*il == 20);
    assert(iv[0] == 19 && iv[(*il)-1] == 0);

    while (*il > 0) {
        fprintf(stderr, "Dequeued %d, ", iv[(*il)-1]);
        *il -= 1;
        fprintf(stderr, "%zu elements left\n", *il);
    }

    free(int_vec);
}
