#include <stdio.h>
#include "../hobig.c"

int main() {
    HoBigInt n = hobig_new_dec("1234567899102808301597904510769737617246198621361725", 0);
    hobig_int_print(n);

    printf("\n");
    return 0;
}