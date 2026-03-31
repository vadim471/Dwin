//
// Created by vadim.tissen on 26.03.2026.
//

#include "bridge/ArkaimParser.hpp"
#include <iostream>

extern "C" {
#include <itp/protocol.h>
#include <itp/package.h>
#include <itp/frame.h>
#include <itp/error_code.h>
#include <itp/memory.h>
#include <itp/transmit.h>
}

namespace bridge {

    std::vector<Message> ArkaimParser::parse(const RawData& input, const std::string& sourceName) {
        std::vector<Message> messages;

        m_buffer.insert(m_buffer.end(), input.data.begin(), input.data.end());

        while (m_buffer.size() >= 8) {

            itp_buffer_t temp_buffer;
            temp_buffer.data = m_buffer.data();
            temp_buffer.size = m_buffer.size();
            temp_buffer.length = m_buffer.size();
            temp_buffer.complete = 0;
            temp_buffer.time = 0;
            
            // Проверяем структуру пакета
            itp_size_t expected_size = 0;
            itp_parse_status_t check_result = itp_check_package(&temp_buffer, &expected_size);
            
            if (check_result == ITP_PARSE_OK) {
                // Пакет полный и корректный
                itp_error_code_t error = ITP_ERRC_NONE;
                itp_package_tp package = itp_extract_package(&temp_buffer, &error);
                
                if (package && error == ITP_ERRC_NONE) {
                    itp_frame_tp frame = itp_parse_package(package, &error);
                    
                    if (frame && error == ITP_ERRC_NONE) {
                        Message msg;
                        msg.source = sourceName;
                        msg.type = "ARKAIM_RESPONSE";
                        msg.timestamp = std::chrono::system_clock::now();
                        
                        // Сохраняем command ID для маршрутизации
                        msg.resource_id = std::to_string(frame->command);
                        
                        // Копируем payload данные
                        if (frame->data && frame->length > 0) {
                            msg.payload.assign(frame->data, frame->data + frame->length);
                        }
                        
                        messages.push_back(msg);
                        
                        // Удаляем обработанные байты
                        size_t consumed = temp_buffer.complete;
                        if (consumed > 0) {
                            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + consumed);
                        } else {
                            // Если complete не установлен, используем размер пакета
                            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + package->size);
                        }
                        
                        itp_free_frame(frame);
                    } else {
                        std::cerr << "[ArkaimParser] Failed to parse package to frame" << std::endl;
                        m_buffer.erase(m_buffer.begin());
                    }
                    
                    itp_free_package(package);
                } else {
                    std::cerr << "[ArkaimParser] Failed to extract package" << std::endl;
                    m_buffer.erase(m_buffer.begin());
                }
            } 
            else if (check_result == ITP_PARSE_NOT_END) {
                // Недостаточно данных, ждем еще
                if (expected_size > 0 && m_buffer.size() < expected_size) {
                    break;
                }
            } 
            else {
                itp_parse_status_t repair_result = itp_repair_offset(&temp_buffer);
                if (repair_result == ITP_PARSE_OK && temp_buffer.complete > 0) {
                    m_buffer.erase(m_buffer.begin(), m_buffer.begin() + temp_buffer.complete);
                } else {
                    m_buffer.erase(m_buffer.begin());
                }
            }
        }

        return messages;
    }

    RawData ArkaimParser::serialize(const Message& msg) {
        RawData raw;

        if (msg.type == ARKAIM_REQUEST) {
            raw.data = msg.payload;
        }

        return raw;
    }
}