//
// Created by vadim.tissen on 18.11.2025.
//

#pragma once
#include "ITransport.hpp"
#include <boost/asio.hpp>
#include <mutex>
#include <queue>

#include "bridge/core/Settings.hpp"
#include "bridge/core/types.hpp"

//SerialTransport.hpp
namespace bridge {

    class SerialTransport : public ITransport {
    public:
        SerialTransport(boost::asio::io_service& ios, const Settings &settings);

        ~SerialTransport() override;

        // ITransport
        void start() override;
        void stop() override;
        void send(const RawData& raw_data) override;
        void setReceiveHandler(ReceiveHandler handler) override;

    private:
        void doRead();
        void handleRead(const boost::system::error_code& ec, std::size_t bytes_transferred);

        void doWrite();
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

        boost::asio::deadline_timer m_write_timer; // Таймер для последовательной отправки команд в ОЗУ процессора DWIN.
    };
}
