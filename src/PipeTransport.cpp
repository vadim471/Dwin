//
// Created by vadim.tissen on [Текущая Дата].
//

#include "bridge/PipeTransport.hpp"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

namespace bridge {

    PipeTransport::PipeTransport(boost::asio::io_service& ios, const std::string& pipe_path, const std::string& name)
        : m_ios(ios),
          m_read_stream(ios),
          m_write_stream(ios),
          m_pipe_path(pipe_path),
          m_name(name),
          m_running(false),
          m_read_fd(-1),
          m_write_fd(-1) {
    }

    PipeTransport::~PipeTransport() {
        stop();
    }

    void PipeTransport::setReceiveHandler(ReceiveHandler handler) {
        m_receive_handler = std::move(handler);
    }

    void PipeTransport::start() {
        if (m_running) return;
        m_running = true;

        doConnect();
    }

    void PipeTransport::stop() {
        if (!m_running) return;
        m_running = false;

        m_ios.post([this]() {
            closeStreams();
        });
    }

    void PipeTransport::closeStreams() {
        boost::system::error_code ec;
        
        if (m_read_stream.is_open()) {
            m_read_stream.cancel(ec);
            m_read_stream.close(ec);
        }
        
        if (m_write_stream.is_open()) {
            m_write_stream.cancel(ec);
            m_write_stream.close(ec);
        }
        
        if (m_read_fd >= 0) {
            ::close(m_read_fd);
            m_read_fd = -1;
        }
        
        if (m_write_fd >= 0) {
            ::close(m_write_fd);
            m_write_fd = -1;
        }
    }

    void PipeTransport::doConnect() {
        if (!m_running) return;


        std::string read_path = "/tmp/" + m_pipe_path + "_w";
        std::string write_path = "/tmp/" + m_pipe_path + "_r";

        std::cout << "[" << m_name << "] Opening read FIFO: " << read_path << std::endl;
        m_read_fd = ::open(read_path.c_str(), O_RDONLY | O_NONBLOCK);
        if (m_read_fd < 0) {
            std::cerr << "[" << m_name << "] Failed to open read FIFO: " << strerror(errno) << std::endl;
            scheduleReconnect();
            return;
        }

        std::cout << "[" << m_name << "] Opening write FIFO: " << write_path << std::endl;
        m_write_fd = ::open(write_path.c_str(), O_WRONLY | O_NONBLOCK);
        if (m_write_fd < 0) {
            std::cerr << "[" << m_name << "] Failed to open write FIFO: " << strerror(errno) << std::endl;
            ::close(m_read_fd);
            m_read_fd = -1;
            scheduleReconnect();
            return;
        }

        // Присваиваем file descriptors к stream_descriptor
        m_read_stream.assign(m_read_fd);
        m_write_stream.assign(m_write_fd);

        std::cout << "[" << m_name << "] Successfully connected to " << m_pipe_path << std::endl;
        doRead();
    }

    void PipeTransport::scheduleReconnect() {
        if (!m_running) return;
        
        // Переподключение через 2 секунды
        auto timer = std::make_shared<boost::asio::deadline_timer>(m_ios, boost::posix_time::seconds(2));
        timer->async_wait([this, timer](const boost::system::error_code& ec) {
            if (!ec && m_running) {
                std::cout << "[" << m_name << "] Reconnecting..." << std::endl;
                doConnect();
            }
        });
    }

    void PipeTransport::send(const RawData& raw_data) {
        if (!m_running) return;

        std::lock_guard<std::mutex> lock(m_write_mutex);

        bool write_in_progress = !m_write_queue.empty();
        m_write_queue.push(raw_data);

        if (!write_in_progress) {
            doWrite();
        }
    }

    void PipeTransport::doWrite() {
        const RawData& raw_data = m_write_queue.front();
        std::cout << "[PIPE_LAYER] Writing " << raw_data.data.size() << " bytes to pipe." << std::endl;
        boost::asio::async_write(
            m_write_stream,
            boost::asio::buffer(raw_data.data),
            [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                handleWrite(ec, bytes_transferred);
            }
        );
    }

    void PipeTransport::handleWrite(const boost::system::error_code& ec, std::size_t /*bytes_transferred*/) {
        std::lock_guard<std::mutex> lock(m_write_mutex);

        if (!ec) {
            m_write_queue.pop();

            if (!m_write_queue.empty()) {
                doWrite();
            }
        } else {
            if (ec != boost::asio::error::operation_aborted) {
                std::cerr << "[" << m_name << "] Write error: " << ec.message() << std::endl;

                while (!m_write_queue.empty()) {
                    m_write_queue.pop();
                }
                
                closeStreams();
                scheduleReconnect();
            }
        }
    }

    void PipeTransport::doRead() {
        if (!m_running) return;

        m_read_stream.async_read_some(
            boost::asio::buffer(m_read_buffer),
            [this](const boost::system::error_code& ec, std::size_t bytes) {
                handleRead(ec, bytes);
            }
        );
    }

    void PipeTransport::handleRead(const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec) {
            if (ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset) {
                std::cerr << "[" << m_name << "] Disconnected from Arkaim." << std::endl;
                closeStreams();
                scheduleReconnect();
                return;
            }
            if (ec != boost::asio::error::operation_aborted) {
                std::cerr << "[" << m_name << "] Read error: " << ec.message() << std::endl;
                closeStreams();
                scheduleReconnect();
            }
            return;
        }

        if (bytes_transferred > 0 && m_receive_handler) {
            RawData raw_data;
            raw_data.data.assign(m_read_buffer.begin(), m_read_buffer.begin() + bytes_transferred);

            m_receive_handler(raw_data, m_name);
        }

        doRead();
    }

}
