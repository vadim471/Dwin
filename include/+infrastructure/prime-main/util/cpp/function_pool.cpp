#include <iostream>
#include <initp/generic/function_pool.hpp>

#include <boost/preprocessor/repetition/repeat.hpp>

BOOST_PP_REPEAT(2, DEFINE_FUNCTION_POOL, void(void));

void print(int value) {
    std::cout << "Value is " << value << std::endl;
}

int main(int argc, char** argv) {

    typedef initp::tools::function_pool<2, void(void)> pool;

    pool::type* f1 = pool::bind(std::bind(&print, 1));
    pool::type* f2 = pool::bind(std::bind(&print, 2));
    pool::type* f3 = pool::bind(std::bind(&print, 3));
    if (f1) f1();
    if (f2) f2();
    if (f3) f3();

    pool::free(f1);

    f3 = pool::bind(std::bind(&print, 3));
    f2 = pool::bind(std::bind(&print, 2));
    f1 = pool::bind(std::bind(&print, 1));
    if (f1) f1();
    if (f2) f2();
    if (f3) f3();

    pool::clear();

    if (f1) f1();
    if (f2) f2();
    if (f3) f3();
    return 0;
}
