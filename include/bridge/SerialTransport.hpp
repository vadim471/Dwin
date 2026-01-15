//
// Created by vadim.tissen on 18.11.2025.
//

#pragma once
#include "ITransport.hpp"
#include <boost/asio.hpp>
#include <mutex>
#include <queue>

//SerialTransport.hpp
namespace bridge {

    class SerialTransport : public ITransport {
    public:
        SerialTransport(boost::asio::io_service& ios,
                        unsigned int baud_rate = 115200,
                        const std::string& port = "COM5",
                        const std::string& name = "UART");

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
        std::string name;
        std::string port_name;

        ReceiveHandler receive_handler;
        std::atomic<bool> running{false};

        //Буффер для сообщений от дисплея
        RawData read_buffer { Bytes(1024) };


        //Очередь для отправки
        std::mutex write_serial_mutex;
        std::queue<RawData> write_serial_queue;
    };
}
