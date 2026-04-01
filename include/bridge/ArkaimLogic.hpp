#pragma once

#include "ILogicHandler.hpp"
#include "ArkaimTransport.hpp"
#include "PaymentData.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <itp/itp.hpp>
#include <itp/cl2.h>
#include <unordered_map>
#include <memory>

namespace bridge {

struct CardData {
    uint8_t reader_address = 0;
    uint32_t reader_api = 0;
    std::string card_number;
    std::vector<uint8_t> card_data;
    bool has_card = false;
};

struct PendingPayment {
    std::string order_id;
    PaymentRequestData payment_data;
    CardData card_data;
    std::vector<uint8_t> pin_data;
};

class ArkaimLogic : public std::enable_shared_from_this<ArkaimLogic>,
                    public ILogicHandler {
public:
    ArkaimLogic(boost::asio::io_service& ios);
    ~ArkaimLogic() override = default;
    
    void startLoop(MessageLayer& core);
    void handle(const Message& msg, MessageLayer& core) override;
    void setTransport(std::shared_ptr<ArkaimTransport> transport);

private:
    // Lifecycle
    void scheduleReconnect(MessageLayer& core);
    void ensurePipeSession(MessageLayer& core);
    
    // ITP callbacks
    void onConnect(uint16_t error, MessageLayer& core);
    void onGetApiList(uint16_t error, itp::frame& response, MessageLayer& core);
    
    // Card reader events
    void onCardDetected(itp::frame& event, MessageLayer& core);
    void onCardResolve(uint16_t error, itp::frame& response, MessageLayer& core);
    
    // Payment operations
    void handleTransaction(const Message& msg, MessageLayer& core);
    void handleConfirm(const Message& msg, MessageLayer& core);
    void handleCancel(const Message& msg, MessageLayer& core);
    void handlePinEntered(const Message& msg, MessageLayer& core);
    
    // Payment callbacks
    void onTransactionResponse(uint16_t error, itp::frame& response, 
                               MessageLayer& core, const std::string& order_id);
    void onConfirmResponse(uint16_t error, itp::frame& response, MessageLayer& core);
    void onCancelResponse(uint16_t error, itp::frame& response, MessageLayer& core);
    
    // Helpers
    void sendTransaction(const PendingPayment& payment, MessageLayer& core);
    void requestPinFromUser(const std::string& order_id, MessageLayer& core);
    void sendPaymentResult(bool success, const std::string& order_id,
                          uint64_t transaction_id, const std::string& message,
                          MessageLayer& core);
    
    boost::asio::io_service& m_ios;
    boost::asio::deadline_timer m_reconnect_timer;
    std::shared_ptr<ArkaimTransport> m_transport;
    
    std::string m_pipe_name;
    bool m_connected;
    uint8_t m_payment_device_address;
    uint8_t m_controller_device_address;
    
    // Card reader state
    std::vector<uint8_t> m_card_reader_addresses;
    CardData m_current_card;
    
    // Payment state
    std::unordered_map<std::string, uint64_t> m_order_to_transaction;
    std::unordered_map<std::string, PendingPayment> m_pending_payments;
};

} // namespace bridge
