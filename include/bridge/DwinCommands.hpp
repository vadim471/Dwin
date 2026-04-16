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

        static void sendTextToDwin(MessageLayer& core, uint16_t vp, const std::string& text, size_t maxLen) {
            Message msg;
            msg.type = DWIN_MESSAGE_TYPE_CHANGE_NUMBER;

            std::vector<uint8_t> payload;

            // 1. Записываем адрес
            payload.push_back((vp >> 8) & 0xFF);
            payload.push_back(vp & 0xFF);

            size_t sendLen = maxLen;
            if (sendLen % 2 != 0) {
                sendLen++;
            }

            for (size_t i = 0; i < sendLen; ++i) {
                if (i < text.length()) {
                    payload.push_back(static_cast<uint8_t>(text[i]));
                } else {
                    payload.push_back(0x00);
                }
            }

            msg.payload = payload;
            core.sendTo(UART_LAYER, msg);
        }

        static void sendRightAlignmentWithPadding(MessageLayer &core, uint16_t vp, const std::string& value, int len) {
            Message msg;
            msg.type = DWIN_MESSAGE_TYPE_CHANGE_NUMBER;

            std::vector<uint8_t> payload;
            // Адрес VP (2 байта)
            payload.push_back((vp >> 8) & 0xFF);
            payload.push_back(vp & 0xFF);

            size_t textLen = value.length();

            size_t padCount = len - textLen;

            for (size_t i = 0; i < padCount; ++i) {
                payload.push_back(0x01);
            }

            for (size_t i = 0; i < textLen; ++i) {
                payload.push_back(static_cast<uint8_t>(value[i]));
            }

            msg.payload = payload;
            core.sendTo(UART_LAYER, msg);
        }

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

        static void sendPlaySoundToDwin(MessageLayer &core, uint8_t fileName) {
            Message msg;
            msg.type = DWIN_MESSAGE_TYPE_PLAY_SOUND;

            std::vector<uint8_t> payload;
            payload.push_back(fileName & 0xFF);

            msg.payload = payload;

            core.sendTo(UART_LAYER, msg);
        }
    };
}