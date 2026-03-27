#include <initp/system/debug.hpp>

int main(int argc, char** argv) {
    sys_debug_print(SYSTEM_LEVEL_WARNING, "main", "No test here :(");
    return 0;
}
