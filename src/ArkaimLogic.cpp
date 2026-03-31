//
// Created by vadim.tissen on 27.03.2026.
//

#include "bridge/ArkaimLogic.hpp"

#include "bridge/MessageLayer.hpp"

#include <itp/cl2.h>
#include <itp/named_pipe.hpp>

#include <initp/system/time.hpp>

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

extern "C" {
#include <itp/error.h>
#include <initp/system/time.h>
}

namespace bridge {

class ArkaimLogic::TerminalEntity : public itp::entity {
public:
    TerminalEntity()
        : itp::entity(itp::CL2_API_ACTIVE_TERMINAL, itp::CL2_DEV_GENERIC) {
    }

    uint32_t status() const override {
        return itp::CL2_DST_ONLINE;
    }
};

namespace {

constexpr uint32_t kClientPollIntervalMs = 5;
constexpr uint32_t kReconnectIntervalMs = 3000; // Увеличиваем интервал переподключения до 3 секунд
constexpr uint8_t kDefaultReaderAddress = 3;

const std::string& defaultCardNumber() {
    static const std::string value = "561234567";
    return value;
}

const std::vector<uint8_t>& defaultCardData() {
    static const std::vector<uint8_t> value = {5, 6, 1, 2, 3, 4, 5, 6, 7};
    return value;
}

} // namespace

ArkaimLogic::ArkaimLogic(const Settings& settings, boost::asio::io_service& ios)
    : m_settings(settings),
      m_ios(ios),
      m_timer(ios),
      m_pipe_poll_timer(ios),
      m_terminal(std::make_unique<TerminalEntity>()),
      m_waitingResponse(false),
      m_connected(false),
      m_initialized(false),
      m_connecting(false),
      m_has_pending_payment(false),
      m_connection_attempts(0),
      m_last_transaction_id(0),
      m_payment_address(0),
      m_reader_address(kDefaultReaderAddress),
      m_reader_api_type(itp::CL2_API_CONTACTLESS_READER | itp::CL2_API_BARCODE_SCANNER) {
}

ArkaimLogic::~ArkaimLogic() {
    boost::system::error_code error;
    m_timer.cancel(error);
    m_pipe_poll_timer.cancel(error);
}

void ArkaimLogic::startLoop(MessageLayer& core) {
    auto self = shared_from_this();
    m_ios.post([self]() {
        self->ensurePipeSession();
        self->schedulePipePoll();
    });
    scheduleNextTick(core);
}

void ArkaimLogic::handle(const Message& message, MessageLayer& core) {
    (void)core;

    if (message.type == PAY_TRANSACTION) {
        PaymentRequestData payment;
        if (!deserializePaymentRequest(message.payload, payment)) {
            std::cerr << "[ArkaimLogic] Failed to parse payment payload" << std::endl;
            return;
        }

        {
            std::lock_guard<std::mutex> lock(m_payment_mutex);
            m_current_payment = payment;
            m_has_pending_payment = true;
        }

        auto self = shared_from_this();
        m_ios.post([self]() {
            self->submitPendingTransaction();
        });
        return;
    }

    auto self = shared_from_this();
    if (message.type == "CHECK_CARD") {
        m_ios.post([self]() {
            self->requestCheckCard();
        });
    } else if (message.type == "CONFIRM_TRANSACTION") {
        m_ios.post([self]() {
            self->requestConfirm();
        });
    } else if (message.type == "CANCEL_TRANSACTION") {
        m_ios.post([self]() {
            self->requestCancel();
        });
    } else if (message.type == "CLOSE_SHIFT") {
        m_ios.post([self]() {
            self->requestCloseShift();
        });
    }
}

void ArkaimLogic::ensurePipeSession() {
    if (m_connected.load() || m_connecting) {
        return;
    }

    if (m_settings.pipe.name.empty()) {
        std::cerr << "[ArkaimLogic] Pipe name is empty" << std::endl;
        return;
    }

    m_connection_attempts++;
    m_connecting = true;
    m_connected.store(false);
    m_initialized.store(false);
    m_payment_address = 0;
    m_reader_address = kDefaultReaderAddress;
    m_reader_api_type = itp::CL2_API_CONTACTLESS_READER | itp::CL2_API_BARCODE_SCANNER;
    m_reader_unique_id.clear();

    std::cout << "[ArkaimLogic] Connection attempt #" << m_connection_attempts 
              << " to pipe: '" << m_settings.pipe.name << "'" << std::endl;

    // Match terminal: create a fresh entity with handlers already registered,
    // instead of reinitializing the root and losing entity-level handlers.
    m_terminal = std::make_unique<TerminalEntity>();

    itp::named_pipe::uptr pipe = std::make_unique<itp::named_pipe>(m_settings.pipe.name);
    if (!pipe) {
        m_connecting = false;
        std::cerr << "[ArkaimLogic] Failed to create named_pipe object" << std::endl;
        return;
    }

    std::cout << "[ArkaimLogic] Calling pipe->open()..." << std::endl;
    if (!pipe->open()) {
        m_connecting = false;
        std::cerr << "[ArkaimLogic] Failed to open transport pipe '" << m_settings.pipe.name << "'" << std::endl;
        std::cerr << "[ArkaimLogic] Make sure Arkaim processing is running on the VM" << std::endl;
        std::cerr << "[ArkaimLogic] Expected pipe path: \\\\.\\pipe\\" << m_settings.pipe.name << std::endl;
        std::cerr << "[ArkaimLogic] Will retry in " << (kReconnectIntervalMs / 1000) << " seconds..." << std::endl;
        return;
    }

    std::cout << "[ArkaimLogic] Pipe opened successfully" << std::endl;

    const auto set_parent_result = m_terminal->node().set_parent(std::move(pipe), nullptr);
    if (set_parent_result != ITP_ERRC_NONE) {
        m_connecting = false;
        std::cerr << "[ArkaimLogic] Failed to attach transport pipe: " << set_parent_result
                  << " (" << itp_get_error_description(set_parent_result) << ")" << std::endl;
        return;
    }

    const auto connect_result = m_terminal->node().request_connect(
        initp::system::time::now(),
        [this](itp::root&, uint16_t error) {
            onConnect(error);
        }
    );

    if (connect_result != ITP_ERRC_NONE) {
        m_connecting = false;
        std::cerr << "[ArkaimLogic] Failed to make connection request: " << connect_result
                  << " (" << itp_get_error_description(connect_result) << ")" << std::endl;
    }
}

void ArkaimLogic::schedulePipePoll() {
    m_pipe_poll_timer.expires_from_now(boost::posix_time::milliseconds(kClientPollIntervalMs));
    auto self = shared_from_this();
    m_pipe_poll_timer.async_wait([self](const boost::system::error_code& error) {
        if (error) {
            return;
        }

        if (self->m_terminal != nullptr) {
            self->m_terminal->poll(sys_clock_ms());
        }

        self->schedulePipePoll();
    });
}

void ArkaimLogic::submitPendingTransaction() {
    PaymentRequestData payment;
    {
        std::lock_guard<std::mutex> lock(m_payment_mutex);
        if (!m_has_pending_payment || m_waitingResponse.load()) {
            return;
        }
        payment = m_current_payment;
        m_has_pending_payment = false;
    }

    if (!m_initialized.load()) {
        std::cout << "[ArkaimLogic] Payment request queued until pipe initialization completes" << std::endl;
        std::lock_guard<std::mutex> lock(m_payment_mutex);
        m_has_pending_payment = true;
        return;
    }

    m_waitingResponse.store(true);
    if (!requestTransaction(payment)) {
        m_waitingResponse.store(false);
        std::lock_guard<std::mutex> lock(m_payment_mutex);
        m_current_payment = payment;
        m_has_pending_payment = true;
    }
}

void ArkaimLogic::scheduleNextTick(MessageLayer& core) {
    m_timer.expires_from_now(boost::posix_time::milliseconds(kReconnectIntervalMs));
    auto self = shared_from_this();
    m_timer.async_wait([self, &core](const boost::system::error_code& ec) {
        if (!ec) {
            self->onTimerExpired(ec, core);
        }
    });
}

void ArkaimLogic::onTimerExpired(const boost::system::error_code& ec, MessageLayer& core) {
    (void)core;
    if (ec) {
        return;
    }

    auto self = shared_from_this();
    m_ios.post([self]() {
        if (!self->m_connected.load()) {
            self->ensurePipeSession();
            return;
        }

        if (!self->m_initialized.load()) {
            self->requestApiList();
            return;
        }

        self->submitPendingTransaction();
    });

    scheduleNextTick(core);
}

void ArkaimLogic::requestApiList() {
    if (!m_connected.load() || m_terminal == nullptr) {
        return;
    }

    auto request = std::make_unique<itp::frame>(itp::CL2_CMD_GET_API_LIST);
    const auto result = m_terminal->node().push_request(
        std::move(request),
        m_terminal->node().root_address(),
        [this](itp::root&, uint16_t error, itp::frame& response) {
            onGetApiList(error, response);
        }
    );

    if (result != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] Failed to request API list: " << result
                  << " (" << itp_get_error_description(result) << ")" << std::endl;
    }
}

