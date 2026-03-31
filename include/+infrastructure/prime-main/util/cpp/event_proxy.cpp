#include <initp/generic/event_proxy.hpp>

#include <iostream>

void test_1(void) {
    std::cout << "Hello from test_1" << std::endl;
}

void test_2(int a) {
    std::cout << "Hello from test_2: " << a << std::endl;
}

void test_3(int a, int b) {
    std::cout << "Hello from test_3: " << a << ", " << b << std::endl;
}

int main(int argc, char** argv) {

    typedef initp::tools::event_proxy<void(void)> ep1_t;
    ep1_t ep1;
    ep1.connect(&test_1);
    ep1.connect(std::bind(&test_2, 1));
    ep1.rise();

    std::cout << "--------------" << std::endl;

    typedef initp::tools::event_proxy<void(int)> ep2_t;
    ep2_t ep2;
    ep2.connect(&test_2);
    ep2.rise(2);

    std::cout << "--------------" << std::endl;

    typedef initp::tools::event_proxy<void(int, int)> ep3_t;
    ep3_t ep3;
    ep3.connect(&test_3);
    ep3.rise(3, 4);
    return 0;
}
