#include <initp/system/log.hpp>

int main(int argc, char** argv) {
    initp::system::log::initialize(".", "test", initp::system::log::BOM_UTF8);
    LOG_TRACE("main") << "Тест";
    initp::system::log::stop();
    return 0;
}
