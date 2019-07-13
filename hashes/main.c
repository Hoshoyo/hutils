#include <stdio.h>
#include "sha256.c"

int main() {
    char result[32] = {0};
    sha256("abc", 3, result);
    sha256_print(result);

    return 0;
}