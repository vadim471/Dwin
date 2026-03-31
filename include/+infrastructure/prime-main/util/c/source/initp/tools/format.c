#include <initp/tools/format.h>
#include <initp/system/debug.h>

#include <stdio.h>

uint8_t tools_array_to_string(const uint8_t* data, size_t size, char* buffer, size_t length) {
    if (!data || !buffer) return 0;
    if (length < 1) return 0;
    size_t max_size = length / 3;
    if (size > max_size)
        size = max_size;
    if (size > 0) {
        sprintf(buffer, "%02x", data[0]);
        for (size_t i = 1; i < size; ++i) {
            sprintf(buffer + i * 3 - 1, " %02x", data[i]);
        }
    } else {
        buffer[0] = 0;
    }
    return 1;
}

void sys_trace_array(const char* what, const char* data, size_t size) {
    if (!data || !size) return;
    const char* target = !what ? "sys_trace_array" : what;
    const uint16_t buffer_size = 0xFF;
    const uint8_t max = buffer_size / 3;
    size_t count = size / (size_t)max;
    if (size % max) count++;
    char buffer[buffer_size];
    for (size_t i = 0; i < count; ++i) {
        size_t chunk = size - i * max;
        if (chunk > max) chunk = (size_t)max;
        if (tools_array_to_string((const uint8_t*)(data + i * max), chunk, buffer, buffer_size)) {
            if (count > 1) {
                sys_debug_print(SYSTEM_LEVEL_TRACE, target, "Data (%d): %s (%d)", (int)(i + 1), buffer, (int)chunk);
            } else {
                sys_debug_print(SYSTEM_LEVEL_TRACE, target, "Data: %s (%d)", buffer, (int)chunk);
            }
        }
    }
}

void tools_char_to_bcd(const char* data, uint8_t size, uint8_t* buffer) {
    if (!data || !size || !buffer) return;
    uint8_t l, h;
    for (uint8_t i = 0, max = size / 2; i < max; ++i) {
        l = (uint8_t)(data[i * 2 + 1] - '0');
        h = (uint8_t)(data[i * 2    ] - '0');
        buffer[i] = l | ((h << 4) & 0xF0);
    }
}