void ArkaimLogic::requestCheckCard() {
    if (!m_initialized.load() || m_payment_address == 0 || m_terminal == nullptr) {
        std::cerr << "[ArkaimLogic] Payment pipe is not initialized yet" << std::endl;
        return;
    }

    auto request = std::make_unique<itp::frame>(itp::CL2_CMD_PAY_CHECK_CARD);
    itp_error_code_t errc = request->write_value(m_reader_address);
    if (!errc) {
        errc = request->write_value(m_reader_api_type);
    }

    if (errc != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] Failed to build CHECK_CARD request: " << errc
                  << " (" << itp_get_error_description(errc) << ")" << std::endl;
        return;
    }

    m_waitingResponse.store(true);
    const auto result = m_terminal->node().push_request(
        std::move(request),
        m_payment_address,
        [this](itp::root&, uint16_t error, itp::frame& response) {
            onCheckCard(error, response);
        }
    );

    if (result != ITP_ERRC_NONE) {
        m_waitingResponse.store(false);
        std::cerr << "[ArkaimLogic] Failed to push CHECK_CARD request: " << result
                  << " (" << itp_get_error_description(result) << ")" << std::endl;
    }
}

bool ArkaimLogic::requestTransaction(const PaymentRequestData& payment) {
    if (!m_initialized.load() || m_payment_address == 0 || m_terminal == nullptr) {
        std::cerr << "[ArkaimLogic] Payment device was not discovered yet" << std::endl;
        return false;
    }

    auto request = std::make_unique<itp::frame>(itp::CL2_CMD_PAY_TRANSACTION);
    const auto& card_data = defaultCardData();
    const std::vector<uint8_t> pin_data;
    const uint64_t time = initp::system::time::now() / 1000;
    const uint32_t reader_api_type = itp::CL2_API_PAYMENT;

    itp_error_code_t errc = request->write_value(m_reader_address);
    if (!errc) errc = request->write_value(reader_api_type);
    if (!errc) errc = request->write_string(defaultCardNumber());
    if (!errc) errc = request->write_array(card_data);
    if (!errc) errc = request->write_array(pin_data);
    if (!errc) errc = request->write_value(time);
    if (!errc) errc = request->write_string(payment.product_id.empty() ? std::string("dt") : payment.product_id);
    if (!errc) errc = request->write_value(payment.price_value == 0 ? uint32_t(190) : payment.price_value);
    if (!errc) errc = request->write_value(payment.price_decimal);
    if (!errc) errc = request->write_value(payment.volume_value == 0 ? uint32_t(200) : payment.volume_value);
    if (!errc) errc = request->write_value(payment.volume_decimal);
    if (!errc) errc = request->write_value(payment.amount_value == 0 ? uint32_t(380) : payment.amount_value);
    if (!errc) errc = request->write_value(payment.amount_decimal);

    if (errc != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] Failed to build PAY_TRANSACTION request: " << errc
                  << " (" << itp_get_error_description(errc) << ")" << std::endl;
        return false;
    }

    const auto result = m_terminal->node().push_request(
        std::move(request),
        m_payment_address,
        [this](itp::root&, uint16_t error, itp::frame& response) {
            onTransaction(error, response);
        }
    );

    if (result != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] Failed to push PAY_TRANSACTION request: " << result
                  << " (" << itp_get_error_description(result) << ")" << std::endl;
        return false;
    }

    return true;
}

