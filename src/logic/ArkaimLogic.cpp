#include "bridge/logic/ArkaimLogic.hpp"
#include "bridge/core/constant.hpp"
#include "bridge/core/MessageLayer.hpp"
#include "bridge/json.hpp"
#include "bridge/core/utility.hpp"
#include "bridge/payment/PaymentData.hpp"
#include "bridge/transport/ArkaimTransport.hpp"

extern "C" {
#include <initp/system/time.h>
}

#include <itp/tools.hpp>
#include <itp/named_pipe.hpp>
#include <initp/platform/named_pipe.hpp>
#include <initp/system/time.hpp>

#include <boost/make_unique.hpp>

#include <iostream>

// Explicit bind macro for ArkaimLogic (RBIND uses entity's private self_type)
#define ABIND(f, ...) std::bind(&ArkaimLogic::f, this, ##__VA_ARGS__)

namespace bridge {
    using namespace std::placeholders;

    ArkaimLogic::ArkaimLogic(boost::asio::io_service &ios,
                             ArkaimTransportPtr transport)
        : entity(itp::CL2_API_ACTIVE_TERMINAL, itp::CL2_DEV_GENERIC)
          , ios(ios)
          , transport(std::move(transport)) {
    }

    ArkaimLogic::~ArkaimLogic() {
        started = false;
        if (poll_thread.joinable()) {
            poll_thread.join();
        }
    }

    void ArkaimLogic::handle(const Message &msg, MessageLayer &core) {
        m_core = &core;

        if (msg.type == CLEAR_CARD_DATA_REQUEST) {
            resetCardState();
        } else if (msg.type == PAY_TRANSACTION) {
            PaymentRequestData payment;
            if (!deserializePaymentRequest(msg.payload, payment)) {
                std::cerr << "[ArkaimLogic] Failed to deserialize payment request" << std::endl;
                sendPaymentResponse(msg.resource_id, false, 0, "Deserialize error");
                return;
            }

            std::cout << "[ArkaimLogic] PAY_TRANSACTION received for order " << msg.resource_id
                    << ", product=" << payment.product_id
                    << ", price=" << payment.price_value << "e" << (int) payment.price_decimal
                    << ", volume=" << payment.volume_value << "e" << (int) payment.volume_decimal
                    << ", amount=" << payment.amount_value << "e" << (int) payment.amount_decimal
                    << std::endl;

            {
                std::lock_guard<std::mutex> lock(mutex);
                pending_payment.active = true;
                pending_payment.order_id = msg.resource_id;
                pending_payment.payment = payment;
                pending_payment.transaction_time = initp::system::time::now();
            }

            tryProcessPendingPayment();
        }
        else if (msg.type == PAY_CONFIRM) {
            handleConfirm(msg);
        } else if (msg.type == USER_TOUCH_BASIC_TOUCH_CANCEL_TRANSACTION_BUTTON) {
            handleCancel(msg);
        } else if (msg.type == PAY_PIN_ENTERED) {
            handlePinEntered(msg);
        } else if (msg.type == PAY_PRINT_RECEIPT) {
            std::string receipt_text(msg.payload.begin(), msg.payload.end());
            handlePrintReceipt(receipt_text);
        } else if (msg.type == PRINT_DEBIT_RECEIPT) {
            handlePrintDebitReceipt(msg);
        } else if (msg.type == PRINT_REFUND_RECEIPT) {
            //handlePrintRefundReceipt(msg);
        } else if (msg.type == PAY_GET_BALANCE) {
            handleGetBalance(msg);
        } else {
            std::cout << "[ArkaimLogic] Unknown message type: " << msg.type << std::endl;
        }
    }

