#include <assert.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"

int main(void) {
    mvec_(int)* vec = mvec_alloc(10, sizeof(int));
    assert(vec);
    int* iv; size_t* ic; size_t* il;
    mvec_ptrs(vec, &ic, &il, (void**)&iv);
    for (size_t i = *ic; i > 0; i--)
        iv[(*il)++] = i;
    assert(iv[0] == 10 && iv[*il-1] == 1);
    free(vec);
}
