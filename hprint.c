#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include "light_array.h"

typedef struct {
    char* buffer;
    int   index;
} HBuffer;

HBuffer hbuffer_new() {
    HBuffer b = {0};
    b.buffer = array_new(char);
    b.index = 0;
    return b;
}

int hprint(HBuffer* buffer, const char* fmt, ...) {
    va_list args;
	va_start(args, fmt);

    int capacity = (int)array_capacity(buffer->buffer) - buffer->index;
    int to_write = vsnprintf(buffer->buffer + buffer->index, capacity, fmt, args) + 1;
    array_length(buffer->buffer) += capacity;

	va_end(args);

    if(to_write > capacity) {
        va_list args2;
        va_start(args2, fmt);

        char* mem = calloc(1, to_write + 1);
        int l = vsnprintf(mem, to_write + 1, fmt, args2);

        int extra = array_capacity(buffer->buffer);
        array_allocate(buffer->buffer, to_write - capacity + extra);
        memcpy(buffer->buffer + buffer->index + capacity - 1, mem + capacity - 1, to_write - capacity + 1);

        array_length(buffer->buffer) += (to_write - capacity);

        free(mem);

        va_end(args2);
    }

    buffer->index += to_write - 1;
}

int main() {

    char* str = "Hello world my name is Hoshoyo!!";

    HBuffer b = hbuffer_new();

    hprint(&b, "%s", str);
    hprint(&b, "hello");
    hprint(&b, "world");
    hprint(&b, "%d %s", 23123, str);

    printf("%s\n", b.buffer);

    return 0;
}