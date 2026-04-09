#include "bridge/ArkaimLogic.hpp"
#include "bridge/constant.hpp"
#include "bridge/MessageLayer.hpp"
#include "bridge/json.hpp"

extern "C" {
#include <initp/system/time.h>
}

#include <itp/tools.hpp>
#include <itp/named_pipe.hpp>
#include <initp/platform/named_pipe.hpp>
#include <initp/system/time.hpp>

#include <boost/make_unique.hpp>

#include <iostream>
#include <sys/stat.h>

// Explicit bind macro for ArkaimLogic (RBIND uses entity's private self_type)
#define ABIND(f, ...) std::bind(&ArkaimLogic::f, this, ##__VA_ARGS__)

namespace bridge {

using namespace std::placeholders;

// ============================================================
//  Construction / Destruction
// ============================================================

ArkaimLogic::ArkaimLogic(boost::asio::io_service& ios,
                         ArkaimTransportPtr transport)
    : entity(itp::CL2_API_ACTIVE_TERMINAL, itp::CL2_DEV_GENERIC)
    , m_ios(ios)
    , m_transport(std::move(transport))
{
}

ArkaimLogic::~ArkaimLogic() {
    m_started = false;
    if (m_poll_thread.joinable()) {
        m_poll_thread.join();
    }
}

// ============================================================
//  ILogicHandler: handle()
// ============================================================

void ArkaimLogic::handle(const Message& msg, MessageLayer& core) {
    m_core = &core;

    if (msg.type == PAY_TRANSACTION) {
        PaymentRequestData payment;
        if (!deserializePaymentRequest(msg.payload, payment)) {
            std::cerr << "[ArkaimLogic] Failed to deserialize payment request" << std::endl;
            sendPaymentResponse(msg.resource_id, false, 0, "Deserialize error");
            return;
        }

        std::cout << "[ArkaimLogic] PAY_TRANSACTION received for order " << msg.resource_id
                  << ", product=" << payment.product_id
                  << ", price=" << payment.price_value << "e" << (int)payment.price_decimal
                  << ", volume=" << payment.volume_value << "e" << (int)payment.volume_decimal
                  << ", amount=" << payment.amount_value << "e" << (int)payment.amount_decimal
                  << std::endl;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_pending.active = true;
            m_pending.order_id = msg.resource_id;
            m_pending.payment = payment;
            m_pending.transaction_time = initp::system::time::now();
        }

        tryProcessPendingPayment();

    } else if (msg.type == PAY_CONFIRM) {
        handleConfirm(msg);
    } else if (msg.type == USER_TOUCH_BASIC_TOUCH_CANCEL_TRANSACTION_BUTTON) {
        handleCancel(msg);
    } else if (msg.type == PAY_PIN_ENTERED) {
        handlePinEntered(msg);
    } else {
        std::cout << "[ArkaimLogic] Unknown message type: " << msg.type << std::endl;
    }
}

// ============================================================
//  startLoop — connect to processing via ITP
// ============================================================

void ArkaimLogic::startLoop(MessageLayer& core) {
    m_core = &core;

    itp::named_pipe::uptr pipe = boost::make_unique<itp::named_pipe>("InitPlusArkaimPayPipe");

    if (!pipe->open()) {
        std::cerr << "[ArkaimLogic] Failed to open pipe" << std::endl;
        return;
    }
    std::cout << "[ArkaimLogic] Pipe opened" << std::endl;

    this->node_.set_parent(std::move(pipe), nullptr);
    std::cout << "[ArkaimLogic] set_parent done" << std::endl;

    itp_error_code_t errc = this->node_.request_connect(
        initp::system::time::now(),
        ABIND(onConnect, _2)
    );
    if (errc) {
        std::cerr << "[ArkaimLogic] request_connect error=" << errc << std::endl;
        return;
    }

    // 5. Start poll (manager::start, manager.cpp:66)
    m_started = true;
    m_poll_thread = std::thread([this]() {
        while (m_started.load()) {
            entity::poll(sys_clock_ms());
            sys_sleep_for(5);
        }
    });

    std::cout << "[ArkaimLogic] Started, waiting for connection..." << std::endl;
}

