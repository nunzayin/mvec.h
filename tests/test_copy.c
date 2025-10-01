#include <assert.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"

int main(void) {
    mvec_(int)* int_vec = mvec_alloc(10, sizeof(int));
    assert(int_vec);
    int* iv; size_t* ic; size_t* il;
    mvec_ptrs(int_vec, &ic, &il, (void**)&iv);

    for (size_t i = 0; i < 20; i++) {
        if (*il == *ic) {
            assert(!mvec_resize(&int_vec, *ic * 3 / 2, sizeof(int)));
            mvec_ptrs(int_vec, &ic, &il, (void**)&iv);
        }
        mvec_shift(int_vec, 0, +1, sizeof(int));
        iv[0] = i;
    }
    assert(iv[0] == 19 && iv[*il - 1] == 0);

    size_t v_size = mvec_size(int_vec, sizeof(int));
    mvec_(int)* new_ivec = malloc(v_size);
    assert(new_ivec);
    memcpy(new_ivec, int_vec, v_size);
    int* n_iv; size_t* n_ic; size_t* n_il;
    mvec_ptrs(new_ivec, &n_ic, &n_il, (void**)&n_iv);
    assert(*ic == *n_ic && *il == *n_il);
    for (size_t i = 0; i < *n_il; i++)
        n_iv[i] += 1;
    assert(n_iv[0] == 20 && n_iv[*n_il - 1] == 1);

    free(int_vec);
    free(new_ivec);
}
