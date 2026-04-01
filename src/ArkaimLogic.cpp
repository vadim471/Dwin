#include "bridge/ArkaimLogic.hpp"
#include "bridge/MessageLayer.hpp"
#include "bridge/constant.hpp"
#include "bridge/json.hpp"
#include <iostream>

extern "C" {
#include <initp/system/time.h>
}

namespace bridge {

ArkaimLogic::ArkaimLogic(boost::asio::io_service& ios)
    : m_ios(ios)
    , m_reconnect_timer(ios)
    , m_pipe_name("InitPlusArkaimPayPipe")
    , m_connected(false)
    , m_payment_device_address(0)
    , m_controller_device_address(0)
{
    std::cout << "[ArkaimLogic] Created" << std::endl;
}

void ArkaimLogic::setTransport(std::shared_ptr<ArkaimTransport> transport) {
    m_transport = transport;
}

void ArkaimLogic::startLoop(MessageLayer& core) {
    std::cout << "[ArkaimLogic] Starting Arkaim integration" << std::endl;
    scheduleReconnect(core);
}

void ArkaimLogic::handle(const Message& msg, MessageLayer& core) {
    if (msg.type == PAY_TRANSACTION) {
        handleTransaction(msg, core);
    } else if (msg.type == "PAY_CONFIRM") {
        handleConfirm(msg, core);
    } else if (msg.type == "PAY_CANCEL") {
        handleCancel(msg, core);
    } else if (msg.type == "PIN_ENTERED") {
        handlePinEntered(msg, core);
    }
}

void ArkaimLogic::scheduleReconnect(MessageLayer& core) {
    // First attempt after 5 seconds to give Arkaim time to initialize
    static bool first_attempt = true;
    int delay = first_attempt ? 5 : 3;
    first_attempt = false;
    
    m_reconnect_timer.expires_from_now(boost::posix_time::seconds(delay));
    
    auto self = shared_from_this();
    m_reconnect_timer.async_wait([self, &core](const boost::system::error_code& ec) {
        if (!ec) {
            if (!self->m_connected) {
                self->ensurePipeSession(core);
            }
            self->scheduleReconnect(core);
        }
    });
}

void ArkaimLogic::ensurePipeSession(MessageLayer& core) {
    if (m_connected || !m_transport) {
        return;
    }
    
    std::cout << "[ArkaimLogic] ========================================" << std::endl;
    std::cout << "[ArkaimLogic] Attempting to connect to Arkaim pipe..." << std::endl;
    std::cout << "[ArkaimLogic] Pipe name: " << m_pipe_name << std::endl;
    
    // Transport will create terminal and start polling
    m_transport->start();
    
    if (!m_transport->isConnected()) {
        std::cerr << "[ArkaimLogic] ✗ Failed to connect to pipe, will retry in 3 seconds..." << std::endl;
        std::cerr << "[ArkaimLogic] ========================================" << std::endl;
        return;
    }
    
    std::cout << "[ArkaimLogic] ✓ Pipe connected, sending connect request..." << std::endl;
    std::cout << "[ArkaimLogic] Current time: " << sys_clock_ms() << std::endl;
    std::cout << "[ArkaimLogic] Terminal node address: " << (void*)&m_transport->terminal()->node() << std::endl;
    std::cout << "[ArkaimLogic] Terminal node root_address: " << (int)m_transport->terminal()->node().root_address() << std::endl;
    
    // Send request_connect
    auto self = shared_from_this();
    itp_error_code_t errc = m_transport->terminal()->node().request_connect(
        sys_clock_ms(),
        [self, &core](itp::root&, uint16_t error) {
            std::cout << "[ArkaimLogic] Connect callback received! error=" << error << std::endl;
            self->onConnect(error, core);
        }
    );
    
    if (errc) {
        std::cerr << "[ArkaimLogic] ✗ request_connect failed immediately with error: " << errc << std::endl;
        m_connected = false;
    } else {
        std::cout << "[ArkaimLogic] request_connect sent successfully, waiting for response..." << std::endl;
        std::cout << "[ArkaimLogic] Timeout is 5 seconds" << std::endl;
    }
}

void ArkaimLogic::onConnect(uint16_t error, MessageLayer& core) {
    if (error) {
        std::cerr << "[ArkaimLogic] ✗ Connect failed with error: " << error;
        if (error == 5) {
            std::cerr << " (TIMEOUT - Arkaim processing is not responding)" << std::endl;
            std::cerr << "[ArkaimLogic] Make sure Arkaim processing server is running!" << std::endl;
            std::cerr << "[ArkaimLogic] Expected: ./terminal pipe InitPlusArkaimPayPipe" << std::endl;
        } else {
            std::cerr << std::endl;
        }
        m_connected = false;
        return;
    }
    
    std::cout << "[ArkaimLogic] ✓ Connected successfully, requesting device list..." << std::endl;
    
    // Request device list
    itp::frame::uptr request = std::make_unique<itp::frame>(itp::CL2_CMD_GET_API_LIST);
    
    auto self = shared_from_this();
    m_transport->terminal()->node().push_request(
        std::move(request),
        m_transport->terminal()->node().root_address(),
        [self, &core](itp::root&, uint16_t error, itp::frame& response) {
            self->onGetApiList(error, response, core);
        }
    );
}

void ArkaimLogic::onGetApiList(uint16_t error, itp::frame& response, MessageLayer& core) {
    if (error) {
        std::cerr << "[ArkaimLogic] Get API list failed: " << error << std::endl;
        m_connected = false;
        return;
    }
    
    std::cout << "[ArkaimLogic] Parsing device list..." << std::endl;
    
    // Parse devices
    while (response.has_more()) {
        uint8_t address;
        uint32_t api;
        uint8_t type;
        std::string id;
        
        itp_error_code_t errc;
        std::tie(address, errc) = response.read_value<uint8_t>();
        if (!errc) std::tie(api, errc) = response.read_value<uint32_t>();
        if (!errc) std::tie(type, errc) = response.read_value<uint8_t>();
        if (!errc) std::tie(id, errc) = response.read_string();
        
        if (errc) break;
        
        std::cout << "[ArkaimLogic] Device: address=" << (int)address 
                  << ", api=0x" << std::hex << api << std::dec
                  << ", type=" << (int)type 
                  << ", id=" << id << std::endl;
        
        // Find payment device
        if (api & itp::CL2_API_PAYMENT) {
            m_payment_device_address = address;
            std::cout << "[ArkaimLogic] ✓ Found payment device at address " << (int)address << std::endl;
        }
        
        // Find controller
        if (api & itp::CL2_API_CONTROLLER) {
            m_controller_device_address = address;
            std::cout << "[ArkaimLogic] ✓ Found controller at address " << (int)address << std::endl;
        }
        
        // Find card readers
        if (api & itp::CL2_API_CONTACTLESS_READER) {
            m_card_reader_addresses.push_back(address);
            std::cout << "[ArkaimLogic] ✓ Found contactless reader at address " << (int)address << std::endl;
            
            // Subscribe to card events
            auto self = shared_from_this();
            if (itp::root::handler_proxy_t* proxy = m_transport->terminal()->node()
                    .create_handler_proxy(address, itp::CL2_CMD_CRD_CARD_DETECTED)) {
                proxy->connect([self, &core](itp::frame& event) {
                    self->onCardDetected(event, core);
                }, this);
                std::cout << "[ArkaimLogic] ✓ Subscribed to card events from address " << (int)address << std::endl;
            }
        }
    }
    
    m_connected = true;
    std::cout << "[ArkaimLogic] ✓✓✓ Ready for payments ✓✓✓" << std::endl;
}

void ArkaimLogic::handleTransaction(const Message& msg, MessageLayer& core) {
    if (!m_connected) {
        std::cerr << "[ArkaimLogic] ✗ Not connected to Arkaim processing" << std::endl;
        std::cerr << "[ArkaimLogic] Payment request rejected for order: " << msg.resource_id << std::endl;
        std::cerr << "[ArkaimLogic] Please start Arkaim processing server first!" << std::endl;
        sendPaymentResult(false, msg.resource_id, 0, "Payment processing unavailable - server not running", core);
        return;
    }
    
    std::string order_id = msg.resource_id;
    
    // Deserialize payment data
    PaymentRequestData payment_data;
    if (!deserializePaymentRequest(msg.payload, payment_data)) {
        std::cerr << "[ArkaimLogic] Failed to deserialize payment request" << std::endl;
        sendPaymentResult(false, order_id, 0, "Invalid payment data", core);
        return;
    }
    
    std::cout << "[ArkaimLogic] Payment request for order " << order_id << std::endl;
    std::cout << "[ArkaimLogic]   product_id: " << payment_data.product_id << std::endl;
    std::cout << "[ArkaimLogic]   amount: " << payment_data.amount_value 
              << "e-" << (int)payment_data.amount_decimal << std::endl;
    
    // Create pending payment
    PendingPayment pending;
    pending.order_id = order_id;
    pending.payment_data = payment_data;
    pending.card_data = m_current_card;
    
    // Save
    m_pending_payments[order_id] = pending;
    
    // Check: do we have a card?
    if (!m_current_card.has_card) {
        std::cout << "[ArkaimLogic] No card detected, waiting for card..." << std::endl;
        // TODO: Show on DWIN "Поднесите карту"
        return;
    }
    
    // Send transaction
    sendTransaction(pending, core);
}

void ArkaimLogic::onCardDetected(itp::frame& event, MessageLayer& core) {
    std::cout << "[ArkaimLogic] Card detected!" << std::endl;
    
    event.prepare_to_read();
    
    uint8_t type;
    std::vector<uint8_t> uid;
    itp_error_code_t errc;
    
    std::tie(type, errc) = event.read_value<uint8_t>();
    if (!errc) errc = event.read_array(uid);
    
    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to read card data" << std::endl;
        return;
    }
    
