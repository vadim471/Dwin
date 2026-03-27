#include <initp/tools/ascii.h>

#include <stdlib.h>
#include <string.h>

uint32_t sys_array_to_uint32(const char* buffer, uint8_t length) {
    if (length > 9) return 0;
    char string[10];
    memcpy(string, buffer, length);
    string[length] = 0;
    return (uint32_t)atol(string);
}

uint64_t sys_array_to_uint64(const char* buffer, uint8_t length) {
    if (length > 15) return 0;
    char string[16];
    memcpy(string, buffer, length);
    string[length] = 0;
    return (uint64_t)atoll(string);
}
