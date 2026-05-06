#include "bridge/DwinParser.hpp"
#include <iostream>
#include "bridge/types.hpp"
#include <iomanip>
#include <vector>
#include "bridge/constant.hpp"


namespace bridge {
    void pushUInt16(std::vector<uint8_t>& buf, uint16_t value) {
        buf.push_back((value >> 8) & 0xFF);
        buf.push_back(value & 0xFF);
    }

    void pushUInt32(std::vector<uint8_t>& buf, uint32_t value) {
        buf.push_back((value >> 24) & 0xFF);
        buf.push_back((value >> 16) & 0xFF);
        buf.push_back((value >> 8) & 0xFF);
        buf.push_back(value & 0xFF);
    }

    // PARSE: От Дисплея к Нам. Выставляем тип пришедшего пакета.
    std::vector<Message> DwinParser::parse(const RawData& input, const std::string& sourceName) {
        std::vector<Message> messages;

        if (!input.data.empty()) {
            //std::cout << "[DWIN RX] Received " << input.data.size() << " bytes: ";
            for (size_t i = 0; i < input.data.size(); ++i) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                          << static_cast<int>(input.data[i]) << " ";
            }
            std::cout << std::dec << std::endl;
        }

        // 1. Дописываем новые данные в конец буфера
        m_buffer.insert(m_buffer.end(), input.data.begin(), input.data.end());

        // 2. Крутим цикл, пока в буфере есть хотя бы заголовок (3 байта: 5A A5 Len)
        while (m_buffer.size() >= 3) {


            // Проверка заголовка 0x5A 0xA5
            if (m_buffer[0] != 0x5A || m_buffer[1] != 0xA5) {
                // Мусор в начале, удаляем 1 байт и ищем дальше
                m_buffer.erase(m_buffer.begin());
                continue;
            }

            // Читаем длину пакета (3-й байт)
            // У DWIN Length — это кол-во байт ПОСЛЕ байта длины (Cmd + Data)
            uint8_t len = m_buffer[2];

            // Полный размер пакета = 2 (Header) + 1 (LenByte) + len
            size_t packetSize = 3 + len;

            // Если данных в буфере меньше, чем длина пакета — ждем следующей порции
            if (m_buffer.size() < packetSize) {
                break;
            }

            // m_buffer[3] — это Команда (например, 0x83 - Read Response)
            uint8_t cmd = m_buffer[3];

            // DEBUG: Логируем распознанную команду
            std::cout << "[DWIN PARSE] CMD=0x" << std::hex << static_cast<int>(cmd) 
                      << " LEN=" << std::dec << static_cast<int>(len) << std::endl;

            Message msg;
            msg.source = sourceName;


            // Разбираем по командам (пример)
            if (cmd == 0x83) {
                msg.type = "vp_data";

                //(пропускаем Header(2), Len(1), Cmd(1))
                if (packetSize > 4) {
                    msg.payload.assign(m_buffer.begin() + 4, m_buffer.begin() + packetSize);
                }
            } else if (cmd == 0x82) {
                if (len == 3 && m_buffer[4] == 0x4F && m_buffer[5] == 0x4B) {
                    msg.type = "write_ack_ok";
                } else {
                    msg.type = "write_ack";
                    std::cout << "[DWIN] Write ACK (echo) received - ";
                    for (size_t i = 4; i < packetSize && i < m_buffer.size(); ++i) {
                        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                                  << static_cast<int>(m_buffer[i]) << " ";
                    }
                    std::cout << std::dec << std::endl;
                }

                if (packetSize > 4) {
                    msg.payload.assign(m_buffer.begin() + 4, m_buffer.begin() + packetSize);
                }
            }
            else {
                msg.type = "unknown_dwin";
                msg.payload.assign(m_buffer.begin(), m_buffer.begin() + packetSize);
                std::cout << "[DWIN] Unknown command 0x" << std::hex << static_cast<int>(cmd) 
                          << std::dec << " - Full packet: ";
                for (size_t i = 0; i < packetSize && i < m_buffer.size(); ++i) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0') 
                              << static_cast<int>(m_buffer[i]) << " ";
                }
            }

            messages.push_back(msg);

