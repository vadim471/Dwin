#include <itp/memory.h>

#include <stdio.h>
#include <stdlib.h>

void* itp_malloc(itp_size_t size, const char* what) {
    void* ptr = malloc(size);
    printf("Allocate %d bytes at %d (%s)\r\n", size, (int)ptr, what);
    return ptr;
}

void itp_free(void* ptr) {
    free(ptr);
    printf("Free memory at %d\r\n", (int)ptr);
}
