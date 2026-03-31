#include <initp/platform/socket.hpp>

#include <iostream>
#include <csignal>
#include <thread>

bool started = false;

void on_signal(int signal) {
    std::cout << "Client received signal " << signal << std::endl;
    started = false;
}

int main(int argc, char** argv) {
    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);
    initp::system::time_t now, last = 0;
    initp::system::socket socket;
    //socket.connect("127.0.0.1", 10135);
    socket.connect("10.10.1.92", 34253);
    const char* data = "Hello";
    char buffer[0x10];
    size_t size;
    for (started = true; started;) {
        now = initp::system::time::now();
        if (last + 2000 < now) {
            last = now;
            size = socket.write(data, 6);
            if (size > 0) {
                std::cout << "Message sent" << std::endl;
            } else {
                std::cout << "Failed to send message" << std::endl;
            }
        }
        size = socket.read(buffer, 0x10);
        if (size > 0) {
            std::cout << "Received " << size << " bytes" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    std::cout << "Client terminated" << std::endl;
    return 0;
}
