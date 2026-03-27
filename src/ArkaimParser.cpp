//
// Created by vadim.tissen on 26.03.2026.
//

#include "bridge/ArkaimParser.hpp"
#include <iostream>
#include "infrastructure/itp-c/include/itp/error.h"

namespace bridge {

    std::vector<Message> ArkaimParser::parse(const RawData& input, const std::string& sourceName) {
        std::vector<Message> messages;

        // 1. Дописываем новые данные в конец буфера
        m_buffer.insert(m_buffer.end(), input.data.begin(), input.data.end());

        // 2. Ищем заголовки и длину пакета Arkaim (CL2)
        // Примерная структура кадра CL2: [Header/Sync] [Length] [Command] [Payload] [CRC]
        // Это псевдокод, так как точный формат зависит от реализации Arkaim CL2

        while (m_buffer.size() >= 5) { // Минимальная длина фрейма

            // Если в протоколе CL2 есть стартовый байт (например 0x02 или 0x5A)
            /*
            if (m_buffer[0] != SYNC_BYTE) {
                m_buffer.erase(m_buffer.begin());
                continue;
            }
            */

            // Допустим, 2-й и 3-й байты — это длина
            // uint16_t packetLen = (m_buffer[1] << 8) | m_buffer[2];
            // size_t totalSize = packetLen + header_size;

            // Заглушка: если пока не умеем искать длину, просто отдаем всё
            size_t totalSize = m_buffer.size();

            if (m_buffer.size() < totalSize) {
                break; // Ждем следующую порцию данных
            }

            Message msg;
            msg.source = sourceName;
            msg.type = "ARKAIM_RESPONSE";

            // В resource_id можно положить Command ID, чтобы логика знала, что за ответ
            // msg.resource_id = std::to_string(m_buffer[3]);

            msg.payload.assign(m_buffer.begin(), m_buffer.begin() + totalSize);
            messages.push_back(msg);

            // Удаляем обработанный пакет из буфера
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + totalSize);
        }

        return messages;
    }

    RawData ArkaimParser::serialize(const Message& msg) {
        RawData raw;

        // Транзитом прокидываем байты, сформированные в ArkaimLogic через itp::frame
        if (msg.type == "ARKAIM_REQUEST") {
            // Если протокол требует обертки (например, CRC или стартовые байты),
            // которых нет в itp::frame, их нужно добавить здесь.
            // Иначе — просто копируем payload.

            raw.data.insert(raw.data.end(), msg.payload.begin(), msg.payload.end());
        }

        return raw;
    }

}