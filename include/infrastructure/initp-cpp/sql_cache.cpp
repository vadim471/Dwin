#include <iostream>
#include <csignal>
#include <thread>

#include <initp/tools/decimal.hpp>
#include <initp/database/cache.hpp>
#include <initp/database/cache_detail.hpp>
#include <initp/system/time.hpp>

bool started = false;

void on_signal(int signal) {
    std::cout << "Received signal " << signal << std::endl;
    started = false;
}

int main(int argc, char** argv) {
    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);

    using namespace initp;
    typedef database::cache::table<uint64_t, float, dec32_t, system::time_t, bool, std::string> cache_t;
    typedef database::cache::table<std::string> cache2_t;

    database::cache::manager manager("./test.cache");
    cache_t& cache = manager.emplace<cache_t>("cache", false, 1000);
    cache.set_names({ "t1", "t2", "t3", "t4" });
    cache2_t& cache2 = manager.emplace<cache2_t>("cache2", false, 1000);

    cache.insert(1, .4d, dec32_t(321, 2), system::time::milliseconds(), true, "test");
    //cache.insert(2, .4d, dec32_t(321, 2), system::time::milliseconds(), true, "test");
    //cache.remove(2);
    //cache.success(2);
    //cache.failed(1);

    uint64_t id(0);
    float t1(.0f);
    dec32_t t2(0, 0);
    system::time_t t3(0);
    bool t4(false);
    std::string arg5;
    system::error_code ec = cache.select_by_expiry(id, t1, t2, t3, t4, arg5);
    //cache.select(id, t1, t2, t3, t4, arg5);
    std::cout << "Result: " << ec << "; ID: " << id << "; T1: " << t1 << "; T2: " << t2 << "; T3: " << t3 << "; Arg5: '" << arg5 << "'" << std::endl;

    /*for (started = true; started;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }*/
    cache2.insert("test");
    std::cout << "Complete" << std::endl;
    return 0;
}
