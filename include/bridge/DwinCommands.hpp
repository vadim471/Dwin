//
// Created by vadim.tissen on 15.01.2026.
//
#pragma once
#include "bridge/MessageLayer.hpp"
#include "bridge/constant.hpp"
#include <vector>

namespace bridge {

    class DwinCommands {
    public:
        static void sendPageToDwin(MessageLayer& core, uint16_t pageId) {
            Message msg;
            msg.type = DWIN_MESSAGE_TYPE_CHANGE_PAGE;

            std::vector<uint8_t> payload;
            payload.push_back((pageId >> 8) & 0xFF);
            payload.push_back(pageId & 0xFF);

            msg.payload = payload;

            core.sendTo(UART_LAYER, msg);
        }

        // Отправлять умноженное на 100, логика дисплея. Отправляет Long Int 4 bytes.
        static void sendFloatToDwin(MessageLayer& core, uint16_t vp, float value) {
            Message msg;
            msg.type = DWIN_MESSAGE_TYPE_CHANGE_NUMBER;

            std::vector<uint8_t> payload;
            // Адрес VP (2 байта)
            payload.push_back((vp >> 8) & 0xFF);
            payload.push_back(vp & 0xFF);

            // Умножаем на 100, так как Dec=2 на экране
            int32_t intValue = static_cast<int32_t>(std::round(value * 100.0f));

            payload.push_back((intValue >> 24) & 0xFF);
            payload.push_back((intValue >> 16) & 0xFF);
            payload.push_back((intValue >> 8) & 0xFF);
            payload.push_back(intValue & 0xFF);

            msg.payload = payload;
            core.sendTo(UART_LAYER, msg);
        }

        static void sendTextToDwin(MessageLayer& core, uint16_t vp, const std::string& text, size_t maxLen) {
            Message msg;
            msg.type = DWIN_MESSAGE_TYPE_CHANGE_NUMBER;

            std::vector<uint8_t> payload;
            // 1. Адрес VP
            payload.push_back((vp >> 8) & 0xFF);
            payload.push_back(vp & 0xFF);

            // 2. Подготовка текста
            // В DWIN один адрес VP вмещает 2 байта (1 слово).
            // Если длина 6 байт, это займет 3 VP адреса.
            for (size_t i = 0; i < maxLen; ++i) {
                if (i < text.length()) {
                    payload.push_back(static_cast<uint8_t>(text[i]));
                } else {
                    payload.push_back(0x00);
                }
            }

            msg.payload = payload;
            core.sendTo(UART_LAYER, msg);
        }
    };

    static void sendInt16ToDwin(MessageLayer &core, uint16_t vp, int value) {
        Message msg;
        msg.type = DWIN_MESSAGE_TYPE_CHANGE_NUMBER;

        std::vector<uint8_t> payload;
        // Адрес VP (2 байта)
        payload.push_back((vp >> 8) & 0xFF);
        payload.push_back(vp & 0xFF);

        payload.push_back((value >> 8) & 0xFF);
        payload.push_back(value & 0xFF);

        msg.payload = payload;
        core.sendTo(UART_LAYER, msg);
    }

}