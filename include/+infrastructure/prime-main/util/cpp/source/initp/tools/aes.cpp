#include <initp/tools/aes.hpp>

extern "C" {

#include <initp/tools/aes.h>
#include <openssl/aes.h>

}

namespace initp {
namespace tools {
namespace aes {

std::string encrypt(const std::string& data, const std::string& key, uint64_t salt) {

    sys_aes_context_t context;
    sys_init_aes(&context, key.c_str(), key.size(), salt, 1, 0);

    std::string result;
    result.resize(data.size() + AES_BLOCK_SIZE);

    result.resize(sys_encrypt_aes(&context, data.c_str(), data.size(), &result[0], result.size()));
    return result;
}

std::string decrypt(const std::string& data, const std::string& key, uint64_t salt) {

    sys_aes_context_t context;
    sys_init_aes(&context, key.c_str(), key.size(), salt, 0, 1);

    std::string result;
    result.resize(data.size() + AES_BLOCK_SIZE);

    result.resize(sys_decrypt_aes(&context, data.c_str(), data.size(), &result[0], result.size()));
    return result;
}

std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key, uint64_t salt) {

    sys_aes_context_t context;
    sys_init_aes(&context, (const char*)&key[0], key.size(), salt, 1, 0);

    std::vector<uint8_t> result;
    result.resize(data.size() + AES_BLOCK_SIZE);

    result.resize(sys_encrypt_aes(&context, (const char*)&data[0], data.size(), (char*)&result[0], result.size()));
    return result;
}

std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key, uint64_t salt) {

    sys_aes_context_t context;
    sys_init_aes(&context, (const char*)&key[0], key.size(), salt, 0, 1);

    std::vector<uint8_t> result;
    result.resize(data.size() + AES_BLOCK_SIZE);

    result.resize(sys_decrypt_aes(&context, (const char*)&data[0], data.size(), (char*)&result[0], result.size()));
    return result;
}

}}}
