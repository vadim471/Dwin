#include <iostream>
#include <csignal>
#include <thread>

bool started = false;

void on_signal(int signal) {
    std::cout << "Dummy received signal " << signal << std::endl;
    started = false;
}

int main(int argc, char** argv) {
    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);
    for (started = true; started;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    std::cout << "Dummy terminated" << std::endl;
    return 0;
}