uint32_t ArkaimLogic::status() const {
    return itp::CL2_DST_ONLINE;
}

void ArkaimLogic::onConnect(uint16_t error) {
    if (error) {
        std::cerr << "[ArkaimLogic] Connection failed, error=" << error << std::endl;
        m_started = false;
        return;
    }

    std::cout << "[ArkaimLogic] Connected successfully" << std::endl;

    itp::frame::uptr request = boost::make_unique<itp::frame>(itp::CL2_CMD_GET_API_LIST);
    itp_error_code_t errc = this->node_.push_request(
        std::move(request),
        this->node_.root_address(),
        ABIND(onGetApiList, _2, _3)
    );

    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to request API list, error=" << errc << std::endl;
        m_started = false;
    }
}

void ArkaimLogic::onGetApiList(uint16_t error, itp::frame& response) {
    if (error) {
        std::cerr << "[ArkaimLogic] Get API list error=" << error << std::endl;
        m_started = false;
        return;
    }

    std::cout << "[ArkaimLogic] Parsing API list..." << std::endl;

    itp_error_code_t errc;
    uint8_t address, type;
    uint32_t api;
    std::string id;

    while (response.has_more()) {
        std::tie(address, errc) = response.read_value<uint8_t>();
        if (!errc) std::tie(api, errc) = response.read_value<uint32_t>();
        if (!errc) std::tie(type, errc) = response.read_value<uint8_t>();
        if (!errc) std::tie(id, errc) = response.read_string();
        if (errc) {
            std::cerr << "[ArkaimLogic] Failed to read device entry" << std::endl;
            m_started = false;
            return;
        }

        DeviceInfo dev;
        dev.address = address;
        dev.api = api;
        dev.type = type;
        dev.id = id;
        m_devices.push_back(dev);

        std::cout << "[ArkaimLogic] Device: addr=" << (int)address
                  << " api=0x" << std::hex << api << std::dec
                  << " type=" << (int)type
                  << " id=\"" << id << "\"" << std::endl;

        // Subscribe to status changes
        {
            std::vector<uint16_t> commands;
            commands.push_back(itp::CL2_CMD_ALL_STATUS_CHANGED);
            errc = this->node_.listen_remote(
                address, commands,
                ABIND(onListenResult, _2, address,
                      itp::tools::get_device_name(type),
                      itp::cl2_command_t::CL2_CMD_ALL_STATUS_CHANGED)
            );
            if (!errc) {
                if (itp::root::handler_proxy_t* proxy =
                        this->node_.create_handler_proxy(address, itp::CL2_CMD_ALL_STATUS_CHANGED)) {
                    proxy->connect(ABIND(onDeviceStatus, _1, address, type), this);
                }
            }
        }

        // Helper lambda for subscribing to events
        auto listen = [this](uint8_t addr, uint8_t dev_type,
                             const std::vector<uint16_t>& cmds) {
            for (uint16_t cmd : cmds) {
                std::vector<uint16_t> current_command;
                current_command.push_back(cmd);
                if (!this->node_.listen_remote(
                        addr, current_command,
                        std::bind(&ArkaimLogic::onListenResult, this, _2, addr,
                                  itp::tools::get_device_name(dev_type),
                                  (itp::cl2_command_t)cmd))) {
                    this->node_.create_handler_proxy(addr, cmd);
                }
            }
        };

        // Subscribe to contactless reader card-detected events
        if (api & itp::CL2_API_CONTACTLESS_READER) {
            std::vector<uint16_t> commands;
            commands.push_back(itp::CL2_CMD_CRD_CARD_DETECTED);
            listen(address, type, commands);

            if (itp::root::handler_proxy_t* proxy =
                    this->node_.create_handler_proxy(address, itp::CL2_CMD_CRD_CARD_DETECTED)) {
                proxy->connect(ABIND(onCardDetected, _1), this);
                std::cout << "[ArkaimLogic] Listening for card on device addr=" << (int)address << std::endl;
            }
        }

        // Subscribe to pinpad events
        if (api & itp::CL2_API_PINPAD) {
            std::vector<uint16_t> commands;
            commands.push_back(itp::CL2_CMD_PIN_KEY_PRESSED);
            commands.push_back(itp::CL2_CMD_PIN_COMPLETE);
            listen(address, type, commands);

            if (itp::root::handler_proxy_t* proxy =
                    this->node_.create_handler_proxy(address, itp::CL2_CMD_PIN_COMPLETE)) {
                proxy->connect(ABIND(onPinReady, _1), this);
                std::cout << "[ArkaimLogic] Listening for PIN on device addr=" << (int)address << std::endl;
            }

            m_pinpad_address = address;
            m_has_pinpad = true;
        }

        // Track controller
        if (api & itp::CL2_API_CONTROLLER) {
            m_controller_address = address;
            m_has_controller = true;
            std::cout << "[ArkaimLogic] Controller found at addr=" << (int)address << std::endl;
        }

        // Subscribe to barcode scanner
        if (api & itp::CL2_API_BARCODE_SCANNER) {
            std::vector<uint16_t> commands;
            commands.push_back(itp::CL2_CMD_BARCODE_SCANNED);
            listen(address, type, commands);
        }
    }

    std::cout << "[ArkaimLogic] Total " << m_devices.size() << " devices found" << std::endl;
}

