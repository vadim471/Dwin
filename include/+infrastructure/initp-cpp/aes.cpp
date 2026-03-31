#include <initp/utils/aes.hpp>

#include <iostream>

int main(int argc, char** argv) {
    using namespace initp::utils;
    {
        const std::string key("KEY");
        std::string out = aes::encrypt("test", key);
        std::cout << "Encrypted size: " << out.size() << std::endl;
        out = aes::decrypt(out, key);
        std::cout << "Decrypted: " << out << std::endl;
    }
    {
        const std::vector<uint8_t> key({ 'K', 'E', 'Y' });
        std::vector<uint8_t> out = aes::encrypt({ 1, 2, 3, 4 }, key);
        std::cout << "Encrypted size: " << out.size() << std::endl;
        out = aes::decrypt(out, key);
        std::cout << "Decrypted:";
        for (size_t i = 0; i < out.size(); ++i)
            std::cout << " " << (int)out[i];
        std::cout << std::endl;
    }
    return 0;
}
