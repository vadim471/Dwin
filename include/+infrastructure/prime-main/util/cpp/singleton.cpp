#include <initp/generic/singleton.hpp>

#include <iostream>
#include <memory>

struct Test {
    Test(int value)
        : value_(value)
    {
        std::cout << "Constructed " << value_ << std::endl;
    }
    ~Test(void) {
        std::cout << "Destructed " << value_ << std::endl;
    }
    int value_;
};

struct TestSingleton {
    std::unique_ptr<Test> instance;
};

int main(int argc, char** argv) {
    TestSingleton& test_1 = *initp::tools::singleton_with_priority<TestSingleton, 100>();
    TestSingleton& test_2 = *initp::tools::singleton_with_priority<TestSingleton, 200>();
    test_1.instance.reset(new Test(1));
    test_2.instance.reset(new Test(2));
    std::cout << "Hello" << std::endl;
    return 0;
}
