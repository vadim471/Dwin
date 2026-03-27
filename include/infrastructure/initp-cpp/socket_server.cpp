#include <initp/platform/socket.hpp>

#include <iostream>
#include <csignal>
#include <thread>

bool started = false;

void on_signal(int signal) {
    std::cout << "Server received signal " << signal << std::endl;
    started = false;
}

int main(int argc, char** argv) {
    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);
    initp::system::socket socket;
    socket.listen(10135);
    char data[0x100];
    size_t size;
    for (started = true; started;) {
        size = socket.read(data, 0x100);
        if (size > 0) {
            socket.write(data, size);
            std::cout << "Received " << size << " bytes" << std::endl;
        }
        initp::system::time::sleep_for(5);
    }
    std::cout << "Server terminated" << std::endl;
    return 0;
}