void ArkaimLogic::requestConfirm() {
    if (!m_initialized.load() || m_payment_address == 0 || m_terminal == nullptr) {
        std::cerr << "[ArkaimLogic] Payment pipe is not initialized yet" << std::endl;
        return;
    }

    const auto transaction_id = m_last_transaction_id.load();
    if (transaction_id == 0) {
        std::cerr << "[ArkaimLogic] There is no transaction to confirm" << std::endl;
        return;
    }

    PaymentRequestData payment;
    {
        std::lock_guard<std::mutex> lock(m_payment_mutex);
        payment = m_current_payment;
    }

    auto request = std::make_unique<itp::frame>(itp::CL2_CMD_PAY_CONFIRM);
    const auto& card_data = defaultCardData();
    const uint64_t time = initp::system::time::now();
    const uint32_t reader_api_type = itp::CL2_API_PAYMENT;
    const uint8_t not_complete = 0;

    itp_error_code_t errc = request->write_value(m_reader_address);
    if (!errc) errc = request->write_value(reader_api_type);
    if (!errc) errc = request->write_string(defaultCardNumber());
    if (!errc) errc = request->write_array(card_data);
    if (!errc) errc = request->write_value(time);
    if (!errc) errc = request->write_string(payment.product_id.empty() ? std::string("dt") : payment.product_id);
    if (!errc) errc = request->write_value(transaction_id);
    if (!errc) errc = request->write_value(payment.price_value);
    if (!errc) errc = request->write_value(payment.price_decimal);
    if (!errc) errc = request->write_value(payment.volume_value);
    if (!errc) errc = request->write_value(payment.volume_decimal);
    if (!errc) errc = request->write_value(payment.amount_value);
    if (!errc) errc = request->write_value(payment.amount_decimal);
    if (!errc) errc = request->write_value(not_complete);
    if (!errc) errc = request->write_value(payment.volume_value);
    if (!errc) errc = request->write_value(payment.volume_decimal);
    if (!errc) errc = request->write_value(payment.amount_value);
    if (!errc) errc = request->write_value(payment.amount_decimal);

    if (errc != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] Failed to build PAY_CONFIRM request: " << errc
                  << " (" << itp_get_error_description(errc) << ")" << std::endl;
        return;
    }

    m_waitingResponse.store(true);
    const auto result = m_terminal->node().push_request(
        std::move(request),
        m_payment_address,
        [this](itp::root&, uint16_t error, itp::frame& response) {
            onConfirm(error, response);
        }
    );

    if (result != ITP_ERRC_NONE) {
        m_waitingResponse.store(false);
        std::cerr << "[ArkaimLogic] Failed to push PAY_CONFIRM request: " << result
                  << " (" << itp_get_error_description(result) << ")" << std::endl;
    }
}

