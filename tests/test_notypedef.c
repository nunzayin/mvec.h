#include <stdio.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"

int main(void) {
    void* omg_what_this_ptr_is_hiding = mvec_alloc(12, sizeof(int));

    int* garbage = mvec_data(omg_what_this_ptr_is_hiding);
    fprintf(stderr,
            "Oh no! I use some uninitialized storage:\n"
            "%d, %d, %d\n",
            garbage[2], garbage[4], garbage[6]
            );

    free(omg_what_this_ptr_is_hiding);
}
