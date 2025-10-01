#include <assert.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"
#include <stdio.h>

int main(void) {
    int* iv = mvalloc(24, sizeof(int));
    assert(iv);

    for (size_t i = 0; i < 20; i++)
        iv[(*mvlen(iv))++] = i;
    assert(*mvlen(iv) == 20);

    while (*mvlen(iv) > 0) {
        fprintf(stderr,
                "Popped first element with value %d\n",
                iv[0]
               );
        mvshift(iv, 1, -1);
    }
    assert(*mvlen(iv) == 0);

    mvfree(iv);
}