void ArkaimLogic::requestCancel() {
    if (!m_initialized.load() || m_payment_address == 0 || m_terminal == nullptr) {
        std::cerr << "[ArkaimLogic] Payment pipe is not initialized yet" << std::endl;
        return;
    }

    const auto transaction_id = m_last_transaction_id.load();
    if (transaction_id == 0) {
        std::cerr << "[ArkaimLogic] There is no transaction to cancel" << std::endl;
        return;
    }

    PaymentRequestData payment;
    {
        std::lock_guard<std::mutex> lock(m_payment_mutex);
        payment = m_current_payment;
    }

    auto request = std::make_unique<itp::frame>(itp::CL2_CMD_PAY_CANCEL);
    const auto& card_data = defaultCardData();
    const uint64_t time = initp::system::time::now();
    const uint32_t reader_api_type = itp::CL2_API_PAYMENT;

    itp_error_code_t errc = request->write_value(m_reader_address);
    if (!errc) errc = request->write_value(reader_api_type);
    if (!errc) errc = request->write_string(defaultCardNumber());
    if (!errc) errc = request->write_array(card_data);
    if (!errc) errc = request->write_value(time);
    if (!errc) errc = request->write_string(payment.product_id.empty() ? std::string("dt") : payment.product_id);
    if (!errc) errc = request->write_value(transaction_id);
    if (!errc) errc = request->write_value(payment.price_value);
    if (!errc) errc = request->write_value(payment.price_decimal);
    if (!errc) errc = request->write_value(payment.volume_value);
    if (!errc) errc = request->write_value(payment.volume_decimal);
    if (!errc) errc = request->write_value(payment.amount_value);
    if (!errc) errc = request->write_value(payment.amount_decimal);

    if (errc != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] Failed to build PAY_CANCEL request: " << errc
                  << " (" << itp_get_error_description(errc) << ")" << std::endl;
        return;
    }

    m_waitingResponse.store(true);
    const auto result = m_terminal->node().push_request(
        std::move(request),
        m_payment_address,
        [this](itp::root&, uint16_t error, itp::frame& response) {
            onCancel(error, response);
        }
    );

    if (result != ITP_ERRC_NONE) {
        m_waitingResponse.store(false);
        std::cerr << "[ArkaimLogic] Failed to push PAY_CANCEL request: " << result
                  << " (" << itp_get_error_description(result) << ")" << std::endl;
    }
}

