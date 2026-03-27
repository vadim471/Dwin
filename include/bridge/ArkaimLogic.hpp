//
// Created by vadim.tissen on 27.03.2026.
//

#pragma once

#include "bridge/ILogicHandler.hpp"
#include "bridge/types.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <atomic>
#include <string>

#include <itp/frame.hpp>
#include "Settings.hpp"

namespace bridge {

    class MessageLayer;

    class ArkaimLogic : public ILogicHandler, public std::enable_shared_from_this<ArkaimLogic> {
    public:
        ArkaimLogic(const Settings &settings, boost::asio::io_service &ios);
        ~ArkaimLogic() override = default;

        void handle(const Message &message, MessageLayer &core) override;
        void startLoop(MessageLayer &core);

    private:
        // --- Сетевые запросы к Arkaim (аналог HTTP запросов) ---
        void checkCard(MessageLayer &core);
        void closeShift(MessageLayer &core);
        void getInfo(MessageLayer &core);
        void transaction(MessageLayer &core);
        void confirm(MessageLayer &core);
        void cancelPayment(MessageLayer &core);

        // --- Обработчики ответов от Arkaim ---
        void processArkaimResponse(const Message &message, MessageLayer &core);
        void onCheckCardResponse(itp::frame& response, MessageLayer &core);
        void onTransactionResponse(itp::frame& response, MessageLayer &core);
        void onConfirmResponse(itp::frame& response, MessageLayer &core);
        void onCancelResponse(itp::frame& response, MessageLayer &core);

        // --- Таймеры и поллинг (если требуется пинговать терминал) ---
        void scheduleNextTick(MessageLayer &core);
        void onTimerExpired(const boost::system::error_code &ec, MessageLayer &core);

        // --- Переменные состояния ---
        Settings m_settings;
        boost::asio::deadline_timer m_timer;
        std::atomic<bool> m_waitingResponse;

        uint64_t m_last_transaction_id = 0;
        uint8_t m_reader_address = 3;

        // Данные текущего заказа
        std::string m_current_card_number;
        std::string m_current_fuel_id;
        uint32_t m_current_price_value = 0;
        uint8_t m_current_price_decimal = 2;
        uint32_t m_current_volume_value = 0;
        uint8_t m_current_volume_decimal = 2;
        uint32_t m_current_amount_value = 0;
        uint8_t m_current_amount_decimal = 2;
    };

}
