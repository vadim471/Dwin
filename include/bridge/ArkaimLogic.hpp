//
// Created by vadim.tissen on 27.03.2026.
//

#pragma once

#include "bridge/ILogicHandler.hpp"
#include "bridge/PaymentData.hpp"
#include "bridge/types.hpp"
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <itp/entity.hpp>
#include <itp/frame.hpp>

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include "Settings.hpp"

namespace bridge {

    class MessageLayer;

    class ArkaimLogic : public ILogicHandler, public std::enable_shared_from_this<ArkaimLogic> {
    public:
        ArkaimLogic(const Settings &settings, boost::asio::io_service &ios);
        ~ArkaimLogic() override;

        void handle(const Message &message, MessageLayer &core) override;
        void startLoop(MessageLayer &core);

    private:
        class TerminalEntity;

        void ensurePipeSession();
        void schedulePipePoll();
        void submitPendingTransaction();

        void scheduleNextTick(MessageLayer &core);
        void onTimerExpired(const boost::system::error_code &ec, MessageLayer &core);

        void requestApiList();
        void requestCheckCard();
        bool requestTransaction(const PaymentRequestData& payment);
        void requestConfirm();
        void requestCancel();
        void requestCloseShift();

        void onConnect(uint16_t error);
        void onGetApiList(uint16_t error, itp::frame& response);
        void onCheckCard(uint16_t error, itp::frame& response);
        void onTransaction(uint16_t error, itp::frame& response);
        void onConfirm(uint16_t error, itp::frame& response);
        void onCancel(uint16_t error, itp::frame& response);
        void onCloseShift(uint16_t error, itp::frame& response);

        Settings m_settings;
        boost::asio::io_service& m_ios;
        boost::asio::deadline_timer m_timer;
        boost::asio::deadline_timer m_pipe_poll_timer;
        std::unique_ptr<TerminalEntity> m_terminal;

        std::atomic<bool> m_waitingResponse;
        std::atomic<bool> m_connected;
        std::atomic<bool> m_initialized;

        bool m_connecting;
        bool m_has_pending_payment;
        uint32_t m_connection_attempts;

        std::atomic<uint64_t> m_last_transaction_id;
        uint8_t m_payment_address;
        uint8_t m_reader_address;
        uint32_t m_reader_api_type;
        std::string m_reader_unique_id;

        std::mutex m_payment_mutex;
        PaymentRequestData m_current_payment;
    };

}
