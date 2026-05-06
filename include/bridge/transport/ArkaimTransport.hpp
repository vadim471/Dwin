#pragma once

#include "ITransport.hpp"

#include <itp/named_pipe.hpp>
#include <itp/itp.hpp>

#include <atomic>

namespace bridge {

class ArkaimTransport : public ITransport {
public:
    explicit ArkaimTransport(itp::named_pipe::uptr&& pipe);
    ~ArkaimTransport() override = default;

    // ITransport
    void start() override;
    void stop() override;
    void send(const RawData& raw_data) override;
    void setReceiveHandler(ReceiveHandler handler) override;

    // ArkaimLogic needs the endpoint to pass to itp::entity
    itp::endpoint::uptr takeEndpoint();
    bool isConnected() const;

private:
    itp::named_pipe::uptr m_pipe;
    ReceiveHandler m_handler;
    std::atomic<bool> m_running{false};
    bool m_endpoint_taken = false;
};

using ArkaimTransportPtr = std::shared_ptr<ArkaimTransport>;

} // namespace bridge
