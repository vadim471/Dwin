#include <initp/utils/des.hpp>

extern "C" {

#include <initp/tools/des.h>

}

namespace initp {
namespace utils {
namespace des {

std::vector<uint8_t> encrypt_3des(const std::vector<uint8_t>& data, uint8_t type, uint8_t padding, const std::vector<uint8_t>& key) {
    std::vector<uint8_t> iv({ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 });
    return encrypt_3des(data, type, padding, key, iv);
}

std::vector<uint8_t> encrypt_3des(const std::vector<uint8_t>& data, uint8_t type, uint8_t padding, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {

    sys_des_context_t context;
    std::vector<uint8_t> result;

    if (!sys_init_3des(&context, &key[0], key.size(), &iv[0], iv.size(), 1, 0, type, padding)) {
        return result;
    }

    result.resize(sys_calc_3des_data_size(data.size()));

    result.resize(sys_encrypt_3des(&context, &data[0], data.size(), &result[0], result.size()));
    return result;
}

std::vector<uint8_t> decrypt_3des(const std::vector<uint8_t>& data, uint8_t type, uint8_t padding, const std::vector<uint8_t>& key) {
    std::vector<uint8_t> iv({ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 });
    return decrypt_3des(data, type, padding, key, iv);
}

std::vector<uint8_t> decrypt_3des(const std::vector<uint8_t>& data, uint8_t type, uint8_t padding, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {

    sys_des_context_t context;
    std::vector<uint8_t> result;

    if (!sys_init_3des(&context, &key[0], key.size(), &iv[0], iv.size(), 0, 1, type, padding)) {
        return result;
    }

    result.resize(sys_calc_3des_data_size(data.size()));

    result.resize(sys_decrypt_3des(&context, &data[0], data.size(), &result[0], result.size()));
    return result;
}

}}}
