#include <iostream>

#include <initp/system/time.hpp>
#include <initp/utils/date.hpp>

int main(int argc, char** argv) {
    std::cout << initp::utils::date::format(initp::system::time::milliseconds()) << std::endl;
    return 0;
}
