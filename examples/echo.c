#include <stdio.h>
#include <string.h>
#define MVEC_IMPLEMENTATION
#include "mvec.h"

// Small echo implementation with special characters replacement.
// Imagine allocating memory for that :skull: :skull: :skull:
int main(int argc, char** argv) {
    if (!argv) return -1;
    mvdef char* arg = mvalloc(64, sizeof(char));
    if (!arg) return -2;
    for (size_t i = 1; i < argc; i++) {
        if (!argv[i]) continue;
        size_t arglen = strlen(argv[i]);
        if (arglen < 2 || !strchr(argv[i], '\\')) {
            puts(argv[i]);
            if (i < argc-1) putc(' ', stdout);
            continue;
        }
        if (mvcap(arg) < arglen) {
            mvdef char* tmp = mvresize(arg, arglen);
            if (!tmp) {
                mvfree(arg);
                return -2;
            }
            arg = tmp;
        }
        memcpy(arg, argv[i], arglen * sizeof(char));
        *mvlen(arg) = arglen;
        for (size_t j = 0; j < *mvlen(arg)-1; j++) {
            if (arg[j] != '\\') continue;
            char replacement =
                arg[j+1] == '\\' ? '\\' :
                arg[j+1] == 'n' ? '\n' :
                arg[j+1] == 't' ? '\t' : 0;
            if (!replacement) continue;
            mvshift(arg, j+1, -1);
            arg[j] = replacement;
        }
        fwrite(arg, mvelsz(arg), *mvlen(arg), stdout);
        if (i < argc-1) putc(' ', stdout);
    }
    putc('\n', stdout);
    fflush(stdout);
    mvfree(arg);
}
