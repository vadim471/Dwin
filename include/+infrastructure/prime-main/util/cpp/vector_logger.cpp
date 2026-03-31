#include <initp/log/vector/logger.hpp>
#include <initp/platform/tools.hpp>

#include <csignal>

bool started = true;
void on_signal(int) { started = false; }

int main(int argc, char** argv) {

    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);

    // Create instance
    initp::log::vector::logger instance;

    // Initialize
    /*instance.set_certificate(
        "cert/ca.crt",
        "cert/cl.crt",
        "cert/key.pem"
    );*/
    instance.initialize(
        "10.9.1.213",
        "9876",
        "prime",
        "TEST-TOOL",
        5000,
        1000,
        2
    );

    // Start & enable
    instance.start();

    // Log messages
    for (size_t i = 0; i < 10; ++i) {
        if (i > 0) {
            std::cout << "Uptime: " + std::to_string(i) + "s" << std::endl;
            instance.trace("main", "Uptime: ", i, "s");
        } else {
            std::cout << "Started" << std::endl;
            instance.info("main", "Started");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    std::cout << "Stopped" << std::endl;
    instance.info("main", "Stopped");

    // Disable, flush & stop
    instance.disable();
    instance.flush();
    instance.stop();
    return 0;
}
