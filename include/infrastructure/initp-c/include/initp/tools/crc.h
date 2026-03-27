#ifndef INITPLUS_TOOLS_CRC_H_INCLUDED
#define INITPLUS_TOOLS_CRC_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

uint16_t sys_eval_crc16(uint16_t crc, const char* data, size_t size);

#endif // INITPLUS_TOOLS_CRC_H_INCLUDED
