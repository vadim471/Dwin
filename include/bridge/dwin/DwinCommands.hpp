//
// Created by vadim.tissen on 15.01.2026.
//
#pragma once
#include "bridge/core/MessageLayer.hpp"
#include "bridge/core/constant.hpp"
#include "bridge/core/utility.hpp"
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

            std::string cp1251_value = utility::convertUtf8ToCp1251(value);

            std::vector<uint8_t> payload;
            // Адрес VP (2 байта)
            payload.push_back((vp >> 8) & 0xFF);
            payload.push_back(vp & 0xFF);

            size_t textLen = cp1251_value.length();

            size_t padCount = len - textLen;

            for (size_t i = 0; i < padCount; ++i) {
                payload.push_back(0x01);
            }

            for (size_t i = 0; i < textLen; ++i) {
                payload.push_back(static_cast<uint8_t>(cp1251_value[i]));
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

        // Управление подсветкой (вывод из спящего режима)
        // brightness: 0-100 (0 = выключить, 100 = максимальная яркость)
        static void sendSetBrightnessToDwin(MessageLayer &core, uint8_t brightness) {
            Message msg;
            msg.type = DWIN_MESSAGE_TYPE_SET_BRIGHTNESS;

            std::vector<uint8_t> payload;
            // VP адрес для управления подсветкой (обычно 0x0000)
            payload.push_back(0x00);
            payload.push_back(0x00);
            // Яркость (0-100)
            payload.push_back(brightness & 0xFF);

            msg.payload = payload;

            core.sendTo(UART_LAYER, msg);
        }

        // Быстрый вывод из спящего режима (100% яркость)
        static void sendWakeUpDwin(MessageLayer &core) {
            sendSetBrightnessToDwin(core, 100);
        }

        static void sendTriPartFloatToDwin(uint16_t thousands_vp, uint16_t units_vp, uint16_t kopecks_vp,
                                        MessageLayer &core,
                                        int thousands_length, int units_length, int kopecks_length,
                                        const std::string& value) {
            std::string int_part, kopecks_part;

            std::tie(int_part, kopecks_part) = utility::splitFloatString(value, kopecks_length);

            std::string thousands_part = "";
            std::string units_part = "0";


            bool is_negative = false;
            if (!int_part.empty() && int_part[0] == '-') {
                is_negative = true;
                int_part = int_part.substr(1);
            }


            if (int_part.length() > 3) {
                units_part = int_part.substr(int_part.length() - 3);
                thousands_part = int_part.substr(0, int_part.length() - 3);
            } else {
                units_part = int_part;
                thousands_part = "";
            }

            if (is_negative) {
                if (!thousands_part.empty()) {
                    thousands_part = "-" + thousands_part;
                } else {
                    units_part = "-" + units_part;
                }
            }

            sendRightAlignmentWithPadding(core, thousands_vp, thousands_part, thousands_length);
            sendRightAlignmentWithPadding(core, units_vp, units_part, units_length);
            sendRightAlignmentWithPadding(core, kopecks_vp, kopecks_part, kopecks_length);
        }
    };
}