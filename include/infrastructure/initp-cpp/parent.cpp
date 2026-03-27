#include <iostream>

#include <initp/tools/enable_parent.hpp>

class A {
    typedef A self_type;
public:
    typedef std::shared_ptr<self_type> ptr;
};

class B:
    public initp::tools::enable_ptr_parent<A> {
public:
    B(A::ptr parent):
        enable_ptr_parent(parent)
    {}
};

class C:
    public initp::tools::enable_raw_parent<B> {
public:
    C(B* parent):
        enable_raw_parent(parent)
    {}
};

int main(int argc, char** argv) {
    A::ptr a = std::make_shared<A>();
    B b(a);
    C c(&b);
    c.parent()->parent();
    return 0;
}
