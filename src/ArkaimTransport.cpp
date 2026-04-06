#include "bridge/ArkaimTransport.hpp"

#include <iostream>

namespace bridge {

ArkaimTransport::ArkaimTransport(itp::named_pipe::uptr&& pipe)
    : m_pipe(std::move(pipe))
{
}

void ArkaimTransport::start() {
    m_running = true;
    std::cout << "[ArkaimTransport] Started" << std::endl;
}

void ArkaimTransport::stop() {
    m_running = false;
    std::cout << "[ArkaimTransport] Stopped" << std::endl;
}

void ArkaimTransport::send(const RawData& /*raw_data*/) {
    // Not used directly — ArkaimLogic communicates via itp::entity/node
}

void ArkaimTransport::setReceiveHandler(ReceiveHandler handler) {
    m_handler = std::move(handler);
}

itp::endpoint::uptr ArkaimTransport::takeEndpoint() {
    if (m_endpoint_taken || !m_pipe) {
        std::cerr << "[ArkaimTransport] Endpoint already taken or pipe is null" << std::endl;
        return nullptr;
    }
    m_endpoint_taken = true;
    return std::move(m_pipe);
}

bool ArkaimTransport::isConnected() const {
    return m_running.load() && m_endpoint_taken;
}

} // namespace bridge
