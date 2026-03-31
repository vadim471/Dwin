#include <iostream>

#include <initp/system/time.hpp>
#include <initp/tools/date.hpp>

int main(int argc, char** argv) {
    std::cout << initp::tools::date::format(initp::system::time::now()) << std::endl;
    return 0;
}
