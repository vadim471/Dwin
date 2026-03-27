#include <iostream>
#include <initp/tools/decimal.hpp>

int main(int argc, char** argv) {
    udec16_t d1(111, 2);
    udec16_t d2(1111, 3);
    udec16_t d3 = d1 + d2;
    std::cout << "d1: " << d1 << "; d2: " << d2 << "; d1+d2: " << d3 << std::endl;
    udec32_t d4 = udec32_t::from_double(1.01, 3);
    std::cout << "d4: " << d4 << std::endl;
    std::cout << "d4 (double): " << double(d4) << std::endl;
    udec32_t d5 = udec32_t::from_double(1.309, 3);
    udec32_t d6 = udec32_t::from_double(1.44, 3);
    std::cout << "diff d5: " << (d4.diff(d5, 0.3) ? "true" : "false") << std::endl;
    std::cout << "diff d6: " << (d4.diff(d6, 0.3) ? "true" : "false") << std::endl;
    return 0;
}
