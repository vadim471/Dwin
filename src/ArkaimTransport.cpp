#include "bridge/ArkaimTransport.hpp"
#include <iostream>

extern "C" {
#include <initp/system/time.h>
}

namespace bridge {

ArkaimTransport::ArkaimTransport(boost::asio::io_service& ios, const std::string& pipe_name)
    : m_ios(ios)
    , m_poll_timer(ios)
    , m_pipe_name(pipe_name)
    , m_connected(false)
    , m_running(false)
{
    std::cout << "[ArkaimTransport] Created with pipe name: " << pipe_name << std::endl;
}

ArkaimTransport::~ArkaimTransport() {
    stop();
}

void ArkaimTransport::start() {
    if (m_running.load()) {
        std::cout << "[ArkaimTransport] Already running" << std::endl;
        return;
    }
    
    m_running = true;
    std::cout << "[ArkaimTransport] Starting..." << std::endl;
    openPipe();
}

void ArkaimTransport::stop() {
    if (!m_running.load()) {
        return;
    }
    
    std::cout << "[ArkaimTransport] Stopping..." << std::endl;
    m_running = false;
    m_connected = false;
    
    m_poll_timer.cancel();
    closePipe();
}

void ArkaimTransport::openPipe() {
    std::cout << "[ArkaimTransport] Opening pipe: " << m_pipe_name << std::endl;
    std::cout << "[ArkaimTransport] Expected FIFO files:" << std::endl;
    std::cout << "[ArkaimTransport]   /tmp/" << m_pipe_name << "_r (for writing)" << std::endl;
    std::cout << "[ArkaimTransport]   /tmp/" << m_pipe_name << "_w (for reading)" << std::endl;
    
    // 1. Create pipe
    itp::named_pipe::uptr pipe = std::make_unique<itp::named_pipe>(m_pipe_name);
    std::cout << "[ArkaimTransport] Named pipe object created" << std::endl;
    
    // 2. Open pipe
    std::cout << "[ArkaimTransport] Attempting to open pipe..." << std::endl;
    if (!pipe->open()) {
        std::cerr << "[ArkaimTransport] ✗ Failed to open pipe '" << m_pipe_name << "'" << std::endl;
        std::cerr << "[ArkaimTransport] Possible reasons:" << std::endl;
        std::cerr << "[ArkaimTransport]   1. Arkaim processing is not running" << std::endl;
        std::cerr << "[ArkaimTransport]   2. FIFO files don't exist or have wrong permissions" << std::endl;
        std::cerr << "[ArkaimTransport]   3. Another process is already connected" << std::endl;
        std::cerr << "[ArkaimTransport] Check with: ls -la /tmp/" << m_pipe_name << "*" << std::endl;
        m_connected = false;
        return;
    }
    
    std::cout << "[ArkaimTransport] ✓ Pipe opened successfully" << std::endl;
    
    // 3. Create terminal
    m_terminal = std::make_unique<TerminalEntity>();
    std::cout << "[ArkaimTransport] TerminalEntity created" << std::endl;
    
    // 4. Attach pipe to terminal
    m_terminal->node().set_parent(std::move(pipe), nullptr);
    std::cout << "[ArkaimTransport] Pipe attached to terminal node" << std::endl;
    
    // 5. START POLLING (critical!)
    std::cout << "[ArkaimTransport] Starting pipe polling with 5ms interval..." << std::endl;
    schedulePipePoll();
    
    m_connected = true;
}

void ArkaimTransport::closePipe() {
    m_terminal.reset();
    std::cout << "[ArkaimTransport] Pipe closed" << std::endl;
}

void ArkaimTransport::schedulePipePoll() {
    if (!m_running.load()) {
        return;
    }
    
    m_poll_timer.expires_from_now(boost::posix_time::milliseconds(5));
    
    auto self = this;
    m_poll_timer.async_wait([self](const boost::system::error_code& ec) {
        self->onPollTimer(ec);
    });
}

void ArkaimTransport::onPollTimer(const boost::system::error_code& ec) {
    if (ec) {
        if (ec != boost::asio::error::operation_aborted) {
            std::cerr << "[ArkaimTransport] Poll timer error: " << ec.message() << std::endl;
        }
        return;
    }
    
    if (!m_running.load() || !m_terminal) {
        return;
    }
    
    // Poll ITP protocol
    static int poll_count = 0;
    if (poll_count++ % 200 == 0) { // Log every second (200 * 5ms = 1000ms)
        std::cout << "[ArkaimTransport] Polling... (count=" << poll_count << ")" << std::endl;
    }
    m_terminal->poll(sys_clock_ms());
    
    // Reschedule
    schedulePipePoll();
}

void ArkaimTransport::send(const RawData& data) {
    // This method is NOT used for ITP protocol
    // Sending happens through terminal->node().push_request() in ArkaimLogic
    std::cerr << "[ArkaimTransport] send() called but not used for ITP protocol" << std::endl;
}

void ArkaimTransport::setReceiveHandler(ReceiveHandler handler) {
    m_receive_handler = handler;
    // ITP protocol uses callbacks, not polling
    // So this handler is not actively used
}

} // namespace bridge
