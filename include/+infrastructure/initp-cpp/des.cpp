#include <initp/utils/des.hpp>
#include <initp/utils/string.hpp>

#include <iostream>

int main(int argc, char** argv) {
    using namespace initp::utils;
    {
        const std::vector<uint8_t> key({ 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF });
        const std::vector<uint8_t> data({ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F });

        std::vector<uint8_t> out;
        std::cout << "   Source: " << initp::utils::to_string(data) << " (" << data.size() << ")" << std::endl;

        out = des::encrypt_3des(data, SYS_DES_TYPE_CBC, SYS_DES_PADDING_NONE, key);
        std::cout << "Encrypted: " << initp::utils::to_string(out) << " (" << out.size() << ")" << std::endl;

        out = des::decrypt_3des(out, SYS_DES_TYPE_CBC, SYS_DES_PADDING_NONE, key);
        std::cout << "Decrypted: " << initp::utils::to_string(out) << " (" << out.size() << ")" << std::endl;
    }
    return 0;
}
