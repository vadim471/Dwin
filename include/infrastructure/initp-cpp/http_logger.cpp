#include <initp/http/logger.hpp>
#include <initp/platform/tools.hpp>

#include <csignal>

bool started = true;
void on_signal(int) { started = false; }

int main(int argc, char** argv) {

    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);

    // Create instance
    initp::http::logger instance;

    // Initialize
    instance.set_certificate(
        "cert/ca.crt",
        "cert/cl.crt",
        "cert/key.pem"
    );
    instance.initialize(
        "10.10.1.84",
        "15346",
        "/api/Logger/AddRecords",
        initp::system::tools::get_host_name(),
        "TEST-TOOL",
        5000,
        1000,
        2
    );

    // Start & enable
    instance.start();

    // Log messages
    for (size_t i = 0; started; ++i) {
        if (i > 0) {
            std::cout << "Uptime: " + std::to_string(i) + "s" << std::endl;
            instance.queue_message(SYSTEM_LEVEL_INFO, "main", "Uptime: " + std::to_string(i) + "s", initp::system::time::milliseconds());
        } else {
            std::cout << "Started" << std::endl;
            instance.queue_message(SYSTEM_LEVEL_INFO, "main", "Started", initp::system::time::milliseconds());
        }
        initp::system::time::sleep_for(1000);
    }
    std::cout << "Stopped" << std::endl;
    instance.queue_message(SYSTEM_LEVEL_INFO, "main", "Stopped", initp::system::time::milliseconds());

    // Disable, flush & stop
    instance.disable();
    instance.flush();
    instance.stop();
    return 0;
}
