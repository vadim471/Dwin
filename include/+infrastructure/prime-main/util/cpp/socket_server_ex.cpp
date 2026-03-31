#include <initp/platform/socket.hpp>
#include <initp/platform/socket_detail.hpp>

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
    std::vector<initp::system::socket::uptr> sockets;
    initp::system::socket::service::ptr service = std::make_shared<initp::system::socket::service>();
    const size_t max_sockets = 10;
    sockets.resize(max_sockets);
    size_t count = 0;
    service->listen<initp::system::socket>(10135, [&sockets, &max_sockets, &count] (initp::system::socket::uptr socket) -> void {
        if (count < max_sockets) {
            sockets[count++] = std::move(socket);
        }
    });
    char data[0x100];
    size_t size;
    for (started = true; started;) {
        for (size_t i = 0; i < count; ++i) {
            size = sockets[i]->read(data, 0x100);
            if (size > 0) {
                sockets[i]->write(data, size);
                std::cout << "Received " << size << " bytes" << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    std::cout << "Server terminated" << std::endl;
    sockets.clear();
    service.reset();
    std::cout << "Memory freed" << std::endl;
    return 0;
}
