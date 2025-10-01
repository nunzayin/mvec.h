#include <assert.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"

int main(void) {
    mvdef int* iv = mvalloc(24, sizeof(*iv));
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

    mvfree(iv);
    mvfree(bigger_iv);
    mvfree(smaller_iv);
}
