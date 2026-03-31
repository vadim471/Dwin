#include <iostream>
#include <vector>

#include <initp/tools/canv.hpp>

int main(int argc, char** argv) {
    initp::tools::canv<std::vector<int>> v(
        [] (const int& a, const int& b) -> bool { return a == b; },
        [] (const int& value) -> int { return value; }
    );
    v->push_back(1);
    v->push_back(2);
    v.accept();
    std::cout << "Count: " << v->size() << std::endl;
    v->push_back(3);
    std::cout << "Count: " << v->size() << std::endl;
    v.cancel();
    std::cout << "Count: " << v->size() << std::endl;
    return 0;
}
