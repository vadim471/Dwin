#pragma once

#include "ILogicHandler.hpp"
#include "ArkaimTransport.hpp"
#include "PaymentData.hpp"
#include "Printer.hpp"

#include <itp/entity.hpp>
#include <itp/itp.hpp>
#include <itp/cl2.h>
#include <itp/command.h>

#include <initp/generic/event_proxy.hpp>

#include <boost/asio.hpp>

#include <thread>
#include <atomic>
#include <map>
#include <mutex>
#include <memory>

namespace bridge {

class ArkaimLogic : public ILogicHandler, public itp::entity {

public:
    ArkaimLogic(boost::asio::io_service& ios,
                ArkaimTransportPtr transport);
    ~ArkaimLogic() override;

    // ILogicHandler
    void handle(const Message& msg, MessageLayer& core) override;

    // Start the ITP connection and poll loop
    void startLoop(MessageLayer& core);

    // itp::entity
    uint32_t status() const override;

private:
    // ---- Connection flow (mirrors manager.cpp) ----
    void onConnect(uint16_t error);
    void onGetApiList(uint16_t error, itp::frame& response);
    void onListenResult(uint16_t error, uint8_t address,
                        const std::string& device_name, itp::cl2_command_t event);
    void onDeviceStatus(itp::frame& event, uint8_t address, uint8_t type);

    // ---- Card flow (mirrors controller.cpp) ----
    void onCardDetected(itp::frame& event);
    void onCardResolve(itp::root& root, uint16_t error, itp::frame& response);

    // ---- Payment flow ----
    void sendTransaction(const std::string& order_id, const PaymentRequestData& payment);
    void onTransactionResponse(itp::root& root, uint16_t error,
                               itp::frame& response, std::string order_id);
    void sendPaymentResponse(const std::string& order_id, bool success,
                             uint64_t transaction_id, const std::string& message);

    // ---- Confirm / Cancel ----
    void handleConfirm(const Message& msg);
    void handleCancel(const Message& msg);
    void onConfirmResponse(uint16_t error, itp::frame& response, std::string order_id);
    void onCancelResponse(uint16_t error, itp::frame& response, std::string order_id);

    // ---- PIN handling ----
    void handlePinEntered(const Message& msg);
    void processPinRequired(itp::frame& response, const std::string& order_id);
    void onPinRequest(uint16_t error, itp::frame& response);
    void onPinReady(itp::frame& event);

    // ---- Helpers ----
    void tryProcessPendingPayment();

    // ---- Printer methods ----
    void handlePrintReceipt(const Message& msg);

    // ---- Balance methods ----
    void handleGetBalance(const Message& msg);
    void sendGetBalance(const std::string& product_id, uint32_t price_value, uint8_t price_decimal);
    void onGetBalanceResponse(uint16_t error, itp::frame& response);

    MessageLayer* m_core = nullptr;
    boost::asio::io_service& m_ios;
    ArkaimTransportPtr m_transport;
    std::thread m_poll_thread;
    std::atomic<bool> m_started{false};
    std::mutex m_mutex;

    // ---- Device addresses discovered from API list ----
    struct DeviceInfo {
        uint8_t address = 0;
        uint32_t api = 0;
        uint8_t type = 0;
        std::string id;
    };
    std::vector<DeviceInfo> m_devices;

    uint8_t m_controller_address = 0;
    bool m_has_controller = false;
    uint8_t m_pinpad_address = 0;
    bool m_has_pinpad = false;
    uint8_t m_printer_address = 0;
    bool m_has_printer = false;
    std::unique_ptr<Printer> m_printer;

    // ---- Card state (from on_card_resolve) ----
    bool m_card_resolved = false;
    uint8_t m_reader_address = 0;
    uint32_t m_reader_api = 0;
    uint8_t m_pay_address = 0;
    uint16_t m_pay_service = 0;
    std::string m_card_number;
    std::vector<uint8_t> m_card_data;
    int32_t m_card_issuer = 0;

    // ---- PIN state ----
    std::vector<uint8_t> m_pin_data;
    uint8_t m_pin_type = 0;
    bool m_waiting_pin = false;

    // ---- Pending payment (waiting for card) ----
    struct PendingPayment {
        bool active = false;
        std::string order_id;
        PaymentRequestData payment;
        uint64_t transaction_time = 0;
    };
    PendingPayment m_pending;

    // ---- Order -> Transaction mapping ----
    std::map<std::string, uint64_t> m_order_transactions;
};

using ArkaimLogicPtr = std::shared_ptr<ArkaimLogic>;

} // namespace bridge