    void ArkaimLogic::startLoop(MessageLayer &core) {
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

        started = true;
        poll_thread = std::thread([this]() {
            while (started.load()) {
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
            started = false;
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
            started = false;
        }
    }

    void ArkaimLogic::onGetApiList(uint16_t error, itp::frame &response) {
        if (error) {
            std::cerr << "[ArkaimLogic] Get API list error=" << error << std::endl;
            started = false;
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
                started = false;
                return;
            }

            DeviceInfo dev;
            dev.address = address;
            dev.api = api;
            dev.type = type;
            dev.id = id;
            devices.push_back(dev);

            std::cout << "[ArkaimLogic] Device: addr=" << (int) address
                    << " api=0x" << std::hex << api << std::dec
                    << " type=" << (int) type
                    << " id=\"" << id << "\"" << std::endl;

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
                    if (itp::root::handler_proxy_t *proxy =
                            this->node_.create_handler_proxy(address, itp::CL2_CMD_ALL_STATUS_CHANGED)) {
                        proxy->connect(ABIND(onDeviceStatus, _1, address, type), this);
                            }
                }
            }

            auto listen = [this](uint8_t addr, uint8_t dev_type,
                                 const std::vector<uint16_t> &cmds) {
                for (uint16_t cmd: cmds) {
                    std::vector<uint16_t> current_command;
                    current_command.push_back(cmd);
                    if (!this->node_.listen_remote(
                        addr, current_command,
                        std::bind(&ArkaimLogic::onListenResult, this, _2, addr,
                                  itp::tools::get_device_name(dev_type),
                                  (itp::cl2_command_t) cmd))) {
                        this->node_.create_handler_proxy(addr, cmd);
                                  }
                }
            };

            if (api & itp::CL2_API_CONTACTLESS_READER) {
                std::vector<uint16_t> commands;
                commands.push_back(itp::CL2_CMD_CRD_CARD_DETECTED);
                listen(address, type, commands);

                if (itp::root::handler_proxy_t *proxy =
                        this->node_.create_handler_proxy(address, itp::CL2_CMD_CRD_CARD_DETECTED)) {
                    proxy->connect(ABIND(onCardDetected, _1), this);
                    std::cout << "[ArkaimLogic] Listening for card on device addr=" << (int) address << std::endl;
                        }
            }

            if (api & itp::CL2_API_PINPAD) {
                std::vector<uint16_t> commands;
                commands.push_back(itp::CL2_CMD_PIN_KEY_PRESSED);
                commands.push_back(itp::CL2_CMD_PIN_COMPLETE);
                listen(address, type, commands);

                if (itp::root::handler_proxy_t *proxy =
                        this->node_.create_handler_proxy(address, itp::CL2_CMD_PIN_COMPLETE)) {
                    proxy->connect(ABIND(onPinReady, _1), this);
                    std::cout << "[ArkaimLogic] Listening for PIN on device addr=" << (int) address << std::endl;
                        }

                pinpad_address = address;
                has_pinpad = true;
            }

            if (api & itp::CL2_API_CONTROLLER) {
                controller_address = address;
                has_controller = true;
                std::cout << "[ArkaimLogic] Controller found at addr=" << (int) address << std::endl;
            }

            // Subscribe to barcode scanner
            if (api & itp::CL2_API_BARCODE_SCANNER) {
                std::vector<uint16_t> commands;
                commands.push_back(itp::CL2_CMD_BARCODE_SCANNED);
                listen(address, type, commands);
            }

            if (api & itp::CL2_API_PRINTER) {
                std::vector<uint16_t> commands;
                commands.push_back(itp::CL2_CMD_PRT_PRINT_TEXT);
                commands.push_back(itp::CL2_CMD_PRT_CUT_PAPER);
                listen(address, type, commands);

                printer_address = address;
                has_printer = true;
                printer.reset(new Printer(this->node_, address));
                std::cout << "[ArkaimLogic] Printer found at addr=" << (int) address << std::endl;
            }
        }

        std::cout << "[ArkaimLogic] Total " << devices.size() << " devices found" << std::endl;