void ArkaimLogic::onListenResult(uint16_t error, uint8_t address,
                                  const std::string& device_name,
                                  itp::cl2_command_t event) {
    if (error) {
        std::cerr << "[ArkaimLogic] Listen error=" << error
                  << " addr=" << (int)address
                  << " device=" << device_name << std::endl;
    }
}

void ArkaimLogic::onDeviceStatus(itp::frame& event, uint8_t address, uint8_t type) {
    itp_error_code_t errc;
    uint32_t dev_status;
    std::tie(dev_status, errc) = event.read_value<uint32_t>();
    if (errc) return;

    std::cout << "[ArkaimLogic] Device status: addr=" << (int)address
              << " status=0x" << std::hex << dev_status << std::dec << std::endl;
}


void ArkaimLogic::onCardDetected(itp::frame& event) {
    event.prepare_to_read();

    itp_error_code_t errc;
    uint8_t card_type;
    std::tie(card_type, errc) = event.read_value<uint8_t>();
    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to read card type" << std::endl;
        return;
    }

    std::vector<uint8_t> uid;
    errc = event.read_array(uid);
    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to read card UID" << std::endl;
        return;
    }

    std::cout << "[ArkaimLogic] Card detected, type=" << (int)card_type
              << ", UID size=" << uid.size() << std::endl;

    if (!m_has_controller) {
        std::cerr << "[ArkaimLogic] No controller to resolve card" << std::endl;
        return;
    }

    itp::frame::uptr request(new itp::frame(itp::CL2_CMD_CTR_RESOLVE_CARD_TYPE));
    errc = this->node_.push_request(
        std::move(request),
        m_controller_address,
        ABIND(onCardResolve, _1, _2, _3)
    );

    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to send resolve card, error=" << errc << std::endl;
    }
}

