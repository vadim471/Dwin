#ifndef ITP_CL1_TOOLS_HPP_INCLUDED
#define ITP_CL1_TOOLS_HPP_INCLUDED

#include <cstdint>
#include <string>

/**
 * \file tools.hpp
 * \brief Файл содержит вспомогательные функции.
 */

namespace itp {
namespace tools {

uint8_t string_to_uint8(const std::string&);

uint32_t string_to_uint32(const std::string&);

bool string_to_bool(const std::string&);

std::string time_to_string(const std::string&, uint64_t);

uint8_t has_bit(int64_t* pack, uint8_t index);
uint8_t get_bit(int64_t* pack, uint8_t index);
void set_bit(int64_t* pack, uint8_t index, uint8_t value);

}}

#endif // ITP_CL1_TOOLS_HPP_INCLUDED
