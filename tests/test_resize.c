#include <assert.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"
#include <stdio.h>

int main(void) {
    mvdef int* iv = mvalloc(10, sizeof(int));
    assert(iv);
    int* initial_ptr = iv;

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
    assert(mvcap(iv) == 33);

    fprintf(
            stderr,
            "Initial pointer: %p\n"
            "Current pointer: %p\n",
            initial_ptr, iv
            );

    for (size_t i = 0; i < *mvlen(iv); i++)
        fprintf(stderr, "%d ", iv[i]);
    fprintf(stderr, "\n");

    mvfree(iv);
}
