#include <itp/memory.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void* ptr;
    size_t size;
} memory_info;

#define MAX_MEMORY 0x800
memory_info memory[MAX_MEMORY];
size_t max_index;
size_t memory_peak;
size_t ptr_peak;
size_t allocated;
size_t freed;
size_t malloc_calls;
size_t free_calls;

void init_memory(void) {
    memset(memory, 0x0, sizeof(memory_info) * MAX_MEMORY);
    max_index = 0;
    memory_peak = 0;
    ptr_peak = 0;
    allocated = 0;
    freed = 0;
    malloc_calls = 0;
    free_calls = 0;
}

void trace_memory(void) {
    printf("Total allocated bytes: %d\r\n", allocated);
    printf("Total freed bytes: %d\r\n", freed);
    printf("Max memory peak: %d\r\n", memory_peak);
    printf("Peak pointers count: %d\r\n", ptr_peak);
    printf("malloc() calls: %d\r\n", malloc_calls);
    printf("free() calls: %d\r\n", free_calls);
    printf("Memory leak: %d\r\n", allocated - freed);
    for (size_t i = 0; i <= max_index; i++) {
        if (memory[i].ptr) {
            printf("%d bytes at %d\r\n", memory[i].size, (int)memory[i].ptr);
        }
    }
}

void* itp_malloc(itp_size_t size, const char* what) {
    malloc_calls++;
    void* ptr = malloc(size);
    printf("Allocate %d bytes at %d (%s)\r\n", size, (int)ptr, what);
    allocated += size;
    for (size_t i = 0; i < MAX_MEMORY; i++) {
        if (!memory[i].ptr) {
            memory[i].ptr = ptr;
            memory[i].size = size;
            if (i > max_index)
                max_index = i;
            break;
        }
    }
    size_t total = 0;
    size_t count = 0;
    for (size_t i = 0; i <= max_index; i++) {
        if (memory[i].ptr) {
            total += memory[i].size;
            count++;
        }
    }
    if (total > memory_peak) {
        memory_peak = total;
        ptr_peak = count;
    }
    return ptr;
}

void itp_free(void* ptr) {
    free_calls++;
    free(ptr);
    for (size_t i = 0; i <= max_index; i++) {
        if (memory[i].ptr == ptr) {
            printf("Free %d bytes at %d\r\n", memory[i].size, (int)ptr);
            freed += memory[i].size;
            memory[i].ptr = NULL;
            return;
        }
    }
    printf("Free memory at %d\r\n", (int)ptr);
}
