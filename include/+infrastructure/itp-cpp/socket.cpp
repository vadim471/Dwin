#include <itp/socket.hpp>

int main(int argc, char** argv) {
    itp::socket socket;
    socket.listen(80);
    socket.connect("127.0.0.1", 80);
    itp::socket::service::ptr service = std::make_shared<itp::socket::service>();
    service->listen<itp::socket>(80, [] (itp::socket::uptr socket) -> void {});
    return 0;
}