    std::cout << "[ArkaimLogic] Card UID: ";
    for (uint8_t b : uid) {
        printf("%02X ", b);
    }
    std::cout << std::endl;
    
    // Send resolve card type
    itp::frame::uptr request = std::make_unique<itp::frame>(itp::CL2_CMD_CTR_RESOLVE_CARD_TYPE);
    
    auto self = shared_from_this();
    m_transport->terminal()->node().push_request(
        std::move(request),
        m_controller_device_address,
        [self, &core](itp::root&, uint16_t error, itp::frame& response) {
            self->onCardResolve(error, response, core);
        }
    );
}

void ArkaimLogic::onCardResolve(uint16_t error, itp::frame& response, MessageLayer& core) {
    if (error) {
        std::cerr << "[ArkaimLogic] Card resolve failed: " << error << std::endl;
        return;
    }
    
    itp_error_code_t errc;
    
    std::tie(m_current_card.reader_address, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(m_current_card.reader_api, errc) = response.read_value<uint32_t>();
    
    uint8_t pay_address;
    uint32_t pay_api;
    uint8_t pay_type;
    uint16_t pay_service;
    
    if (!errc) std::tie(pay_address, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(pay_api, errc) = response.read_value<uint32_t>();
    if (!errc) std::tie(pay_type, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(pay_service, errc) = response.read_value<uint16_t>();
    if (!errc) std::tie(m_current_card.card_number, errc) = response.read_string();
    
    int32_t card_issuer;
    if (!errc) std::tie(card_issuer, errc) = response.read_value<int32_t>();
    if (!errc) errc = response.read_array(m_current_card.card_data);
    
    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to parse card resolve response" << std::endl;
        return;
    }
    
    m_current_card.has_card = true;
    
    std::cout << "[ArkaimLogic] ✓ Card resolved:" << std::endl;
    std::cout << "[ArkaimLogic]   reader_address: " << (int)m_current_card.reader_address << std::endl;
    std::cout << "[ArkaimLogic]   card_number: " << m_current_card.card_number << std::endl;
    std::cout << "[ArkaimLogic]   card_data size: " << m_current_card.card_data.size() << std::endl;
    
    // Check: is there a pending payment?
    if (!m_pending_payments.empty()) {
        std::cout << "[ArkaimLogic] Found pending payment, processing..." << std::endl;
        
        // Take first pending payment
        auto& pending = m_pending_payments.begin()->second;
        pending.card_data = m_current_card;
        
        sendTransaction(pending, core);
    }
}

void ArkaimLogic::sendTransaction(const PendingPayment& payment, MessageLayer& core) {
    std::cout << "[ArkaimLogic] Sending transaction for order " << payment.order_id << std::endl;
    
    itp::frame::uptr request = std::make_unique<itp::frame>(itp::CL2_CMD_PAY_TRANSACTION);
    
    // Write parameters
    request->write_value(payment.card_data.reader_address);
    request->write_value(payment.card_data.reader_api);
    request->write_string(payment.card_data.card_number);
    request->write_array(payment.card_data.card_data);
    request->write_array(payment.pin_data);
    request->write_value(sys_clock_ms());
    request->write_string(payment.payment_data.product_id);
    request->write_value(payment.payment_data.price_value);
    request->write_value(payment.payment_data.price_decimal);
    request->write_value(payment.payment_data.volume_value);
    request->write_value(payment.payment_data.volume_decimal);
    request->write_value(payment.payment_data.amount_value);
    request->write_value(payment.payment_data.amount_decimal);
    
    auto self = shared_from_this();
    std::string order_id = payment.order_id;
    m_transport->terminal()->node().push_request(
        std::move(request),
        m_payment_device_address,
        [self, &core, order_id](itp::root&, uint16_t error, itp::frame& response) {
            self->onTransactionResponse(error, response, core, order_id);
        }
    );
    
    std::cout << "[ArkaimLogic] Transaction request sent, waiting for response..." << std::endl;
}

void ArkaimLogic::onTransactionResponse(uint16_t error, itp::frame& response, 
                                        MessageLayer& core, const std::string& order_id) {
    std::cout << "[ArkaimLogic] Transaction response received for order " << order_id << std::endl;
    
    // Check for PIN required
    if (error == itp::CL2_ERR_PAY_PIN_REQUIRED) {
        std::cout << "[ArkaimLogic] PIN required for this transaction" << std::endl;
        requestPinFromUser(order_id, core);
        return;
    }
    
    if (error) {
        std::cerr << "[ArkaimLogic] Transaction failed with error: " << error << std::endl;
        sendPaymentResult(false, order_id, 0, "Transaction error: " + std::to_string(error), core);
        
        // Remove pending payment
        m_pending_payments.erase(order_id);
        return;
    }
    
    // Parse response
    itp_error_code_t errc;
    uint8_t code;
    std::string receipt_wide;
    std::string message;
    int32_t internal_code;
    std::string internal_message;
    
    std::tie(code, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(receipt_wide, errc) = response.read_wide_string();
    if (!errc) std::tie(message, errc) = response.read_string();
    if (!errc) std::tie(internal_code, errc) = response.read_value<int32_t>();
    if (!errc) std::tie(internal_message, errc) = response.read_string();
    
    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to parse transaction response" << std::endl;
        sendPaymentResult(false, order_id, 0, "Failed to parse response", core);
        m_pending_payments.erase(order_id);
        return;
    }
    
    if (code != 0) {
        std::cerr << "[ArkaimLogic] Payment declined: code=" << (int)code 
                  << ", message=" << message << std::endl;
        sendPaymentResult(false, order_id, 0, message, core);
        m_pending_payments.erase(order_id);
        return;
    }
    
    // Success - read transaction details
    uint64_t transaction_id;
    uint32_t price_v, volume_v, amount_v;
    uint8_t price_e, volume_e, amount_e;
    
    std::tie(transaction_id, errc) = response.read_value<uint64_t>();
    if (!errc) std::tie(price_v, errc) = response.read_value<uint32_t>();
    if (!errc) std::tie(price_e, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(volume_v, errc) = response.read_value<uint32_t>();
    if (!errc) std::tie(volume_e, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(amount_v, errc) = response.read_value<uint32_t>();
    if (!errc) std::tie(amount_e, errc) = response.read_value<uint8_t>();
    
    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to parse transaction details" << std::endl;
        sendPaymentResult(false, order_id, 0, "Failed to parse transaction details", core);
        m_pending_payments.erase(order_id);
        return;
    }
    
    // Save mapping
    m_order_to_transaction[order_id] = transaction_id;
    
    std::cout << "[ArkaimLogic] ✓✓✓ Payment SUCCESS ✓✓✓" << std::endl;
    std::cout << "[ArkaimLogic]   order_id: " << order_id << std::endl;
    std::cout << "[ArkaimLogic]   transaction_id: " << transaction_id << std::endl;
    std::cout << "[ArkaimLogic]   amount: " << amount_v << "e-" << (int)amount_e << std::endl;
    
    sendPaymentResult(true, order_id, transaction_id, message, core);
    
    // Remove pending payment
    m_pending_payments.erase(order_id);
}

void ArkaimLogic::requestPinFromUser(const std::string& order_id, MessageLayer& core) {
    std::cout << "[ArkaimLogic] Requesting PIN from user for order " << order_id << std::endl;
    
    // TODO: Send message to DWIN to show PIN entry page
    Message msg;
    msg.type = "SHOW_PIN_PAGE";
    msg.source = PIPE_LAYER;
    msg.resource_id = order_id;
    
    core.sendToLogicLayer(UART_LAYER, msg);
}

void ArkaimLogic::sendPaymentResult(bool success, const std::string& order_id,
                                    uint64_t transaction_id, const std::string& message,
                                    MessageLayer& core) {
    nlohmann::json result = {
        {"success", success},
        {"order_id", order_id},
        {"transaction_id", transaction_id},
        {"message", message}
    };
    
    std::string json_str = result.dump();
    
    Message response_msg;
    response_msg.type = "PAY_TRANSACTION_RESPONSE";
    response_msg.source = PIPE_LAYER;
    response_msg.resource_id = order_id;
    response_msg.payload = Bytes(json_str.begin(), json_str.end());
    
    core.sendToLogicLayer(HTTP_LAYER, response_msg);
}

void ArkaimLogic::handleConfirm(const Message& msg, MessageLayer& core) {
    std::string order_id = msg.resource_id;
    
    auto it = m_order_to_transaction.find(order_id);
    if (it == m_order_to_transaction.end()) {
        std::cerr << "[ArkaimLogic] No transaction found for order " << order_id << std::endl;
        return;
    }
    
    uint64_t transaction_id = it->second;
    
    std::cout << "[ArkaimLogic] Confirming transaction " << transaction_id 
              << " for order " << order_id << std::endl;
    
    // TODO: Implement PAY_CONFIRM
    std::cout << "[ArkaimLogic] TODO: Implement PAY_CONFIRM" << std::endl;
}

void ArkaimLogic::handleCancel(const Message& msg, MessageLayer& core) {
    std::string order_id = msg.resource_id;
    
    auto it = m_order_to_transaction.find(order_id);
    if (it == m_order_to_transaction.end()) {
        std::cerr << "[ArkaimLogic] No transaction found for order " << order_id << std::endl;
        return;
    }
    
    uint64_t transaction_id = it->second;
    
    std::cout << "[ArkaimLogic] Cancelling transaction " << transaction_id 
              << " for order " << order_id << std::endl;
    
    // TODO: Implement PAY_CANCEL
    std::cout << "[ArkaimLogic] TODO: Implement PAY_CANCEL" << std::endl;
}

void ArkaimLogic::handlePinEntered(const Message& msg, MessageLayer& core) {
    std::string order_id = msg.resource_id;
    
    std::cout << "[ArkaimLogic] PIN entered for order " << order_id << std::endl;
    
    // TODO: Retry transaction with PIN
    std::cout << "[ArkaimLogic] TODO: Retry transaction with PIN" << std::endl;
}

void ArkaimLogic::onConfirmResponse(uint16_t error, itp::frame& response, MessageLayer& core) {
    // TODO: Implement
}

void ArkaimLogic::onCancelResponse(uint16_t error, itp::frame& response, MessageLayer& core) {
    // TODO: Implement
}

} // namespace bridge
