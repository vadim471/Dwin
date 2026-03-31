#ifndef INITPLUS_UTILS_AES_HPP_INCLUDED
#define INITPLUS_UTILS_AES_HPP_INCLUDED

#include <string>
#include <vector>
#include <cstdint>

namespace initp {
namespace utils {
namespace aes {

std::string encrypt(const std::string&, const std::string&, uint64_t = 0);

std::string decrypt(const std::string&, const std::string&, uint64_t = 0);

std::vector<uint8_t> encrypt(const std::vector<uint8_t>&, const std::vector<uint8_t>&, uint64_t = 0);

std::vector<uint8_t> decrypt(const std::vector<uint8_t>&, const std::vector<uint8_t>&, uint64_t = 0);

}}}

#endif // INITPLUS_UTILS_AES_HPP_INCLUDED