            // Удаляем обработанный пакет из буфера
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + packetSize);
        }

        return messages;
    }

    // SERIALIZE: От Нас к Дисплею
    RawData DwinParser::serialize(const Message& msg) {
        RawData raw;
        std::vector<uint8_t>& data = raw.data;

        // --- СЦЕНАРИЙ 1: СМЕНА СТРАНИЦЫ ---
        if (msg.type == DWIN_MESSAGE_TYPE_CHANGE_PAGE) {
            if (msg.payload.size() != 2) {
                std::cerr << "[DgusParser] Error: change_page requires 2 bytes payload" << std::endl;
                return raw;
            }

            data.push_back(0x5A);
            data.push_back(0xA5);
            data.push_back(0x07); // <-- Длина 0x07
            data.push_back(0x82); // Cmd: Write VP

            data.push_back(0x00); // VP High: 0x0084
            data.push_back(0x84); // VP Low

            data.push_back(0x5A);
            data.push_back(0x01);

            data.push_back(msg.payload[0]); // Page High (0x00)
            data.push_back(msg.payload[1]); // Page Low (0xXX)
        }

        // --- СЦЕНАРИЙ 2: ЗАПИСЬ ЧИСЛА ---
        else if (msg.type == DWIN_MESSAGE_TYPE_CHANGE_NUMBER) {
            if (msg.payload.size() < 2) {
                std::cerr << "[DgusParser] Error: write_vp_data requires VP address." << std::endl;
                return raw;
            }

            size_t data_len = msg.payload.size() - 2; // Длина данных без адреса (напр. 4 байта)

            // Проверка длины
            if (data_len == 0 || data_len > 252) {
                return raw;
            }

            // 1. Заголовок
            data.push_back(0x5A);
            data.push_back(0xA5);
            // Длина пакета: CMD(1) + VP(2) + DATA(N)
            data.push_back(static_cast<uint8_t>(1 + 2 + data_len));
            data.push_back(0x82); // Команда Write VP

            // 2. Адрес VP (первые 2 байта payload)
            data.push_back(msg.payload[0]);
            data.push_back(msg.payload[1]);

            for (size_t i = 2; i < msg.payload.size(); ++i) {
                data.push_back(msg.payload[i]);
            }
        }

        // --- СЦЕНАРИЙ 3: ЗАПИСЬ В VP ---
        else if (msg.type == DWIN_MESSAGE_TYPE_WRITE_VP) {
            if (msg.payload.size() < 2) return raw;

            size_t dataLen = msg.payload.size() - 2;

            data.push_back(0x5A);
            data.push_back(0xA5);
            // Длина: 1(Cmd) + 2(VP) + N(Data)
            data.push_back(static_cast<uint8_t>(1 + 2 + dataLen));
            data.push_back(0x82); // Cmd: Write VP

            // Весь payload (VP + Data) копируем как есть
            data.insert(data.end(), msg.payload.begin(), msg.payload.end());
        }

        else if (msg.type == DWIN_MESSAGE_TYPE_PLAY_SOUND) {
            // if (msg.payload.size() < 2) return raw;


            data.push_back(0x5A);
            data.push_back(0xA5);
            // Длина: 1(Cmd) + 2(VP) + N(Data)
            data.push_back(0x05);
            // Cmd: Write VP
            data.push_back(0x82);
            // vp AUDIO
            data.push_back(0x00);
            data.push_back(0xA0);

            // address audio
            data.push_back(msg.payload[0]);
        }

        // --- СЦЕНАРИЙ 4: УПРАВЛЕНИЕ ЯРКОСТЬЮ (ВЫВОД ИЗ СПЯЩЕГО РЕЖИМА) ---
        else if (msg.type == DWIN_MESSAGE_TYPE_SET_BRIGHTNESS) {
            // Формат: 5A A5 05 82 00 82 [brightness]
            // VP 0x0082 - системный регистр управления подсветкой
            // brightness: 0x00-0x64 (0-100)
            
            if (msg.payload.size() < 3) {
                std::cerr << "[DgusParser] Error: SET_BRIGHTNESS requires 3 bytes (VP + brightness)" << std::endl;
                return raw;
            }

            data.push_back(0x5A);
            data.push_back(0xA5);
            data.push_back(0x05);  // Длина: Cmd(1) + VP(2) + Data(1) + CRC(1)
            data.push_back(0x82);  // Cmd: Write VP
            
            // VP адрес (из payload)
            data.push_back(msg.payload[0]);
            data.push_back(msg.payload[1]);
            
            // Значение яркости
            data.push_back(msg.payload[2]);
        }
        
        if (!data.empty()) {
            for (size_t i = 0; i < data.size(); ++i) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                          << static_cast<int>(data[i]) << " ";
            }
        }

        return raw;
    }

}
