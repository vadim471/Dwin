#ifndef ITP_CL1_TOOLS_H_INCLUDED
#define ITP_CL1_TOOLS_H_INCLUDED

#include <stdint.h>

/**
 * \file tools.h
 * \brief Файл содержит вспомогательные функции.
 */

uint8_t itp_cl1_tools_string_to_uint8(const char* string);

uint32_t itp_cl1_tools_string_to_uint32(const char* string);

uint8_t itp_cl1_tools_string_to_bool(const char* string);

uint8_t itp_cl1_tools_has_bit(int64_t* pack, uint8_t index);
uint8_t itp_cl1_tools_get_bit(int64_t* pack, uint8_t index);
void itp_cl1_tools_set_bit(int64_t* pack, uint8_t index, uint8_t value);

#endif // ITP_CL1_TOOLS_H_INCLUDED
