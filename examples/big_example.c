#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"

// shitty randint implementation
static inline int randint(int upper_bound) {
    return rand() % upper_bound;
}

int main(void) {
    mvdef int* iv = mvalloc(16, sizeof(int));
    assert(iv);

    iv[0] = 1337;
    *mvlen(iv) += 1;

    fprintf(stderr,
            "Current capacity = %zu\n"
            "Current length = %zu\n"
            "Size of a single element (bytes): %zu\n",
            mvcap(iv),
            *mvlen(iv),
            mvelsz(iv)
            );

    for (int i = iv[0] + 1; *mvlen(iv) < mvcap(iv); i++)
        iv[(*mvlen(iv))++] = i;

    fprintf(stderr,
            "First element [0]: %d\n"
            "Last element [%zu]: %d\n",
            iv[0],
            *mvlen(iv) - 1,
            iv[*mvlen(iv) - 1]
           );

    bool iv_empty = *mvlen(iv) == 0;

    for (size_t i = 1; i <= 5; i++) {
        if (*mvlen(iv) == mvcap(iv)) {
            mvdef int* new_iv = mvresize(iv, mvcap(iv) * 2);
            assert(new_iv);
            iv = new_iv;
        }
        iv[(*mvlen(iv))++] = iv[15] + i;
    }

    if (*mvlen(iv) < mvcap(iv)) {
        mvdef int* new_iv = mvresize(iv, *mvlen(iv));
        assert(new_iv);
        iv = new_iv;
    }

    *mvlen(iv) = 0;

    assert(mvcap(iv) >= 5);
    for (size_t i = 0; i < 5; i++)
        iv[(*mvlen(iv))++] = i+1;
    assert(*mvlen(iv) + 2 <= mvcap(iv));
    mvshift(iv, 3, +1);
    iv[3] = 1337;
    iv[(*mvlen(iv))++] = iv[0];
    mvshift(iv, 1, -1);
    fputs("Current iv contents: ", stderr);
    for (size_t i = 0; i < *mvlen(iv); i++)
        fprintf(stderr, "%d ", iv[i]);
    fputc('\n', stderr);

    memset(iv, 0, mvcap(iv) * mvelsz(iv));
    for (size_t i = 0; i < mvcap(iv); i++)
        assert(iv[i] == 0);

    // TODO mvcopy
    // TODO assigning array

    mvfree(iv);
}
