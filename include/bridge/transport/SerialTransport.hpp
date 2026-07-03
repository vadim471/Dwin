#pragma once
#include "bridge/transport/ITransport.hpp"
#include <boost/asio.hpp>
#include <mutex>
#include <queue>
#include <atomic>

#include "bridge/core/Settings.hpp"
#include "bridge/core/types.hpp"

namespace bridge {

    class SerialTransport : public ITransport {
    public:
        SerialTransport(boost::asio::io_service& ios, const Settings &settings);
        ~SerialTransport() override;

        void start() override;
        void stop() override;
        void send(const RawData& raw_data) override;
        void setReceiveHandler(ReceiveHandler handler) override;

    private:
        void doRead();
        void handleRead(const boost::system::error_code& ec, std::size_t bytes_transferred);

        void doWrite();
        void doWriteImpl(); // Вспомогательный метод для отправки
        void handleWrite(const boost::system::error_code& ec, std::size_t bytes_transferred);

        void closePort();

        boost::asio::io_service& ios;
        boost::asio::serial_port port;
        unsigned int baud_rate;
        std::string port_name;
        Settings m_settings;

        ReceiveHandler receive_handler;
        std::atomic<bool> running{false};

        RawData read_buffer { Bytes(1024) };

        std::mutex write_serial_mutex;
        std::queue<RawData> write_serial_queue;
        bool m_write_in_progress = false;

        // --- Умный дроссель очереди ---
        boost::asio::deadline_timer m_throttle_timer;
        size_t m_burst_bytes = 0;

        const size_t MAX_BURST_BYTES = 64; 
    };
}