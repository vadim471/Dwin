//
// Created by vadim.tissen on 19.11.2025.
//

#include "bridge/transport/SerialTransport.hpp"
#include <iostream>

#include "bridge/core/constant.hpp"
#include "bridge/core/Logger.hpp"

//SerialTransport.cpp
namespace bridge {
    SerialTransport::~SerialTransport() {
        SerialTransport::stop();
    }

    SerialTransport::SerialTransport(boost::asio::io_service& ios,
                                     unsigned int baud_rate,
                                     const std::string& port,
                                     const std::string& name)
        : ios(ios)
          , port(ios)
          , baud_rate(baud_rate)
          , name(name)
          , port_name(port)
    {
    }

    void SerialTransport::setReceiveHandler(ReceiveHandler handler) {
        receive_handler = std::move(handler); // ref ref по левой ссылке, передаю владение объектом другой функции
    }

    void SerialTransport::start() {
        if (running) return;
        running = true;

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

        // Проверяем, была ли очередь пуста ДО того, как мы добавили элемент.
        // Если была пуста, это наш пакет должен запустить процесс записи.
        bool write_in_progress = !write_serial_queue.empty();
        write_serial_queue.push(raw_data);

        // Если запись сейчас не идет, запускаем её.
        // Если идет, doWrite сам подхватит следующий элемент после завершения текущего.
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
            write_serial_queue.pop(); // Удаляем отправленный пакет

            // Если есть еще что-то в очереди, отправляем дальше
            if (!write_serial_queue.empty()) {
                doWrite();
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
            //LOG_UART_DEBUG << "Received " << bytes_transferred << " bytes from " << port_name;
            RawData raw_data;
            raw_data.data.assign(read_buffer.data.begin(), read_buffer.data.begin() + bytes_transferred);

            receive_handler(raw_data, UART_LAYER);
        }
        doRead();
    }
}
