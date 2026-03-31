#ifndef INITPLUS_TOOLS_CRC_H_INCLUDED
#define INITPLUS_TOOLS_CRC_H_INCLUDED

#include <stdint.h>

uint32_t sys_array_to_uint32(const char* buffer, uint8_t length);

uint64_t sys_array_to_uint64(const char* buffer, uint8_t length);

#endif // INITPLUS_TOOLS_CRC_H_INCLUDED