void ArkaimLogic::onCardResolve(itp::root& root, uint16_t error, itp::frame& response) {
    if (error) {
        std::cerr << "[ArkaimLogic] Card resolve error=" << error << std::endl;
        return;
    }

    itp_error_code_t errc;
    uint32_t pay_api;
    uint8_t pay_type;

    std::tie(m_reader_address, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(m_reader_api, errc) = response.read_value<uint32_t>();
    if (!errc) std::tie(m_pay_address, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(pay_api, errc) = response.read_value<uint32_t>();
    if (!errc) std::tie(pay_type, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(m_pay_service, errc) = response.read_value<uint16_t>();
    if (!errc) std::tie(m_card_number, errc) = response.read_string();
    if (!errc) std::tie(m_card_issuer, errc) = response.read_value<int32_t>();
    if (!errc) errc = response.read_array(m_card_data);

    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to parse card resolve response" << std::endl;
        return;
    }

    m_card_resolved = true;
    m_pin_data.clear();
    m_pin_required_on_transaction = false;

    // Проверяем card_data на PinRequiredOnInitializeTransaction
    try {
        std::string card_json_str(m_card_data.begin(), m_card_data.end());
        auto card_json = nlohmann::json::parse(card_json_str);
        if (card_json.contains("PinRequiredOnInitializeTransaction")) {
            m_pin_required_on_transaction = card_json.value("PinRequiredOnInitializeTransaction", false);
            std::cout << "[ArkaimLogic] PIN required on transaction (from card data)" << std::endl;
        }
    } catch (...) {
    }

    std::cout << "[ArkaimLogic] Card resolved:"
              << " reader_addr=" << (int)m_reader_address
              << " reader_api=0x" << std::hex << m_reader_api << std::dec
              << " pay_addr=" << (int)m_pay_address
              << " pay_service=" << m_pay_service
              << " card=\"" << m_card_number << "\""
              << " card_data_size=" << m_card_data.size()
              << std::endl;

    Message msg;
    msg.source = PIPE_LAYER;
    msg.type = PAY_CARD_RESOLVED;

    msg.payload.push_back(m_pin_required_on_transaction ? 1 : 0);

    m_core->sendToLogicLayer(HTTP_LAYER, msg);

    if (!m_pin_required_on_transaction) {
        tryProcessPendingPayment();
    }
}

void ArkaimLogic::tryProcessPendingPayment() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_pending.active || !m_card_resolved) return;

    sendTransaction(m_pending.order_id, m_pending.payment);
}

void ArkaimLogic::sendTransaction(const std::string& order_id,
                                   const PaymentRequestData& payment) {
    std::cout << "[ArkaimLogic] Sending PAY_TRANSACTION for order " << order_id << std::endl;

    itp::frame::uptr request(new itp::frame(itp::CL2_CMD_PAY_TRANSACTION));
    request->write_value(m_reader_address);
    request->write_value(m_reader_api);
    request->write_string(m_card_number);
    request->write_array(m_card_data);
    request->write_array(m_pin_data);
    request->write_value(m_pending.transaction_time);
    request->write_string(payment.product_id);
    request->write_value(payment.price_value);
    request->write_value(payment.price_decimal);
    request->write_value(payment.volume_value);
    request->write_value(payment.volume_decimal);
    request->write_value(payment.amount_value);
    request->write_value(payment.amount_decimal);

    std::string captured_order_id = order_id;
    itp_error_code_t errc = this->node_.push_request(
        std::move(request),
        m_pay_address,
        [this, captured_order_id](itp::root& root, uint16_t err, itp::frame& resp) {
            this->onTransactionResponse(root, err, resp, captured_order_id);
        }
    );

    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to send transaction, error=" << errc << std::endl;
        sendPaymentResponse(order_id, false, 0, "ITP send error");
        m_pending.active = false;
    }
}