        if (m_core) {
            Message msg;
            msg.source = PIPE_LAYER;
            msg.type = ARKAIM_INITIALIZED;
            
            m_core->sendToLogicLayer(PRIME_HTTP_LAYER, msg);
        }
    }

    void ArkaimLogic::onListenResult(uint16_t error, uint8_t address,
                                     const std::string &device_name,
                                     itp::cl2_command_t event) {
        if (error) {
            std::cerr << "[ArkaimLogic] Listen error=" << error
                    << " addr=" << (int) address
                    << " device=" << device_name << std::endl;
        }
    }

    void ArkaimLogic::onDeviceStatus(itp::frame &event, uint8_t address, uint8_t type) {
        itp_error_code_t errc;
        uint32_t dev_status;
        std::tie(dev_status, errc) = event.read_value<uint32_t>();
        if (errc) return;

        std::cout << "[ArkaimLogic] Device status: addr=" << (int) address
                << " status=0x" << std::hex << dev_status << std::dec << std::endl;
    }

    void ArkaimLogic::onCardDetected(itp::frame &event) {

        if (card_resolved || card_resolve_in_progress || pending_payment.active) {
            std::cout << "[ArkaimLogic] Ignoring card tap: already resolved or processing." << std::endl;
            return;
        }

        card_resolve_in_progress = true;

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

        std::cout << "[ArkaimLogic] Card detected, type=" << (int) card_type
                << ", UID size=" << uid.size() << std::endl;

        if (!has_controller) {
            std::cerr << "[ArkaimLogic] No controller to resolve card" << std::endl;
            return;
        }
        // Оповестить логику о приложенной карте. Только если нет данных предыдущей.
        if (card_number.empty()) {
            Message card_detected;
            card_detected.source = PIPE_LAYER;
            card_detected.type = ON_CARD_DETECTED;
            if (m_core) {
                m_core->sendToLogicLayer(PRIME_HTTP_LAYER, card_detected);
            }
        }
        //

        itp::frame::uptr request(new itp::frame(itp::CL2_CMD_CTR_RESOLVE_CARD_TYPE));
        errc = this->node_.push_request(
            std::move(request),
            controller_address,
            ABIND(onCardResolve, _1, _2, _3)
        );

        if (errc) {
            std::cerr << "[ArkaimLogic] Failed to send resolve card, error=" << errc << std::endl;
        }
    }

    void ArkaimLogic::onCardResolve(itp::root &root, uint16_t error, itp::frame &response) {
        card_resolve_in_progress = false;
        if (error) {
            std::cerr << "[ArkaimLogic] Card resolve error= " << error << std::endl;
            resetCardState();
            handleCardError(error);
            return;
        }

        itp_error_code_t errc;
        uint32_t pay_api;
        uint8_t pay_type;

        std::tie(reader_address, errc) = response.read_value<uint8_t>();
        if (!errc) std::tie(reader_api, errc) = response.read_value<uint32_t>();
        if (!errc) std::tie(pay_address, errc) = response.read_value<uint8_t>();
        if (!errc) std::tie(pay_api, errc) = response.read_value<uint32_t>();
        if (!errc) std::tie(pay_type, errc) = response.read_value<uint8_t>();
        if (!errc) std::tie(pay_service, errc) = response.read_value<uint16_t>();
        if (!errc) std::tie(card_number, errc) = response.read_string();
        if (!errc) std::tie(card_issuer, errc) = response.read_value<int32_t>();
        if (!errc) errc = response.read_array(card_data);

        if (errc) {
            std::cerr << "[ArkaimLogic] Failed to parse card resolve response" << std::endl;
            return;
        }

        card_resolved = true;
        pin_data.clear();

        std::cout << "[ArkaimLogic] Card resolved:"
                << " reader_addr=" << (int) reader_address
                << " reader_api=0x" << std::hex << reader_api << std::dec
                << " pay_addr=" << (int) pay_address
                << " pay_service=" << pay_service
                << " card=\"" << card_number << "\""
                << " card_data_size=" << card_data.size()
                << std::endl;

        Message msg;
        msg.source = PIPE_LAYER;
        msg.type = PAY_CARD_RESOLVED;

        m_core->sendToLogicLayer(PRIME_HTTP_LAYER, msg);
        tryProcessPendingPayment();
    }

    void ArkaimLogic::tryProcessPendingPayment() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!pending_payment.active || !card_resolved) return;

        sendTransaction(pending_payment.order_id, pending_payment.payment);
    }

    void ArkaimLogic::sendTransaction(const std::string &order_id,
                                      const PaymentRequestData &payment) {
        std::cout << "[ArkaimLogic] Sending PAY_TRANSACTION for order " << order_id << std::endl;

        itp::frame::uptr request(new itp::frame(itp::CL2_CMD_PAY_TRANSACTION));
        request->write_value(reader_address);
        request->write_value(reader_api);
        request->write_string(card_number);
        request->write_array(card_data);
        request->write_array(pin_data);
        request->write_value(pending_payment.transaction_time);
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
            pay_address,
            [this, captured_order_id](itp::root &root, uint16_t err, itp::frame &resp) {
                this->onTransactionResponse(root, err, resp, captured_order_id);
            }
        );

        if (errc) {
            std::cerr << "[ArkaimLogic] Failed to send transaction, error=" << errc << std::endl;
            sendPaymentResponse(order_id, false, 0, "ITP send error");
            pending_payment.active = false;
        }
    }

    void ArkaimLogic::onTransactionResponse(itp::root &root, uint16_t error,
                                            itp::frame &response, std::string order_id) {
        if (error == itp::CL2_ERR_PAY_PIN_REQUIRED) {
            std::cout << "[ArkaimLogic] PIN required for order " << order_id << std::endl;
            processPinRequired(response, &order_id, GET_PAYMENT_TYPE);
            return;
        }

        if (error) {
            std::cerr << "[ArkaimLogic] Transaction error=" << error
                    << " for order " << order_id << std::endl;
            sendPaymentResponse(order_id, false, 0,
                                "Transaction error " + std::to_string(error));
            pending_payment.active = false;
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
            pending_payment.active = false;
            return;
        }

        std::cout << "[ArkaimLogic] Transaction response: code=" << (int) code
                << " message=\"" << message << "\""
                << " int_code=" << int_code << std::endl;

        if (code == itp::CL2_PAY_ERR_NONE) {
            std::cout << "[ArkaimLogic] Transaction SUCCESS, id=" << transaction_id << std::endl;

            // Заносим заказ в мапу.
            ActiveOrder active_order;
            active_order.card_data = card_data;
            active_order.card_number = card_number;
            active_order.pin_data = pin_data;
            active_order.transaction_id = transaction_id;
            active_order.transaction_time = pending_payment.transaction_time;

            order_transactions[order_id] = active_order;
            //
            // Подчищаем текущие данные карты после оплаты. Не надо, при кнопке "новый заказ" они подчистятся.
            //resetCardState();

            sendPaymentResponse(order_id, true, transaction_id, message);
            // m_pending остаётся active — данные нужны для cancel/confirm
        } else if (code == itp::CL2_PAY_ERR_INVALID_PIN) {
            std::cerr << "[ArkaimLogic] Transaction FAILED, Incorrect PIN" << (int) code
                    << " msg=\"" << message << "\"" << std::endl;
            sendPaymentResponse(order_id, false, 0, message);
        } else if (code == itp::CL2_PAY_ERR_GENERIC_ERROR) {
            std::cerr << "[ArkaimLogic] Transaction FAILED, NOT ENOUGH MONEY" << (int) code
                    << " msg=\"" << message << "\"" << std::endl;
            sendPaymentResponse(order_id, false, 0, message);
        } else {
            std::cerr << "[ArkaimLogic] Transaction FAILED, code=" << (int) code
                    << " msg=\"" << message << "\"" << std::endl;
            sendPaymentResponse(order_id, false, 0, message);
            pending_payment.active = false;
        }
    }

    void ArkaimLogic::sendPaymentResponse(const std::string &order_id, bool success,
                                          uint64_t transaction_id, const std::string &message) {
        if (!m_core) return;

        nlohmann::json json;
        json["success"] = success;
        json["transaction_id"] = transaction_id;
        json["card_number"] = card_number;
        
        json["message"] = message;

        std::string body = json.dump();

        Message msg;
        msg.source = PIPE_LAYER;
        msg.type = PAY_TRANSACTION_RESPONSE;
        msg.resource_id = order_id;
        msg.payload.assign(body.begin(), body.end());

        m_core->sendToLogicLayer(PRIME_HTTP_LAYER, msg);
    }

    void ArkaimLogic::handleConfirm(const Message &msg) {
        std::string order_id = msg.resource_id;

        auto it = order_transactions.find(order_id);
        if (it == order_transactions.end()) {
            std::cerr << "[ArkaimLogic] No transaction found for order " << order_id << std::endl;
            return;
        }

        const ActiveOrder &active_order = it->second;

        PaymentRequestData payment;
        deserializePaymentRequest(msg.payload, payment);

        std::cout << "[ArkaimLogic] Confirming transaction " << active_order.transaction_id
                << " for order " << order_id << std::endl;

        itp::frame::uptr request = boost::make_unique<itp::frame>(itp::CL2_CMD_PAY_CONFIRM);
        request->write_value(reader_address);
        request->write_value(reader_api);
        request->write_string(active_order.card_number);
        request->write_array(active_order.card_data);
        request->write_value(active_order.transaction_time);
        request->write_string(payment.product_id);
        request->write_value(active_order.transaction_id);
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
            pay_address,
            [this, captured_order_id](itp::root &, uint16_t err, itp::frame &resp) {
                this->onConfirmResponse(err, resp, captured_order_id);
            }
        );

        if (errc) {
            std::cerr << "[ArkaimLogic] Failed to send confirm, error=" << errc << std::endl;
        }
    }

    void ArkaimLogic::handleCancel(const Message &msg) {
        std::string order_id;
        PaymentRequestData payment;

        if (!msg.payload.empty() && deserializePaymentRequest(msg.payload, payment)) {
            order_id = msg.resource_id;
        } else {
            std::lock_guard<std::mutex> lock(mutex);
            if (!pending_payment.active) {
                std::cerr << "[ArkaimLogic] Cancel: no payload and no pending payment" << std::endl;
                return;
            }
            order_id = pending_payment.order_id;
            payment = pending_payment.payment;
        }

        auto it = order_transactions.find(order_id);
        if (it == order_transactions.end()) {
            std::cerr << "[ArkaimLogic] No transaction found for cancel, order " << order_id << std::endl;
            return;
        }

        const ActiveOrder &active_order = it->second;

        std::cout << "[ArkaimLogic] Cancelling transaction " << active_order.transaction_id
                << " for order " << order_id << std::endl;

        itp::frame::uptr request = boost::make_unique<itp::frame>(itp::CL2_CMD_PAY_CANCEL);
        request->write_value(reader_address);
        request->write_value(reader_api);
        request->write_string(active_order.card_number);
        request->write_array(active_order.card_data);
        request->write_value(active_order.transaction_time);
        request->write_string(payment.product_id);
        request->write_value(active_order.transaction_id);
        request->write_value(payment.price_value);
        request->write_value(payment.price_decimal);
        request->write_value(payment.volume_value);
        request->write_value(payment.volume_decimal);
        request->write_value(payment.amount_value);
        request->write_value(payment.amount_decimal);

        std::string captured_order_id = order_id;
        itp_error_code_t errc = this->node_.push_request(
            std::move(request),
            pay_address,
            [this, captured_order_id](itp::root &, uint16_t err, itp::frame &resp) {
                this->onCancelResponse(err, resp, captured_order_id);
            }
        );

        if (errc) {
            std::cerr << "[ArkaimLogic] Failed to send cancel, error=" << errc << std::endl;
        }
    }

    void ArkaimLogic::onConfirmResponse(uint16_t error, itp::frame &response,
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
                m_core->sendToLogicLayer(PRIME_HTTP_LAYER, msg);
            }
        }

        // card_resolved = false;
        // card_number.clear();
        // card_data.clear();
        // pin_data.clear();
        //resetCardState(); Подчищает после возврата пистолета - PrimeLogic сообщит
        pending_payment.active = false;
        order_transactions.erase(order_id);
    }

    void ArkaimLogic::onCancelResponse(uint16_t error, itp::frame &response,
                                       std::string order_id) {
        if (error) {
            std::cerr << "[ArkaimLogic] Cancel error=" << error
                    << " for order " << order_id << std::endl;
            return;
        }
        std::cout << "[ArkaimLogic] Cancel SUCCESS for order " << order_id << std::endl;

        if (m_core) {
            Message msg;
            msg.source = PIPE_LAYER;
            msg.type = PAY_CANCEL_RESPONSE_SUCCESS;
            msg.resource_id = order_id;
            m_core->sendToLogicLayer(PRIME_HTTP_LAYER, msg);
        }

        // card_resolved = false;
        // card_number.clear();
        // card_data.clear();
        // pin_data.clear();
        // resetCardState(); Подчищает после возврата пистолета - PrimeLogic сообщит
        pending_payment.active = false;
        order_transactions.erase(order_id);
    }

    void ArkaimLogic::handlePinEntered(const Message &msg) {
        pin_data.assign(msg.payload.begin(), msg.payload.end());
        std::cout << "[ArkaimLogic] PIN entered, length=" << pin_data.size() << std::endl;
        
        if (waiting_pin_for_balance) {
            waiting_pin_for_balance = false;
            sendGetBalance("ai92", 0, 0);
        } else {
            waiting_pin = false;
            tryProcessPendingPayment();
        }
    }

    void ArkaimLogic::processPinRequired(itp::frame &response, const std::string* order_id,
        const std::string &type_pin_required) {

        itp_error_code_t errc;
        uint8_t type;

        std::tie(type, errc) = response.read_value<uint8_t>();
        if (errc) {
            std::cerr << "[ArkaimLogic] Failed to read PIN type" << std::endl;
            if (type_pin_required == GET_BALANCE_TYPE) {
                sendGetBalanceResponse("PIN type read error", false);
            } else if (type_pin_required == GET_PAYMENT_TYPE) {
                sendPaymentResponse(*order_id, false, 0, "PIN type read error");
                pending_payment.active = false;
            }
            return;
        }

        pin_type = type;

        std::cout << "[ArkaimLogic] PIN required, type=" << (int) type << std::endl;

        if (!has_pinpad) {
            std::cerr << "[ArkaimLogic] No pinpad available" << std::endl;
            if (type_pin_required == GET_BALANCE_TYPE) {
                waiting_pin_for_balance = true;
                sendGetBalanceResponse(NO_PINPAD, false);
            } else if (type_pin_required == GET_PAYMENT_TYPE) {
                waiting_pin = true;
                sendPaymentResponse(*order_id, false, 0, NO_PINPAD);
            }
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
                pinpad_address,
                ABIND(onPinRequest, _2, _3)
            );
        }

        if (errc) {
            std::cerr << "[ArkaimLogic] Failed to request PIN, error=" << errc << std::endl;
            sendPaymentResponse(*order_id, false, 0, "PIN request error");
            pending_payment.active = false;
            waiting_pin = false;
            waiting_pin_for_balance = false;
        }
    }

    void ArkaimLogic::onPinRequest(uint16_t error, itp::frame &response) {
        if (error) {
            std::cerr << "[ArkaimLogic] PIN request failed, error=" << error << std::endl;
        } else {
            std::cout << "[ArkaimLogic] PIN entry started, waiting for input..." << std::endl;
        }
    }

    void ArkaimLogic::onPinReady(itp::frame &event) {
        event.prepare_to_read();

        uint32_t dukpt_id = 0;
        itp_error_code_t errc = event.read_array(pin_data);
        if (!errc && pin_type == itp::CL2_PIN_TYPE_DUKPT) {
            errc = event.read_value(dukpt_id);
        }

        if (errc) {
            std::cerr << "[ArkaimLogic] Failed to read PIN data" << std::endl;
            if (pending_payment.active) {
                sendPaymentResponse(pending_payment.order_id, false, 0, "PIN read error");
                pending_payment.active = false;
            }
            waiting_pin = false;
            return;
        }

        std::cout << "[ArkaimLogic] PIN received, data size=" << pin_data.size() << std::endl;
        waiting_pin = false;

        if (pending_payment.active) {
            sendTransaction(pending_payment.order_id, pending_payment.payment);
        }
    }

    void ArkaimLogic::handlePrintReceipt(std::string receipt_text) {
        if (!has_printer) {
            std::cerr << "[ArkaimLogic] No printer available" << std::endl;
            return;
        }

        std::cout << "[ArkaimLogic] Printing" << std::endl;
        std::string error_desc;
        printer->printText(receipt_text, [this, error_desc](bool success, const std::string &error) {
            if (success) {
                std::cout << "[ArkaimLogic] Receipt printed successfully" << std::endl;

                printer->cutPaper(true, [](bool cut_success, const std::string &cut_error) {
                    if (cut_success) {
                        std::cout << "[ArkaimLogic] Paper cut successfully" << std::endl;
                    } else {
                        std::cerr << "[ArkaimLogic] Failed to cut paper: " << cut_error << std::endl;
                    }
                });
            } else {
                std::cerr << "[ArkaimLogic] Failed to print receipt: " << error << std::endl;
            }
        });
    }

    void ArkaimLogic::handlePrintDebitReceipt(const Message& msg) {
        if (!has_printer) {
            std::cerr << "[ArkaimLogic] No printer available for debit receipt" << std::endl;
            return;
        }

        ReceiptData receipt;
        if (!deserializeReceiptData(msg.payload, receipt)) {
            std::cerr << "[ArkaimLogic] Failed to deserialize debit receipt data" << std::endl;
            return;
        }

        // Заполняем номер карты и transaction_id
        receipt.card_number = card_number;

        // Получаем transaction_id из маппинга order -> transaction
        auto it = order_transactions.find(msg.resource_id);
        if (it != order_transactions.end()) {
            receipt.transaction_id = it->second.transaction_id;
        }

        std::string receipt_text = utility::createDebitReceipt(
            receipt.address,
            receipt.card_number,
            receipt.product_name,
            "",
            receipt.volume_liters,
            receipt.price_per_liter,
            receipt.transaction_id,
            nullptr,
            nullptr,
            nullptr
        );

        std::cout << "[ArkaimLogic] Printing debit receipt for order " << msg.resource_id << std::endl;
        handlePrintReceipt(receipt_text);
    }

    void ArkaimLogic::handleGetBalance(const Message &msg) {
        if (!card_resolved) {
            std::cerr << "[ArkaimLogic] No card resolved for balance request" << std::endl;
            return;
        }

        std::cout << "[ArkaimLogic] Get balance request" << std::endl;

        // дефолтные значения для запроса баланса
        // В реальном случае можно передать product_id и цену через msg.payload
        sendGetBalance("ai92", 0, 0);
    }

    void ArkaimLogic::sendGetBalance(const std::string &product_id, uint32_t price_value, uint8_t price_decimal) {
        std::cout << "[ArkaimLogic] Sending GET_BALANCE request" << std::endl;

        itp::frame::uptr request(new itp::frame(itp::CL2_CMD_PAY_GET_INFO));
        request->write_value(reader_address);
        request->write_value(reader_api);
        request->write_string(card_number);
        request->write_array(card_data);
        request->write_array(pin_data);
        //
        // // Для DUKPT пин-кода нужен transaction ID, но если пин не передается, все равно пишем 0
        // if (m_pin_type == itp::CL2_PIN_TYPE_DUKPT || m_pin_data.empty()) {
        //     request->write_value<uint32_t>(0);
        // }

        request->write_string(product_id);
        request->write_value(price_value);
        request->write_value(price_decimal);

        itp_error_code_t errc = this->node_.push_request(
            std::move(request),
            pay_address,
            ABIND(onGetBalanceResponse, _2, _3)
        );

        if (errc) {
            std::cerr << "[ArkaimLogic] Failed to send get balance request, error=" << errc << std::endl;
        }
    }

    void ArkaimLogic::sendGetBalanceResponse(const std::string &message , bool success) {
        if (!m_core) return;

        nlohmann::json json;
        json["success"] = success;
        json["message"] = message;

        std::string body = json.dump();

        Message message_get_balance;
        message_get_balance.source = PIPE_LAYER;
        message_get_balance.type = ON_GET_BALANCE_RESPONSE;
        message_get_balance.payload.assign(body.begin(), body.end());

        m_core->sendToLogicLayer(PRIME_HTTP_LAYER, message_get_balance);
    }

    void ArkaimLogic::onGetBalanceResponse(uint16_t error, itp::frame &response) {
        if (error == itp::CL2_ERR_PAY_PIN_REQUIRED) {
            std::cout << "[ArkaimLogic] PIN required for balance request" << std::endl;
            processPinRequired(response, nullptr, GET_BALANCE_TYPE);
            return;
        }

        if (error) {
            std::cerr << "[ArkaimLogic] Get balance error=" << error << std::endl;
            resetCardState();
            sendGetBalanceResponse(NON_RECOGNIZED_ERROR, false);
            return;
        }

        itp_error_code_t errc;
        uint8_t code;
        std::string receipt;
        std::string message;
        int32_t int_code;
        std::string int_message;

        std::tie(code, errc) = response.read_value<uint8_t>();
        if (!errc) std::tie(receipt, errc) = response.read_wide_string();
        if (!errc) std::tie(message, errc) = response.read_string();
        if (!errc) std::tie(int_code, errc) = response.read_value<int32_t>();
        if (!errc) std::tie(int_message, errc) = response.read_string();

        if (errc) {
            std::cerr << "[ArkaimLogic] Failed to parse balance response" << std::endl;
            return;
        }

        std::cout << "[ArkaimLogic] Balance response: code=" << (int) code
                << " message=\"" << message << "\"" << std::endl;

        //m_parsed_additional_card_info = utility::parseTerminalBalanceReceipt(receipt);
        
        if (code == itp::CL2_PAY_ERR_NONE) {
            std::cout << "[ArkaimLogic] Balance request SUCCESS" << std::endl;
            std::cout << "[ArkaimLogic] Receipt:\n" << receipt << std::endl;

            sendGetBalanceResponse(receipt, true);
        } else if (code == itp::CL2_PAY_ERR_INVALID_PIN) {
            std::cerr << "[ArkaimLogic] Transaction FAILED, Incorrect PIN" << (int) code
                    << " msg=\"" << message << "\"" << std::endl;
            waiting_pin_for_balance = true;
            sendGetBalanceResponse(INCORECT_PINCODE, false);
        }
        else {
            std::cerr << "[ArkaimLogic] Balance request FAILED, code=" << (int) code
                    << " msg=\"" << message << "\"" << std::endl;
        }

    }

    void ArkaimLogic::resetCardState() {
        card_resolved = false;
        card_number.clear();
        card_data.clear();
        pin_data.clear();
        pending_payment.active = false;
        std::cout << "[ArkaimLogic] Card state reset due to user UI action" << std::endl;
    }

    void ArkaimLogic::handleCardError(const uint16_t error) {
        Message error_card_message;
        error_card_message.source = PIPE_LAYER;
        error_card_message.status_code = ERROR_STATUS_CODE;

        switch (error) {
            case ERROR_SERVER_PROCESSING: {
                error_card_message.type = ERROR_SERVER_PROCESSING_TYPE;
            }

        }
        if (m_core) {
            m_core->sendToLogicLayer(PRIME_HTTP_LAYER, error_card_message);
        }
    }
} // namespace bridge

#undef ABIND
