//
// Created by vadim.tissen on [Текущая Дата].
//

#pragma once

#include "bridge/ITransport.hpp"
#include <boost/asio.hpp>
#include <queue>
#include <mutex>
#include <string>
#include <array>

namespace bridge {

    class PipeTransport : public ITransport {
    public:

        PipeTransport(boost::asio::io_service& ios, const std::string& pipe_path, const std::string& name = "PIPE_LAYER");
        ~PipeTransport() override;

        void start() override;
        void stop() override;
        void send(const RawData& raw_data) override;
        void setReceiveHandler(ReceiveHandler handler) override;

    private:
        void doConnect();
        void doRead();
        void handleRead(const boost::system::error_code& ec, std::size_t bytes_transferred);

        void doWrite();
        void handleWrite(const boost::system::error_code& ec, std::size_t bytes_transferred);

        void closeSocket();

        boost::asio::io_service& m_ios;
        boost::asio::local::stream_protocol::socket m_socket;
        boost::asio::local::stream_protocol::endpoint m_endpoint;

        std::string m_pipe_path;
        std::string m_name;
        bool m_running;

        std::array<uint8_t, 2048> m_read_buffer;

        std::queue<RawData> m_write_queue;
        std::mutex m_write_mutex;
        ReceiveHandler m_receive_handler;
    };

}