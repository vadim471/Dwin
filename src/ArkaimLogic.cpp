//
// Created by vadim.tissen on 27.03.2026.
//

//
// Created by vadim.tissen on [Current Date].
//

#include "bridge/ArkaimLogic.hpp"
#include "bridge/MessageLayer.hpp"
#include "bridge/utility.hpp"
#include "bridge/constant.hpp"
#include "bridge/DwinCommands.hpp"
#include <iostream>
#include <boost/make_unique.hpp>
#include <initp/system/time.hpp>

namespace bridge {

    ArkaimLogic::ArkaimLogic(const Settings &settings, boost::asio::io_service &ios)
        : m_settings(settings),
          m_timer(ios),
          m_waitingResponse(false) {
    }

    void ArkaimLogic::startLoop(MessageLayer &core) {
        // Запуск поллинга или инициализации терминала
        scheduleNextTick(core);
    }


    void ArkaimLogic::scheduleNextTick(MessageLayer &core) {
        m_timer.expires_from_now(boost::posix_time::milliseconds(1000));
        auto self = shared_from_this();

        m_timer.async_wait([self, &core](const boost::system::error_code &ec) {
            if (!ec) {
                self->onTimerExpired(ec, core);
            }
        });
    }

    void ArkaimLogic::onTimerExpired(const boost::system::error_code &ec, MessageLayer &core) {
        if (ec) return;

        if (m_waitingResponse.load()) {
            // Watchdog логика: если зависли, можно сбросить стейт терминала
        }

        // Перезавод таймера
        scheduleNextTick(core);
    }

    void ArkaimLogic::handle(const Message &message, MessageLayer &core) {
        // Сброс флага ожидания при любом ответе от железа
        if (message.source == "ARKAIM_LAYER") {
            m_waitingResponse.store(false);
            processArkaimResponse(message, core);
        }
        // Ловим UI ивенты от DWIN
        else if (message.type == USER_TOUCH_BASIC_TOUCH_BEGIN_FUELLING_BUTTON) {
            // Нажата кнопка оплаты — запускаем транзакцию
            m_waitingResponse.store(true);
            transaction(core);
        }
        else if (message.type == USER_TOUCH_CLOSE_ORDER_BUTTON) {
            m_waitingResponse.store(true);
            confirm(core); // Подтверждаем транзакцию после налива
        }
    }

    void ArkaimLogic::processArkaimResponse(const Message &message, MessageLayer &core) {
        // Восстанавливаем itp::frame из сырых байтов, которые прислал ArkaimParser
        itp::frame чresponse;
        response.write_array(message.payload);
        response.reset_read(); // Сброс указателя чтения в начало фрейма

        uint16_t command = message.resource_id.empty() ? 0 : std::stoi(message.resource_id);

        switch(command) {
            case itp::CL2_CMD_PAY_CHECK_CARD:
                onCheckCardResponse(response, core);
                break;
            case itp::CL2_CMD_PAY_TRANSACTION:
                onTransactionResponse(response, core);
                break;
            case itp::CL2_CMD_PAY_CONFIRM:
                onConfirmResponse(response, core);
                break;
            case itp::CL2_CMD_PAY_CANCEL:
                onCancelResponse(response, core);
                break;
            default:
                std::cerr << "[ArkaimLogic] Unknown command response: " << command << std::endl;
                break;
        }
    }

