#include <stdio.h>
#include <assert.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"

int main(void) {
    mvdef int* iv = mvalloc(10, sizeof(int));
    assert(iv);

    // Let's try to use mvec as a queue.
    // We'll prepend a new element to enqueue and
    // take the last element to dequeue.

    for (size_t i = 0; i < 20; i++) {
        if (*mvlen(iv) == mvcap(iv)) {
            int* new_iv;
            assert((new_iv = mvresize(iv, mvcap(iv) * 3 / 2)));
            iv = new_iv;
        }
        mvshift(iv, 0, +1);
        iv[0] = i;
    }
    assert(*mvlen(iv) == 20);
    assert(iv[0] == 19 && iv[*mvlen(iv) - 1] == 0);

    while (*mvlen(iv) > 0) {
        fprintf(stderr, "Dequeued %d, ", iv[*mvlen(iv)-1]);
        *mvlen(iv) -= 1;
        fprintf(stderr, "%zu elements left\n", *mvlen(iv));
    }

    mvfree(iv);
}
