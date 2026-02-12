//
// Created by vadim.tissen on 04.12.2025.
//

#include "bridge/DwinLogic.hpp"
#include "bridge/utility.hpp"

#include "bridge/constant.hpp"
#include "bridge/MessageLayer.hpp"
#include "bridge/types.hpp"
#include "bridge/DwinCommands.hpp"

namespace bridge {
    DwinLogic::DwinLogic(const Settings &settings) : m_settings(settings) {}



    void DwinLogic::handle(const Message& message, MessageLayer& core) {
        if (message.type == "vp_data") {
            handleDwinEvent(message, core);
        }
        // Если нужно логировать ACK
        else if (message.type == "write_ack") {
            // std::cout << "Write OK" << std::endl;
        }
    }

    // Обработка нажатий на дисплей.
    void DwinLogic::handleDwinEvent(const Message &message, MessageLayer &core) {
        // Проверка длины: VP(2) + Count(1) + Data(2 минимум) = 5 байт
        if (message.payload.size() < 5) return;

        uint16_t vp = (message.payload[0] << 8) | message.payload[1];

        // Нажатие на кнопку количества литров.
        if (vp == m_settings.dwin.vp_chosen_order_volume) {
            handleFuelVolume(message, core);
        }

        // Нажатие на кнопку "стрелки" выбора ТРК.
        if (vp == m_settings.dwin.vp_pagination_trk) {
            handlePaginationButton(message, core);
        }

        // Нажатие на выбранную ТРК.
        if (vp == m_settings.dwin.vp_choose_trk) {
            handleChosenTrk(message, core);
        }

        // Нажатие на кнопки "Назад" или "Отмена" - запрос статуса ТРК.
        if (vp == m_settings.dwin.vp_basic_touch) {
            handleBasicTouchButton(message, core);
        }

        if (vp == m_settings.dwin.vp_next_button) {
            handleNextButton(message, core);
        }

        if (vp == m_settings.dwin.vp_pinpad) {
            handlePinpadButton(message, core);
        }
    }

    void DwinLogic::handleFuelVolume(const Message &message, MessageLayer& core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH;

        msg.payload.push_back(message.payload[0]);
        msg.payload.push_back(message.payload[1]);

        msg.payload.push_back(message.payload[3]);
        msg.payload.push_back(message.payload[4]);

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handlePaginationButton(const Message &message, MessageLayer& core) {
        uint16_t value = (message.payload[3] << 8) | message.payload[4];

        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH;

        int direction = (value == 0x0001) ? 1 : -1;
        msg.payload.push_back(message.payload[0]);
        msg.payload.push_back(message.payload[1]);
        msg.payload.push_back(static_cast<uint8_t>(direction));

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handleChosenTrk(const Message &message, MessageLayer &core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH;
        msg.payload.push_back(message.payload[0]);
        msg.payload.push_back(message.payload[1]);

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handleBasicTouchButton(const Message &message, MessageLayer& core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH;

        msg.payload.push_back(message.payload[0]);
        msg.payload.push_back(message.payload[1]);

        msg.payload.push_back(message.payload[3]);
        msg.payload.push_back(message.payload[4]);

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handleNextButton(const Message &message, MessageLayer &core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH;

        msg.payload.push_back(message.payload[0]);
        msg.payload.push_back(message.payload[1]);

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handlePinpadButton(const Message &message, MessageLayer &core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH;

        msg.payload.push_back(message.payload[0]);
        msg.payload.push_back(message.payload[1]);

        msg.payload.push_back(message.payload[3]);
        msg.payload.push_back(message.payload[4]);

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }
}