void ArkaimLogic::requestCloseShift() {
    if (!m_initialized.load() || m_payment_address == 0 || m_terminal == nullptr) {
        std::cerr << "[ArkaimLogic] Payment pipe is not initialized yet" << std::endl;
        return;
    }

    auto request = std::make_unique<itp::frame>(itp::CL2_CMD_PAY_CLOSE_SHIFT);
    m_waitingResponse.store(true);
    const auto result = m_terminal->node().push_request(
        std::move(request),
        m_payment_address,
        [this](itp::root&, uint16_t error, itp::frame& response) {
            onCloseShift(error, response);
        }
    );

    if (result != ITP_ERRC_NONE) {
        m_waitingResponse.store(false);
        std::cerr << "[ArkaimLogic] Failed to push PAY_CLOSE_SHIFT request: " << result
                  << " (" << itp_get_error_description(result) << ")" << std::endl;
    }
}

void ArkaimLogic::onConnect(uint16_t error) {
    m_connecting = false;
    if (error != ITP_ERRC_NONE) {
        m_connected.store(false);
        m_initialized.store(false);
        std::cerr << "[ArkaimLogic] request_connect failed with error: " << error
                  << " (" << itp_get_error_description(error) << ")" << std::endl;
        std::cerr << "[ArkaimLogic] Will retry connection..." << std::endl;
        return;
    }

    m_connected.store(true);
    m_connection_attempts = 0; // Сбрасываем счетчик при успешном подключении
    std::cout << "[ArkaimLogic] Connected to Arkaim pipe '" << m_settings.pipe.name << "'" << std::endl;
    requestApiList();
}

