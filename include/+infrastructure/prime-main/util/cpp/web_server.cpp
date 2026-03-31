#include <iostream>

#include <initp/http/server/server.hpp>

#include <csignal>
#include <chrono>
#include <iostream>
#include <thread>

bool started = false;

void on_signal(int signal) {
    std::cout << "Get signal " << signal << std::endl;
    started = false;
}

int main(int argc, char** argv) {
    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);

    auto server = initp::http::server::create();
    server->serve();
    for (started = true; started;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return 0;
}