void ArkaimLogic::onTransactionResponse(itp::root& root, uint16_t error,
                                          itp::frame& response, std::string order_id) {
    if (error == itp::CL2_ERR_PAY_PIN_REQUIRED) {
        std::cout << "[ArkaimLogic] PIN required for order " << order_id << std::endl;
        processPinRequired(response, order_id);
        return;
    }

    if (error) {
        std::cerr << "[ArkaimLogic] Transaction error=" << error
                  << " for order " << order_id << std::endl;
        sendPaymentResponse(order_id, false, 0,
                            "Transaction error " + std::to_string(error));
        m_pending.active = false;
        return;
    }

    itp_error_code_t errc;
    uint8_t code;
    std::string receipt;
    std::string message;
    int32_t int_code;
    std::string int_message;
    uint64_t transaction_id = 0;

    std::tie(code, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(receipt, errc) = response.read_wide_string();
    if (!errc) std::tie(message, errc) = response.read_string();
    if (!errc) std::tie(int_code, errc) = response.read_value<int32_t>();
    if (!errc) std::tie(int_message, errc) = response.read_string();

    if (code == itp::CL2_PAY_ERR_NONE) {
        uint32_t price_v = 0, volume_v = 0, amount_v = 0;
        uint8_t price_e = 0, volume_e = 0, amount_e = 0;

        if (!errc) std::tie(transaction_id, errc) = response.read_value<uint64_t>();
        if (!errc) std::tie(price_v, errc) = response.read_value<uint32_t>();
        if (!errc) std::tie(price_e, errc) = response.read_value<uint8_t>();
        if (!errc) std::tie(volume_v, errc) = response.read_value<uint32_t>();
        if (!errc) std::tie(volume_e, errc) = response.read_value<uint8_t>();
        if (!errc) std::tie(amount_v, errc) = response.read_value<uint32_t>();
        if (!errc) std::tie(amount_e, errc) = response.read_value<uint8_t>();
    }

    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to parse transaction response" << std::endl;
        sendPaymentResponse(order_id, false, 0, "Parse error");
        m_pending.active = false;
        return;
    }

    std::cout << "[ArkaimLogic] Transaction response: code=" << (int)code
              << " message=\"" << message << "\""
              << " int_code=" << int_code << std::endl;

    if (code == itp::CL2_PAY_ERR_NONE) {
        std::cout << "[ArkaimLogic] Transaction SUCCESS, id=" << transaction_id << std::endl;
        m_order_transactions[order_id] = transaction_id;
        sendPaymentResponse(order_id, true, transaction_id, message);
        // m_pending остаётся active — данные нужны для cancel/confirm
    } else {
        std::cerr << "[ArkaimLogic] Transaction FAILED, code=" << (int)code
                  << " msg=\"" << message << "\"" << std::endl;
        sendPaymentResponse(order_id, false, 0, message);
        m_pending.active = false;
    }
}

void ArkaimLogic::sendPaymentResponse(const std::string& order_id, bool success,
                                        uint64_t transaction_id, const std::string& message) {
    if (!m_core) return;

    nlohmann::json json;
    json["success"] = success;
    json["transaction_id"] = transaction_id;
    json["message"] = message;

    std::string body = json.dump();

    Message msg;
    msg.source = PIPE_LAYER;
    msg.type = PAY_TRANSACTION_RESPONSE;
    msg.resource_id = order_id;
    msg.payload.assign(body.begin(), body.end());

    m_core->sendToLogicLayer(HTTP_LAYER, msg);
}