void ArkaimLogic::onGetApiList(uint16_t error, itp::frame& response) {
    if (error != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] GET_API_LIST failed with error: " << error
                  << " (" << itp_get_error_description(error) << ")" << std::endl;
        return;
    }

    m_payment_address = 0;
    m_reader_address = kDefaultReaderAddress;
    m_reader_api_type = itp::CL2_API_CONTACTLESS_READER | itp::CL2_API_BARCODE_SCANNER;
    m_reader_unique_id.clear();

    while (response.has_more()) {
        itp_error_code_t errc = ITP_ERRC_NONE;
        uint8_t address = 0;
        uint32_t api = 0;
        uint8_t type = 0;
        std::string id;

        std::tie(address, errc) = response.read_value<uint8_t>();
        if (!errc) std::tie(api, errc) = response.read_value<uint32_t>();
        if (!errc) std::tie(type, errc) = response.read_value<uint8_t>();
        if (!errc) std::tie(id, errc) = response.read_string();
        if (errc != ITP_ERRC_NONE) {
            std::cerr << "[ArkaimLogic] Failed to parse API list entry: " << errc
                      << " (" << itp_get_error_description(errc) << ")" << std::endl;
            break;
        }

        std::cout << "[ArkaimLogic] Device: address=" << static_cast<int>(address)
                  << ", api=0x" << std::hex << api << std::dec
                  << ", type=" << static_cast<int>(type)
                  << ", id=" << id << std::endl;

        if ((api & itp::CL2_API_PAYMENT) && m_payment_address == 0) {
            m_payment_address = address;
        }

        if ((api & (itp::CL2_API_CONTACTLESS_READER | itp::CL2_API_BARCODE_SCANNER)) && m_reader_unique_id.empty()) {
            m_reader_address = address;
            m_reader_api_type = api & (itp::CL2_API_CONTACTLESS_READER | itp::CL2_API_BARCODE_SCANNER);
            m_reader_unique_id = id;
        }
    }

    if (m_payment_address == 0) {
        m_initialized.store(false);
        std::cerr << "[ArkaimLogic] Payment device was not found in API list" << std::endl;
        return;
    }

    m_initialized.store(true);
    std::cout << "[ArkaimLogic] Payment device address=" << static_cast<int>(m_payment_address)
              << ", reader address=" << static_cast<int>(m_reader_address) << std::endl;

    submitPendingTransaction();
}

void ArkaimLogic::onCheckCard(uint16_t error, itp::frame& response) {
    m_waitingResponse.store(false);
    if (error != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] CHECK_CARD failed with error: " << error
                  << " (" << itp_get_error_description(error) << ")" << std::endl;
        return;
    }

    itp_error_code_t errc = ITP_ERRC_NONE;
    uint16_t card_type = 0;
    std::string card_name;

    std::tie(card_type, errc) = response.read_value<uint16_t>();
    if (!errc) std::tie(card_name, errc) = response.read_string();

    if (errc != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] Failed to read CHECK_CARD response: " << errc
                  << " (" << itp_get_error_description(errc) << ")" << std::endl;
        return;
    }

    std::cout << "[ArkaimLogic] Card detected: type=" << card_type << ", name=" << card_name << std::endl;
}

void ArkaimLogic::onTransaction(uint16_t error, itp::frame& response) {
    m_waitingResponse.store(false);
    if (error != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] PAY_TRANSACTION failed with error: " << error
                  << " (" << itp_get_error_description(error) << ")" << std::endl;
        return;
    }

    itp_error_code_t errc = ITP_ERRC_NONE;
    uint8_t op_err = 0;
    std::string receipt_text;
    std::string client_message;
    uint32_t service_err = 0;
    std::string service_err_desc;

    std::tie(op_err, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(receipt_text, errc) = response.read_wide_string();
    if (!errc) std::tie(client_message, errc) = response.read_string();
    if (!errc) std::tie(service_err, errc) = response.read_value<uint32_t>();
    if (!errc) std::tie(service_err_desc, errc) = response.read_string();

    if (errc != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] Failed to read PAY_TRANSACTION response: " << errc
                  << " (" << itp_get_error_description(errc) << ")" << std::endl;
        return;
    }

    if (op_err != 0) {
        std::cerr << "[ArkaimLogic] Transaction rejected: op_err=" << static_cast<int>(op_err)
                  << ", service_err=" << service_err
                  << ", service_desc=" << service_err_desc << std::endl;
        return;
    }

    uint64_t transaction_id = 0;
    uint32_t price_value = 0;
    uint8_t price_decimal = 0;
    uint32_t volume_value = 0;
    uint8_t volume_decimal = 0;
    uint32_t amount_value = 0;
    uint8_t amount_decimal = 0;

    std::tie(transaction_id, errc) = response.read_value<uint64_t>();
    if (!errc) std::tie(price_value, errc) = response.read_value<uint32_t>();
    if (!errc) std::tie(price_decimal, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(volume_value, errc) = response.read_value<uint32_t>();
    if (!errc) std::tie(volume_decimal, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(amount_value, errc) = response.read_value<uint32_t>();
    if (!errc) std::tie(amount_decimal, errc) = response.read_value<uint8_t>();

    if (errc != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] Failed to read transaction payload: " << errc
                  << " (" << itp_get_error_description(errc) << ")" << std::endl;
        return;
    }

    m_last_transaction_id.store(transaction_id);
    std::cout << "[ArkaimLogic] Transaction success: id=" << transaction_id
              << ", amount=" << amount_value << "." << static_cast<int>(amount_decimal)
              << ", client_message='" << client_message << "'" << std::endl;
}

