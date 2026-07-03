#include "bridge/transport/SerialTransport.hpp"
#include <iostream>

#include "bridge/core/constant.hpp"
#include "bridge/core/Logger.hpp"
#include "bridge/core/Settings.hpp"

namespace bridge {
    SerialTransport::~SerialTransport() {
        SerialTransport::stop();
    }

    SerialTransport::SerialTransport(boost::asio::io_service &ios, const Settings &settings)
        : ios(ios),
          m_settings(settings),
          port(ios),
          m_throttle_timer(ios)
    {
    }

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
        port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
        port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

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

        m_throttle_timer.cancel(ec);

        if (port.is_open()) {
            port.cancel(ec);
            port.close(ec);
        }
        if (ec) {
            std::cerr << "[" << port_name << "] Error closing port: " << ec.message() << std::endl;
        }
    }

    void SerialTransport::send(const RawData &raw_data) {
        if (!running) return;

        std::lock_guard<std::mutex> lock(write_serial_mutex);
        write_serial_queue.push(raw_data);

        // Если транспорт простаивает, сразу начинаем цикл записи
        if (!m_write_in_progress) {
            m_write_in_progress = true;
            doWrite();
        }
    }

    void SerialTransport::doWrite() {
        if (write_serial_queue.empty()) {
            m_write_in_progress = false;
            m_burst_bytes = 0; // Очередь опустела - сбрасываем счетчик объема
            return;
        }

        const RawData& raw_data = write_serial_queue.front();

        // Если с добавлением этого пакета мы превысим MAX_BURST_BYTES, делаем таймаут
        if (m_burst_bytes > 0 && (m_burst_bytes + raw_data.data.size() > MAX_BURST_BYTES)) {

            m_throttle_timer.expires_from_now(boost::posix_time::milliseconds(10)); // Таймаут 10 мс

            m_throttle_timer.async_wait([this](const boost::system::error_code& ec) {
                // Проверяем, что таймер не был отменен
                if (!ec && running) {
                    std::lock_guard<std::mutex> lock(write_serial_mutex);
                    m_burst_bytes = 0; // Сбрасываем счетчик после паузы
                    doWriteImpl();     // Пишем в порт
                }
            });
        } else {
            // Если объем в норме, просто отправляем данные
            doWriteImpl();
        }
    }

    void SerialTransport::doWriteImpl() {
        const RawData& raw_data = write_serial_queue.front();

        // Увеличиваем счетчик отправленных байт в текущем "бёрсте"
        m_burst_bytes += raw_data.data.size();

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
            doWrite(); // Идем за следующим пакетом
        } else {
            if (ec != boost::asio::error::operation_aborted) {
                std::cerr << "[" << port_name << "] Write error: " << ec.message() << std::endl;
                while (!write_serial_queue.empty()) {
                    write_serial_queue.pop();
                }
            }
            m_write_in_progress = false;
            m_burst_bytes = 0;
        }
    }

    void SerialTransport::doRead() {
        if (!running) return;

        port.async_read_some(
            boost::asio::buffer(read_buffer.data),
            [this](const boost::system::error_code &ec, std::size_t bytes) {
                handleRead(ec, bytes);
            }
        );
    }

    void SerialTransport::handleRead(const boost::system::error_code &ec, std::size_t bytes_transferred) {
        if (ec) {
            if (ec == boost::asio::error::operation_aborted) {
                LOG_UART_WARN << "Read operation aborted: " << ec.message();
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