void ArkaimLogic::handleConfirm(const Message& msg) {
    std::string order_id = msg.resource_id;

    auto it = m_order_transactions.find(order_id);
    if (it == m_order_transactions.end()) {
        std::cerr << "[ArkaimLogic] No transaction found for order " << order_id << std::endl;
        return;
    }

    uint64_t transaction_id = it->second;

    PaymentRequestData payment;
    deserializePaymentRequest(msg.payload, payment);

    std::cout << "[ArkaimLogic] Confirming transaction " << transaction_id
              << " for order " << order_id << std::endl;

    itp::frame::uptr request = boost::make_unique<itp::frame>(itp::CL2_CMD_PAY_CONFIRM);
    request->write_value(m_reader_address);
    request->write_value(m_reader_api);
    request->write_string(m_card_number);
    request->write_array(m_card_data);
    request->write_value(m_pending.transaction_time);
    request->write_string(payment.product_id);
    request->write_value(transaction_id);
    request->write_value(payment.price_value);
    request->write_value(payment.price_decimal);
    request->write_value(payment.volume_value);
    request->write_value(payment.volume_decimal);
    request->write_value(payment.amount_value);
    request->write_value(payment.amount_decimal);
    request->write_value(payment.not_complete);
    request->write_value(payment.fact_volume_value);
    request->write_value(payment.fact_volume_decimal);
    request->write_value(payment.fact_amount_value);
    request->write_value(payment.fact_amount_decimal);

    std::string captured_order_id = order_id;
    itp_error_code_t errc = this->node_.push_request(
        std::move(request),
        m_pay_address,
        [this, captured_order_id](itp::root&, uint16_t err, itp::frame& resp) {
            this->onConfirmResponse(err, resp, captured_order_id);
        }
    );

    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to send confirm, error=" << errc << std::endl;
    }
}

void ArkaimLogic::handleCancel(const Message& msg) {
    std::string order_id;
    PaymentRequestData payment;

    if (!msg.payload.empty() && deserializePaymentRequest(msg.payload, payment)) {
        order_id = msg.resource_id;
    } else {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_pending.active) {
            std::cerr << "[ArkaimLogic] Cancel: no payload and no pending payment" << std::endl;
            return;
        }
        order_id = m_pending.order_id;
        payment = m_pending.payment;
    }

    auto it = m_order_transactions.find(order_id);
    if (it == m_order_transactions.end()) {
        std::cerr << "[ArkaimLogic] No transaction found for cancel, order " << order_id << std::endl;
        return;
    }

    uint64_t transaction_id = it->second;

    std::cout << "[ArkaimLogic] Cancelling transaction " << transaction_id
              << " for order " << order_id << std::endl;

    itp::frame::uptr request = boost::make_unique<itp::frame>(itp::CL2_CMD_PAY_CANCEL);
    request->write_value(m_reader_address);
    request->write_value(m_reader_api);
    request->write_string(m_card_number);
    request->write_array(m_card_data);
    request->write_value(m_pending.transaction_time);
    request->write_string(payment.product_id);
    request->write_value(transaction_id);
    request->write_value(payment.price_value);
    request->write_value(payment.price_decimal);
    request->write_value(payment.volume_value);
    request->write_value(payment.volume_decimal);
    request->write_value(payment.amount_value);
    request->write_value(payment.amount_decimal);

    std::string captured_order_id = order_id;
    itp_error_code_t errc = this->node_.push_request(
        std::move(request),
        m_pay_address,
        [this, captured_order_id](itp::root&, uint16_t err, itp::frame& resp) {
            this->onCancelResponse(err, resp, captured_order_id);
        }
    );

    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to send cancel, error=" << errc << std::endl;
    }
}

void ArkaimLogic::onConfirmResponse(uint16_t error, itp::frame& response,
                                      std::string order_id) {
    if (error) {
        std::cerr << "[ArkaimLogic] Confirm error=" << error
                  << " for order " << order_id << std::endl;
    } else {
        std::cout << "[ArkaimLogic] Confirm SUCCESS for order " << order_id << std::endl;

        if (m_core) {
            Message msg;
            msg.source = PIPE_LAYER;
            msg.type = PAY_CONFIRM_RESPONSE_SUCCESS;
            msg.resource_id = order_id;
            m_core->sendToLogicLayer(UART_LAYER, msg);
            m_core->sendToLogicLayer(HTTP_LAYER, msg);
        }
    }

    m_card_resolved = false;
    m_card_number.clear();
    m_card_data.clear();
    m_pin_data.clear();
    m_pending.active = false;
    m_order_transactions.erase(order_id);
}

