#include <assert.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"
#include <stdio.h>

int main(void) {
    mvec_t* int_vec = mvec_alloc(24, sizeof(int));
    assert(int_vec);
    int* iv; size_t* il;
    mvec_ptrs(int_vec, NULL, &il, (void**)&iv);

    for (size_t i = 0; i < 20; i++)
        iv[(*il)++] = i;
    assert(*il == 20);

    while (*il > 0) {
        fprintf(stderr,
                "Popped first element with value %d\n",
                iv[0]
               );
        mvec_shift(int_vec, 1, -1, sizeof(int));
    }
    assert(*il == 0);

    free(int_vec);
}
