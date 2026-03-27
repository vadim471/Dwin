#include <cstdint>
#include <iostream>

#include <initp/tools/function_generic.hpp>

void ex_f(int value, int ex) {
    std::cout << "ex_f> Value: " << value << "; Ex: " << ex << std::endl;
}

int main(int argc, char** argv) {
    typedef initp::tools::function<void(int)> fp;
    size_t address;
    fp::r_type* f;
    std::tie(f, address) = fp::bind(std::bind(ex_f, std::placeholders::_1, 123));
    f(address, 1);
    f(address, 2);
    initp::tools::function<>::free(address);
    return 0;
}