void ArkaimLogic::onCancelResponse(uint16_t error, itp::frame& response,
                                     std::string order_id) {
    if (error) {
        std::cerr << "[ArkaimLogic] Cancel error=" << error
                  << " for order " << order_id << std::endl;
    } else {
        std::cout << "[ArkaimLogic] Cancel SUCCESS for order " << order_id << std::endl;

        if (m_core) {
            Message msg;
            msg.source = PIPE_LAYER;
            msg.type = PAY_CANCEL_RESPONSE_SUCCESS;
            msg.resource_id = order_id;
            m_core->sendToLogicLayer(UART_LAYER, msg);
            m_core->sendToLogicLayer(HTTP_LAYER, msg);
        }
    }

    m_card_resolved = false;
    m_card_number.clear();
    m_card_data.clear();
    m_pin_data.clear();
    m_pending.active = false;
    m_order_transactions.erase(order_id);
}

void ArkaimLogic::handlePinEntered(const Message& msg) {
    m_pin_data.assign(msg.payload.begin(), msg.payload.end());
    m_waiting_pin = false;

    std::cout << "[ArkaimLogic] PIN entered, length=" << m_pin_data.size() << std::endl;

    tryProcessPendingPayment();
}

void ArkaimLogic::processPinRequired(itp::frame& response, const std::string& order_id) {
    itp_error_code_t errc;
    uint8_t type;

    std::tie(type, errc) = response.read_value<uint8_t>();
    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to read PIN type" << std::endl;
        sendPaymentResponse(order_id, false, 0, "PIN type read error");
        m_pending.active = false;
        return;
    }

    m_pin_type = type;
    m_waiting_pin = true;

    std::cout << "[ArkaimLogic] PIN required, type=" << (int)type << std::endl;

    if (!m_has_pinpad) {
        std::cerr << "[ArkaimLogic] No pinpad available" << std::endl;
        sendPaymentResponse(order_id, false, 0, "No pinpad");
        m_pending.active = false;
        m_waiting_pin = false;
        return;
    }

    uint16_t pin_cmd = (type == itp::CL2_PIN_TYPE_PLAIN)
                     ? itp::CL2_CMD_PIN_GET_PLAIN
                     : itp::CL2_CMD_PIN_GET_DUKPT;

    itp::frame::uptr request(new itp::frame(pin_cmd));
    errc = request->write_value<uint16_t>(30);

    if (!errc) {
        errc = this->node_.push_request(
            std::move(request),
            m_pinpad_address,
            ABIND(onPinRequest, _2, _3)
        );
    }

    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to request PIN, error=" << errc << std::endl;
        sendPaymentResponse(order_id, false, 0, "PIN request error");
        m_pending.active = false;
        m_waiting_pin = false;
    }
}

void ArkaimLogic::onPinRequest(uint16_t error, itp::frame& response) {
    if (error) {
        std::cerr << "[ArkaimLogic] PIN request failed, error=" << error << std::endl;
    } else {
        std::cout << "[ArkaimLogic] PIN entry started, waiting for input..." << std::endl;
    }
}

void ArkaimLogic::onPinReady(itp::frame& event) {
    event.prepare_to_read();

    uint32_t dukpt_id = 0;
    itp_error_code_t errc = event.read_array(m_pin_data);
    if (!errc && m_pin_type == itp::CL2_PIN_TYPE_DUKPT) {
        errc = event.read_value(dukpt_id);
    }

    if (errc) {
        std::cerr << "[ArkaimLogic] Failed to read PIN data" << std::endl;
        if (m_pending.active) {
            sendPaymentResponse(m_pending.order_id, false, 0, "PIN read error");
            m_pending.active = false;
        }
        m_waiting_pin = false;
        return;
    }

    std::cout << "[ArkaimLogic] PIN received, data size=" << m_pin_data.size() << std::endl;
    m_waiting_pin = false;

    if (m_pending.active) {
        sendTransaction(m_pending.order_id, m_pending.payment);
    }
}

} // namespace bridge

#undef ABIND
