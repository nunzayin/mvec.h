#include <assert.h>
#include <stdlib.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"

int main(void) {
    size_t il = 5;
    int* ia = malloc(il*sizeof(int));
    assert(ia);
    for (size_t i = 0; i < il; i++)
        ia[i] = i;

    mvdef int* iv = mvec_fromNormal(ia, il, sizeof(int));
    assert(iv);

    mvdef int* iv_tmp; assert(iv_tmp = mvresize(iv, 10));
    iv = iv_tmp;

    while (*mvlen(iv) < mvcap(iv)) {
        iv[*mvlen(iv)] = *mvlen(iv);
        *mvlen(iv) += 1;
    }

    ia = mvec_toNormal(iv, &il, NULL);
    assert(ia);
    for (size_t i = 0; i < il; i++)
        assert(ia[i] == i);

    free(ia);
}
