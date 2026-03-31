#include <iostream>
#include <memory>

#include <initp/generic/weak_bind.hpp>

class A:
    public std::enable_shared_from_this<A> {
    typedef A self_type;
public:
    typedef std::shared_ptr<self_type> ptr;
};

void hello(void) {
    std::cout << "Hello" << std::endl;
}

void print(int value) {
    std::cout << "Value: " << value << std::endl;
}

int main(int argc, char** argv) {
    A::ptr a = std::make_shared<A>();
    auto f = initp::tools::weak::bind<void(int), A>(std::bind(&print, std::placeholders::_1), a);
    f(1);
    auto h = initp::tools::weak::bind<void(void), A>(std::bind(&hello), a);
    h();
    a.reset();
    h();
    return 0;
}
