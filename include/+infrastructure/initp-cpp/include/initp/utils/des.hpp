#ifndef INITPLUS_UTILS_DES_HPP_INCLUDED
#define INITPLUS_UTILS_DES_HPP_INCLUDED

#include <vector>
#include <cstdint>

extern "C" {

#include <initp/tools/des.h>

}

namespace initp {
namespace utils {
namespace des {

std::vector<uint8_t> encrypt_3des(const std::vector<uint8_t>& data, uint8_t type, uint8_t padding, const std::vector<uint8_t>& key);

std::vector<uint8_t> encrypt_3des(const std::vector<uint8_t>& data, uint8_t type, uint8_t padding, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv);

std::vector<uint8_t> decrypt_3des(const std::vector<uint8_t>& data, uint8_t type, uint8_t padding, const std::vector<uint8_t>& key);

std::vector<uint8_t> decrypt_3des(const std::vector<uint8_t>& data, uint8_t type, uint8_t padding, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv);

}}}

#endif // INITPLUS_UTILS_DES_HPP_INCLUDED