    void ArkaimLogic::transaction(MessageLayer &core) {
        itp_error_code_t errc;
        uint32_t reader_api_type = itp::CL2_API_PAYMENT;
        std::vector<uint8_t> card_data = {5, 6, 1, 2, 3, 4, 5, 6, 7}; // Заглушка, берется из стейта
        std::vector<uint8_t> pin_data = {};
        uint64_t time = initp::system::time::now() / 1000;

        // Пример использования itp::frame
        itp::frame::uptr request = boost::make_unique<itp::frame>(itp::CL2_CMD_PAY_TRANSACTION);

        errc = request->write_value(m_reader_address);
        if (!errc) request->write_value(reader_api_type);
        if (!errc) request->write_string(m_current_card_number.empty() ? "561234567" : m_current_card_number);
        if (!errc) request->write_array(card_data);
        if (!errc) request->write_array(pin_data);
        if (!errc) request->write_value(time);
        if (!errc) request->write_string(m_current_fuel_id.empty() ? "dt" : m_current_fuel_id);
        if (!errc) request->write_value(m_current_price_value == 0 ? 190 : m_current_price_value);
        if (!errc) request->write_value(m_current_price_decimal);
        if (!errc) request->write_value(m_current_volume_value == 0 ? 200 : m_current_volume_value);
        if (!errc) request->write_value(m_current_volume_decimal);
        if (!errc) request->write_value(m_current_amount_value == 0 ? 380 : m_current_amount_value);
        if (!errc) request->write_value(m_current_amount_decimal);

        if (errc) {
            std::cerr << "[ArkaimLogic] transaction> Failed to construct request" << std::endl;
            return;
        }

        // Перекладываем сырые данные фрейма в наш Message
        Message msg;
        msg.type = "ARKAIM_REQUEST";
        msg.resource_id = std::to_string(itp::CL2_CMD_PAY_TRANSACTION); // Сохраняем команду, чтобы парсер знал длину

        std::vector<uint8_t> frame_data; // Достаем байты из request (в зависимости от API itp)
        // Предполагаемый метод выгрузки байтов из itp::frame:
        // frame_data = request->get_buffer();
        msg.payload = frame_data;

        core.sendTo("ARKAIM_LAYER", msg);
    }

    void ArkaimLogic::onTransactionResponse(itp::frame& response, MessageLayer &core) {
        itp_error_code_t errc;
        uint8_t op_err;
        std::string receipt_text, client_message, service_err_desc;
        uint32_t service_err;

        std::tie(op_err, errc) = response.read_value<uint8_t>();
        if (!errc) std::tie(receipt_text, errc) = response.read_wide_string();
        if (!errc) std::tie(client_message, errc) = response.read_string();
        if (!errc) std::tie(service_err, errc) = response.read_value<uint32_t>();
        if (!errc) std::tie(service_err_desc, errc) = response.read_string();

        if (errc || op_err != 0) {
            std::cerr << "[ArkaimLogic] Transaction Error: " << (int)op_err << std::endl;
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_error_creating_order);
            return;
        }

        uint64_t transaction_id;
        std::tie(transaction_id, errc) = response.read_value<uint64_t>();

        if (!errc) {
            m_last_transaction_id = transaction_id;
            std::cout << "[ArkaimLogic] Transaction success! ID: " << m_last_transaction_id << std::endl;

            // Запускаем колонку через HttpLogic
            Message start_fuel_msg;
            start_fuel_msg.type = USER_TOUCH_BASIC_TOUCH_BEGIN_FUELLING_BUTTON;
            core.sendToLogicLayer(HTTP_LAYER, start_fuel_msg);
        }
    }

    void ArkaimLogic::confirm(MessageLayer &core) {
        // Аналогично формируем CL2_CMD_PAY_CONFIRM
        // ... (логика из void payment::confirm)
    }

    void ArkaimLogic::onConfirmResponse(itp::frame& response, MessageLayer &core) {
        // ... (логика из void payment::on_confirm)
    }

    void ArkaimLogic::checkCard(MessageLayer &core) {}
    void ArkaimLogic::onCheckCardResponse(itp::frame& response, MessageLayer &core) {}
    void ArkaimLogic::closeShift(MessageLayer &core) {}
    void ArkaimLogic::getInfo(MessageLayer &core) {}
    void ArkaimLogic::cancelPayment(MessageLayer &core) {}
    void ArkaimLogic::onCancelResponse(itp::frame& response, MessageLayer &core) {}
}