//
// Created by vadim.tissen on [Текущая Дата].
//

#include "bridge/PipeTransport.hpp"
#include <iostream>

namespace bridge {

    PipeTransport::PipeTransport(boost::asio::io_service& ios, const std::string& pipe_path, const std::string& name)
        : m_ios(ios),
          m_socket(ios),
          m_endpoint(pipe_path),
          m_pipe_path(pipe_path),
          m_name(name),
          m_running(false) {
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

        // Безопасное закрытие из потока io_service
        m_ios.post([this]() {
            closeSocket();
        });
    }

    void PipeTransport::closeSocket() {
        boost::system::error_code ec;
        if (m_socket.is_open()) {
            m_socket.cancel(ec);
            m_socket.close(ec);
        }
        if (ec) {
            std::cerr << "[" << m_name << "] Error closing socket: " << ec.message() << std::endl;
        }
    }

    void PipeTransport::doConnect() {
        if (!m_running) return;

        m_socket.async_connect(m_endpoint, [this](const boost::system::error_code& ec) {
            if (!ec) {
                std::cout << "[" << m_name << "] Successfully connected to " << m_pipe_path << std::endl;
                doRead();
            } else {
                std::cerr << "[" << m_name << "] Failed to connect to " << m_pipe_path
                          << ": " << ec.message() << std::endl;

                // Опционально: можно запустить таймер на переподключение, если процесс Arkaim еще не запущен
                // m_timer.expires_from_now(boost::posix_time::seconds(2));
                // m_timer.async_wait([this](...) { doConnect(); });
            }
        });
    }

    void PipeTransport::send(const RawData& raw_data) {
        if (!m_running) return;

        std::lock_guard<std::mutex> lock(m_write_mutex);

        // Если очередь была пуста, значит процесс записи не активен и его нужно запустить
        bool write_in_progress = !m_write_queue.empty();
        m_write_queue.push(raw_data);

        if (!write_in_progress) {
            doWrite();
        }
    }

    void PipeTransport::doWrite() {
        const RawData& raw_data = m_write_queue.front();

        // Гарантируем полную запись фрейма через async_write
        boost::asio::async_write(
            m_socket,
            boost::asio::buffer(raw_data.data),
            [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                handleWrite(ec, bytes_transferred);
            }
        );
    }

    void PipeTransport::handleWrite(const boost::system::error_code& ec, std::size_t /*bytes_transferred*/) {
        std::lock_guard<std::mutex> lock(m_write_mutex);

        if (!ec) {
            m_write_queue.pop(); // Удаляем успешно отправленный пакет

            // Если в очереди есть еще пакеты, продолжаем запись
            if (!m_write_queue.empty()) {
                doWrite();
            }
        } else {
            if (ec != boost::asio::error::operation_aborted) {
                std::cerr << "[" << m_name << "] Write error: " << ec.message() << std::endl;

                // Очищаем очередь при разрыве соединения
                while (!m_write_queue.empty()) {
                    m_write_queue.pop();
                }
            }
        }
    }

    void PipeTransport::doRead() {
        if (!m_running) return;

        m_socket.async_read_some(
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
                // Демон Arkaim упал или закрыл соединение.
                closeSocket();
                return;
            }
            if (ec != boost::asio::error::operation_aborted) {
                std::cerr << "[" << m_name << "] Read error: " << ec.message() << std::endl;
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