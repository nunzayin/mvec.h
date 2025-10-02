#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MVEC_CUSTOM_MEMFUNCS
#define MVEC_IMPLEMENTATION
#include "mvec.h"

int main(void) {
    mvec_setMemcpy(memcpy);
    mvec_setMemmove(memmove);
    mvdef int* iv = mvalloc(23, sizeof(int));
    assert(iv);
    for (size_t i = 0; i < 16; i++) {
        iv[(*mvlen(iv))++] = i;
    }
    assert(iv[0] == 0 && iv[*mvlen(iv) - 1] == 15);

    mvdef int* bigger_iv = mvcopy(iv, 36);
    assert(bigger_iv);
    assert(mvcap(bigger_iv) == 36);
    for (size_t i = 0; i < 16; i++) {
        bigger_iv[i] += 1;
    }
    assert(bigger_iv[0] == 1 && bigger_iv[*mvlen(bigger_iv) - 1] == 16);

    mvdef int* smaller_iv = mvcopy(iv, 8);
    assert(smaller_iv);
    assert(mvcap(smaller_iv) == 8);
    for (size_t i = 0; i < *mvlen(smaller_iv); i++) {
        smaller_iv[i] -= 1;
    }
    assert(
            smaller_iv[0] == -1
            && smaller_iv[*mvlen(smaller_iv) - 1] == *mvlen(smaller_iv) - 2
          );

    for (size_t i = 0; i < 20; i++) {
        if (*mvlen(iv) == mvcap(iv)) {
            mvdef int* new_iv;
            assert((new_iv = mvresize(iv, mvcap(iv) * 3 / 2)));
            iv = new_iv;
        }
        mvshift(iv, 0, +1);
        iv[0] = i;
    }
    fputs("Current iv contents: ", stderr);
    for (size_t i = 0; i < *mvlen(iv); i++)
        fprintf(stderr, "%d ", iv[i]);
    fputc('\n', stderr);

    mvfree(iv);
    mvfree(bigger_iv);
    mvfree(smaller_iv);
}
