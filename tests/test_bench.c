#include <assert.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"
#include <stdio.h>
#include <time.h>

static const size_t AMOUNT_ELEMENTS_TO_ADD = 100000000;

// shitty randint implementation
static inline int randint(int upper_bound) {
    return rand() % upper_bound;
}

static int int_comp(const void* _a, const void* _b) {
    int a = *(const int*)_a;
    int b = *(const int*)_b;
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

static mvec_t* bench_mvec(void) {
    mvec_t* int_vec = mvec_alloc(10, sizeof(int));
    assert(int_vec);
    int* iv; size_t* il; size_t* ic;
    mvec_ptrs(int_vec, &ic, &il, (void**)&iv);

    for (size_t i = 0; i < AMOUNT_ELEMENTS_TO_ADD; i++) {
        if (*il == *ic) {
            assert(!mvec_resize(&int_vec, *ic * 2, sizeof(int)));
            mvec_ptrs(int_vec, &ic, &il, (void**)&iv);
        }
        iv[(*il)++] = randint(AMOUNT_ELEMENTS_TO_ADD);
    }

    qsort((void*)iv, *il, sizeof(int), int_comp);
    return int_vec;
}

typedef struct {
    int* head;
    size_t len;
    size_t cap;
} IntDynamicArray;

static IntDynamicArray bench_manual(void) {
    IntDynamicArray int_arr = {0};
    int_arr.head = malloc(10*sizeof(int));
    assert(int_arr.head);
    int_arr.cap = 10;

    for (size_t i = 0; i < AMOUNT_ELEMENTS_TO_ADD; i++) {
        if (int_arr.len == int_arr.cap) {
            int* new_head = realloc(
                    int_arr.head,
                    int_arr.cap * 2 * sizeof(int)
                    );
            assert(new_head);
            int_arr.head = new_head;
            int_arr.cap *= 2;
        }
        (int_arr.head)[int_arr.len++] = randint(AMOUNT_ELEMENTS_TO_ADD);
    }

    qsort((void*)(int_arr.head), int_arr.len, sizeof(int), int_comp);
    return int_arr;
}

int main(void) {
    time_t time_start;
    time_t time_end;
    double bench_res;

    assert(time(&time_start) != (time_t)(-1));
    mvec_t* int_vec = bench_mvec();
    assert(time(&time_end) != (time_t)(-1));
    bench_res = difftime(time_end, time_start);
    int* iv; size_t* il;
    mvec_ptrs(int_vec, NULL, &il, (void**)&iv);
    assert(*il >= 6);
    fprintf(stderr,
            "MVEC\n"
            "ELEMENTS: %d %d %d ... %d %d %d\n"
            "BENCHMARK RESULTS: %.2fs\n",
            iv[0], iv[1], iv[2], iv[(*il)-3], iv[(*il)-2], iv[(*il)-1],
            bench_res
            );
    free(int_vec);

    assert(time(&time_start) != (time_t)(-1));
    IntDynamicArray int_arr = bench_manual();
    assert(time(&time_end) != (time_t)(-1));
    bench_res = difftime(time_end, time_start);
    assert(int_arr.len >= 6);
    fprintf(stderr,
            "\nMANUAL DYNARR\n"
            "ELEMENTS: %d %d %d ... %d %d %d\n"
            "BENCHMARK RESULTS: %.2fs\n",
            int_arr.head[0], int_arr.head[1], int_arr.head[2],
            int_arr.head[(int_arr.len)-3], int_arr.head[(int_arr.len)-2],
            int_arr.head[(int_arr.len)-1],
            bench_res
            );
    free(int_arr.head);
}
