//
// Created by vadim.tissen on 04.12.2025.
//

#include "bridge/DwinLogic.hpp"
#include "bridge/utility.hpp"

#include "bridge/constant.hpp"
#include "bridge/MessageLayer.hpp"
#include "bridge/types.hpp"
#include "bridge/DwinCommands.hpp"
#include "bridge/Logger.hpp"

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
        else if (message.type == PAY_CANCEL_RESPONSE_SUCCESS) {
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_return_money_process_end);
        }
        else if (message.type == PAY_CONFIRM_RESPONSE_SUCCESS) {
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_fuel_ended);
        }
    }

    // Обработка нажатий на дисплей.
    void DwinLogic::handleDwinEvent(const Message &message, MessageLayer &core) {
        // Проверка длины: VP(2) + Count(1) + Data(2 минимум) = 5 байт
        if (message.payload.size() < 5) return;

        uint16_t vp = (message.payload[0] << 8) | message.payload[1];

        if (vp == m_settings.dwin.vp_chosen_order_volume) {
            handleCreateOrder(message, core);
        }

        // Нажатие на кнопку "стрелки" выбора ТРК.
        if (vp == m_settings.dwin.vp_pagination_trk) {
            handlePaginationButton(message, core, USER_TOUCH_PAGINATION_TRK_BUTTON);
        }

        // Нажатие на выбранную ТРК при конфигурации мульти ТРК.
        if (vp == m_settings.dwin.vp_choose_multi_trk) {
            handleTouchMultiTRK(core);
        }

        // Нажатие на выбранную ТРК при конфигурации моно/дуо ТРК.
        if (vp == m_settings.dwin.vp_button_choose_left_trk_double_trk_config
            || vp == m_settings.dwin.vp_button_choose_right_trk_double_trk_config
            || vp == m_settings.dwin.vp_button_choose_trk_uno_trk_config) {
            handleTouchTRK(message, core);
        }

        // Нажатие на кнопки "Назад" или "Отмена" - запрос статуса ТРК.
        if (vp == m_settings.dwin.vp_basic_touch) {
            handleBasicTouch(message, core);
        }

        if (vp == m_settings.dwin.vp_fuel_volume_pinpad || vp == m_settings.dwin.vp_set_fuel_price_pinpad || vp == m_settings.dwin.vp_enter_pin_code_pinpad || vp == m_settings.dwin.vp_set_service_pincode_pinpad ) {
            handleTouchPinPad(message, core);
        }

        if (vp == m_settings.dwin.vp_button_service_menu) {
            handleServiceMenu(message, core);
        }

        if (vp == m_settings.dwin.vp_button_choose_amount_trk) {
            handleAmountTRK(message, core);
        }

        if (vp == m_settings.dwin.vp_button_fuel_edit_price_chosen) {
            // Для редактирования цены топлива 1 вида. Строки с именамии видами топлива.
        }

        if (vp == m_settings.dwin.vp_button_pagination_fuel_type_for_edit) {
            handlePaginationButton(message, core, USER_TOUCH_PAGINATION_FUEL_BUTTON);
        }

        if (vp == m_settings.dwin.vp_button_chosen_fuel_type_for_edit) {
            handleFuelTypeEditing(core);
        }

        if (vp == m_settings.dwin.vp_button_pagination_level_gauges) {
            handlePaginationButton(message, core, USER_TOUCH_PAGINATION_LEVEL_GAUGE_BUTTON);
        }

        if (vp == m_settings.dwin.vp_text_select_trk_button) {
            handleSelectedTRK(message, core);
        }

        if (vp == m_settings.dwin.vp_back_button_service_menu) {

        }

        if (vp == m_settings.dwin.vp_button_cancel_transaction) {
            handleCancelTransaction(message, core);
        }
    }

    void DwinLogic::handleSelectedTRK(const Message &message, MessageLayer &core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_TAP_ON_USED_TRK_BUTTON;
        uint16_t value = (message.payload[3] << 8) | message.payload[4];

        msg.payload.push_back(static_cast<uint8_t>(value));

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handleFuelTypeEditing(MessageLayer& core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_CHOOSE_FUEL_FOR_EDIT_BUTTON;

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handleAmountTRK(const Message &message, MessageLayer &core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_AMOUNT_TRK_BUTTON;

        uint16_t value = (message.payload[3] << 8) | message.payload[4];

        msg.payload.push_back(static_cast<uint8_t>(value));

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handleServiceMenu(const Message &message, MessageLayer &core) {
        Message msg;
        msg.source = UART_LAYER;

        uint16_t value = (message.payload[3] << 8) | message.payload[4];

        if (value == 1) {
            msg.type = USER_TOUCH_SERVICE_MENU_LEVEL_GAUGE_BUTTON;
        } else if (value == 2) {
            msg.type = USER_TOUCH_SERVICE_MENU_CHANGE_FUEL_PRICE_BUTTON;
        } else if (value == 4) {
            msg.type = USER_TOUCH_SET_USED_TRK_BUTTON;
        }
        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handleTouchPinPad(const Message &message, MessageLayer& core) {
        Message msg;
        msg.source = UART_LAYER;

        uint16_t vp = (message.payload[0] << 8) | message.payload[1];
        uint16_t value = (message.payload[3] << 8) | message.payload[4];
        int number = static_cast<int>(value);

        if (number >= 0 && number <= 9 ) {
            size_t dot_position = d_pinpad_buffer.find(",");
            if (dot_position != std::string::npos) {
                if (d_pinpad_buffer.length() - dot_position > 2) {
                    return;
                }
            }

            if (d_pinpad_buffer.length() < 6) {
                if (d_pinpad_buffer == "0") {
                    d_pinpad_buffer = std::to_string(number);
                } else {
                    d_pinpad_buffer += std::to_string(number);
                }
            }
            // Dot.
        } else if (number == 10) {
            if (d_pinpad_buffer.find(',') == std::string::npos) {
                if (d_pinpad_buffer.empty()) {
                    d_pinpad_buffer = "0,";
                } else {
                    d_pinpad_buffer += ",";
                }
            }
            // Decline.
        } else if (number == 11) {
            d_pinpad_buffer.clear();
            clearAllInputPinPad(core);
            // Backspace.
        } else if (number == 12) {
            if (!d_pinpad_buffer.empty()) {
                d_pinpad_buffer.pop_back();
            }
            // Enter.
        } else if (number == 13) {
            if (!d_pinpad_buffer.empty()) {
                int value, exponent;
                std::tie(value, exponent) = utility::formatFloatStringToInt(d_pinpad_buffer);

                if (vp == m_settings.dwin.vp_fuel_volume_pinpad) {
                    msg.type = USER_TOUCH_PIN_PAD_ENTER_FUEL_VOLUME_BUTTON;
                    DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_enterring_volume_order_pinpad, "0",
                                                               m_settings.dwin.text_len_order_integer);
                } else if (vp == m_settings.dwin.vp_set_fuel_price_pinpad) {
                    msg.type = USER_TOUCH_PIN_PAD_ENTER_FUEL_EDITING_BUTTON;
                    DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_text_enter_fuel_price,
                "", m_settings.dwin.text_len_order_integer);
                } else if (vp == m_settings.dwin.vp_set_service_pincode_pinpad) {
                    if (std::stoi(d_pinpad_buffer) == m_settings.gas_station.service_code) {
                        DwinCommands::sendPageToDwin(core, m_settings.dwin.page_service_menu);
                    }
                        DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_text_pinpad_service_code,
               "", m_settings.dwin.text_len_trk_id);

                } else if (vp == m_settings.dwin.vp_enter_pin_code_pinpad) {
                    msg.type = USER_TOUCH_PIN_PAD_ENTER_PIN_CODE_BUTTON;
                }

                msg.payload.assign(d_pinpad_buffer.begin(), d_pinpad_buffer.end());
                d_pinpad_buffer.clear();
                core.sendToLogicLayer(HTTP_LAYER, msg);
            }
            return;
        }
        // Работа с отображением.
        std::string textToShow = d_pinpad_buffer.empty() ? "0" : d_pinpad_buffer;

        if (vp == m_settings.dwin.vp_fuel_volume_pinpad) {
            DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_enterring_volume_order_pinpad,
                textToShow, m_settings.dwin.text_len_order_integer);
        } else if (vp == m_settings.dwin.vp_set_fuel_price_pinpad) {
            DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_text_enter_fuel_price,
               textToShow, m_settings.dwin.text_len_order_integer);
        } else if (vp == m_settings.dwin.vp_set_service_pincode_pinpad) {
            DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_pinpad_service_code, std::string(d_pinpad_buffer.length(), '*'), m_settings.dwin.text_len_trk_id);
        } else if (vp == m_settings.dwin.vp_enter_pin_code_pinpad) {
            DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_enter_pincode,std::string(d_pinpad_buffer.length(), '*'), m_settings.dwin.text_len_trk_id);
        }
    }

    void DwinLogic::clearAllInputPinPad(MessageLayer &core) const {
        DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_enterring_volume_order_pinpad,
                "", m_settings.dwin.text_len_order_integer);
        DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_text_enter_fuel_price,
               "", m_settings.dwin.text_len_order_integer);
        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_pinpad_service_code,
              "", m_settings.dwin.text_len_trk_id);
        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_pinpad_service_code,
               "", m_settings.dwin.text_len_trk_id);
    }

    void DwinLogic::handleCreateOrder(const Message &message, MessageLayer& core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_VOLUME_BUTTON;

        msg.payload.push_back(message.payload[3]);
        msg.payload.push_back(message.payload[4]);

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handleTouchMultiTRK(MessageLayer &core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_CHOOSE_MULTI_TRK_BUTTON;

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handleTouchTRK(const Message &message, MessageLayer& core) {
        uint16_t key_value = (message.payload[3] << 8) | message.payload[4];

        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_CHOOSE_TRK_BUTTON;

        msg.payload.push_back(static_cast<uint8_t>(key_value));

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handleCancelTransaction(const Message &message, MessageLayer& core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_BASIC_TOUCH_CANCEL_TRANSACTION_BUTTON;

        core.sendToLogicLayer(PIPE_LAYER, msg);
        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handleBasicTouch(const Message &message, MessageLayer &core) {
        uint16_t key_value = (message.payload[3] << 8) | message.payload[4];
        Message msg;
        msg.source = UART_LAYER;

        // Кнопка "Назад". Клик для перелистывания страницы назад и запроса статуса ТРК.
        if (key_value == 1) {
            msg.type = USER_TOUCH_BASIC_TOUCH_BUTTON;
        }

        // Кнопка "Далее" после оплаты.
        if (key_value == 2) {
            msg.type = USER_TOUCH_BASIC_TOUCH_BEGIN_FUELLING_BUTTON;
        }

        if (key_value == 3) {
            msg.type = USER_TOUCH_BASIC_TOUCH_CREATE_NEW_ORDER_BUTTON;
        }

        // Кнопка "Назад" после выбора ТРК на экране "Приложите карту". Сбросить выбранную ТРК и вернуться на startPage.
        if (key_value == 4) {
            msg.type = USER_TOUCH_BASIC_TOUCH_ATTACH_CARD_BACK_BUTTON;
        }

        if (key_value == 5) {
            DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_pinpad_service_code,
              "", m_settings.dwin.text_len_trk_id);
            d_pinpad_buffer.clear();
            msg.type = USER_TOUCH_BASIC_TOUCH_DECLINE_ENTER_SERVICE_CODE_BUTTON;
        }

        // Кнопка сброса выбора ТРК редактирования используемых ТРК.
        if  (key_value == 8) {
            msg.type = USER_TOUCH_ACCEPT_SELECTED_TRK_BUTTON;
        }

        // Кнопка подтверждения выбора ТРК на всей АЗС (Подтвердить в нижней панели).
        if (key_value == 9) {
            msg.type = USER_TOUCH_RESET_SELECTED_TRK_BUTTON;
        }

        // Кнопка окончания заказа. Для отправки запроса завершения заказа на PRIME.
        if (key_value == 10) {
            msg.type = USER_TOUCH_CLOSE_ORDER_BUTTON;
        }

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }

    void DwinLogic::handlePaginationButton(const Message &message, MessageLayer& core, const std::string message_type) {
        uint16_t value = (message.payload[3] << 8) | message.payload[4];

        Message msg;
        msg.source = UART_LAYER;
        msg.type = message_type;

        int direction = (value == 0x0001) ? 1 : -1;
        msg.payload.push_back(static_cast<uint8_t>(direction));

        core.sendToLogicLayer(HTTP_LAYER, msg);
    }
}
