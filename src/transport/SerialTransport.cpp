//
// Created by vadim.tissen on 19.11.2025.
//

#include "bridge/transport/SerialTransport.hpp"
#include <iostream>

#include "bridge/core/constant.hpp"
#include "bridge/core/Logger.hpp"
#include "bridge/core/Settings.hpp"

//SerialTransport.cpp
namespace bridge {
    SerialTransport::~SerialTransport() {
        SerialTransport::stop();
    }

    SerialTransport::SerialTransport(boost::asio::io_service& ios, const Settings &settings)
        :   ios(ios),
            m_settings(settings),
            port(ios),
            m_write_timer(ios) // Инициализируем таймер
    {}

    void SerialTransport::setReceiveHandler(ReceiveHandler handler) {
        receive_handler = std::move(handler);
    }

    void SerialTransport::start() {
        if (running) return;
        running = true;
        port_name = m_settings.serial.port;
        baud_rate = m_settings.serial.baud_rate;

        boost::system::error_code ec;

        LOG_UART_INFO << "Opening serial port: " << port_name;
        port.open(port_name, ec);
        if (ec) {
            LOG_UART_ERROR << "Failed to open " << port_name << ": " << ec.message();
            std::cerr << "[UART] Failed to open " << port_name
                      << ": " << ec.message() << std::endl;
            return;
        }

        LOG_UART_INFO << "Configuring serial port: baudrate=" << baud_rate;
        port.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
        port.set_option(boost::asio::serial_port_base::character_size(8));
        port.set_option(boost::asio::serial_port_base::parity(
            boost::asio::serial_port_base::parity::none));
        port.set_option(boost::asio::serial_port_base::stop_bits(
            boost::asio::serial_port_base::stop_bits::one));
        port.set_option(boost::asio::serial_port_base::flow_control(
            boost::asio::serial_port_base::flow_control::none));

        LOG_UART_INFO << "Serial port " << port_name << " opened successfully";
        doRead();
    }

    void SerialTransport::stop() {
        if (!running) return;
        running = false;

        ios.post([this]() {
            closePort();
        });
    }

    void SerialTransport::closePort() {
        boost::system::error_code ec;

        // Обязательно отменяем таймер при остановке транспорта
        m_write_timer.cancel(ec);

        if (port.is_open()) {
            port.cancel(ec);
            port.close(ec);
        }
        if (ec) {
            std::cerr << "[" << port_name << "] Error closing port: " << ec.message() << std::endl;
        }
    }

    void SerialTransport::send(const RawData& raw_data) {
        if (!running) return;

        std::lock_guard<std::mutex> lock(write_serial_mutex);

        bool write_in_progress = !write_serial_queue.empty();
        write_serial_queue.push(raw_data);

        // Если очередь была пуста, отправляем ПЕРВЫЙ пакет мгновенно
        if (!write_in_progress) {
            doWrite();
        }
    }

    void SerialTransport::doWrite() {
        const RawData& raw_data = write_serial_queue.front();

        boost::asio::async_write(
            port,
            boost::asio::buffer(raw_data.data),
            [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                handleWrite(ec, bytes_transferred);
            }
        );
    }

    void SerialTransport::handleWrite(const boost::system::error_code& ec, std::size_t bytes_transferred) {
        std::lock_guard<std::mutex> lock(write_serial_mutex);

        if (!ec) {
            write_serial_queue.pop();

            if (!write_serial_queue.empty()) {
                m_write_timer.expires_from_now(boost::posix_time::milliseconds(5));

                m_write_timer.async_wait([this](const boost::system::error_code& timer_ec) {
                    // Проверяем, что таймер не был отменен (например, вызван stop())
                    if (!timer_ec) {
                        std::lock_guard<std::mutex> timer_lock(write_serial_mutex);
                        if (!write_serial_queue.empty() && running) {
                            doWrite();
                        }
                    }
                });
            }
        } else {
            if (ec != boost::asio::error::operation_aborted) {
                std::cerr << "[" << port_name << "] Write error: " << ec.message() << std::endl;
                while (!write_serial_queue.empty()) {
                    write_serial_queue.pop();
                }
            }
        }
    }

    void SerialTransport::doRead() {
        if (!running) return;

        port.async_read_some(
            boost::asio::buffer(read_buffer.data),
            [this](const boost::system::error_code& ec, std::size_t bytes) {
                handleRead(ec, bytes);
            }
        );
    }

    void SerialTransport::handleRead(const boost::system::error_code& ec,
                                 std::size_t bytes_transferred) {
        if (ec) {
            if (ec == boost::asio::error::operation_aborted) {
                LOG_UART_WARN << "Read operation aborted: " << ec.message();
                std::cerr << "[UART] read error: " << ec.message() << std::endl;
            }
            return;
        }

        if (bytes_transferred > 0 && receive_handler) {
            RawData raw_data;
            raw_data.data.assign(read_buffer.data.begin(), read_buffer.data.begin() + bytes_transferred);

            receive_handler(raw_data);
        }
        doRead();
    }
}