void ArkaimLogic::onConfirm(uint16_t error, itp::frame& response) {
    m_waitingResponse.store(false);
    if (error != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] PAY_CONFIRM failed with error: " << error
                  << " (" << itp_get_error_description(error) << ")" << std::endl;
        return;
    }

    itp_error_code_t errc = ITP_ERRC_NONE;
    uint8_t op_err = 0;
    std::string receipt_text;
    std::string client_message;
    uint32_t service_err = 0;
    std::string service_err_desc;

    std::tie(op_err, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(receipt_text, errc) = response.read_wide_string();
    if (!errc) std::tie(client_message, errc) = response.read_string();
    if (!errc) std::tie(service_err, errc) = response.read_value<uint32_t>();
    if (!errc) std::tie(service_err_desc, errc) = response.read_string();

    if (errc != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] Failed to read PAY_CONFIRM response: " << errc
                  << " (" << itp_get_error_description(errc) << ")" << std::endl;
        return;
    }

    if (op_err != 0) {
        std::cerr << "[ArkaimLogic] Confirm rejected: op_err=" << static_cast<int>(op_err)
                  << ", service_err=" << service_err
                  << ", service_desc=" << service_err_desc << std::endl;
        return;
    }

    std::cout << "[ArkaimLogic] Confirm success. Receipt: " << receipt_text << std::endl;
}

void ArkaimLogic::onCancel(uint16_t error, itp::frame& response) {
    m_waitingResponse.store(false);
    if (error != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] PAY_CANCEL failed with error: " << error
                  << " (" << itp_get_error_description(error) << ")" << std::endl;
        return;
    }

    itp_error_code_t errc = ITP_ERRC_NONE;
    uint8_t op_err = 0;
    std::string receipt_text;
    std::string client_message;
    uint32_t service_err = 0;
    std::string service_err_desc;

    std::tie(op_err, errc) = response.read_value<uint8_t>();
    if (!errc) std::tie(receipt_text, errc) = response.read_wide_string();
    if (!errc) std::tie(client_message, errc) = response.read_string();
    if (!errc) std::tie(service_err, errc) = response.read_value<uint32_t>();
    if (!errc) std::tie(service_err_desc, errc) = response.read_string();

    if (errc != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] Failed to read PAY_CANCEL response: " << errc
                  << " (" << itp_get_error_description(errc) << ")" << std::endl;
        return;
    }

    if (op_err != 0) {
        std::cerr << "[ArkaimLogic] Cancel rejected: op_err=" << static_cast<int>(op_err)
                  << ", service_err=" << service_err
                  << ", service_desc=" << service_err_desc << std::endl;
        return;
    }

    std::cout << "[ArkaimLogic] Cancel success. Receipt: " << receipt_text << std::endl;
}

void ArkaimLogic::onCloseShift(uint16_t error, itp::frame& response) {
    (void)response;
    m_waitingResponse.store(false);
    if (error != ITP_ERRC_NONE) {
        std::cerr << "[ArkaimLogic] PAY_CLOSE_SHIFT failed with error: " << error
                  << " (" << itp_get_error_description(error) << ")" << std::endl;
        return;
    }

    std::cout << "[ArkaimLogic] Close shift completed" << std::endl;
}

} // namespace bridge
