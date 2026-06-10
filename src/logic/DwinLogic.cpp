//
// Created by vadim.tissen on 04.12.2025.
//

#include "bridge/logic/DwinLogic.hpp"
#include "bridge/core/utility.hpp"

#include "bridge/core/constant.hpp"
#include "bridge/core/MessageLayer.hpp"
#include "bridge/core/types.hpp"
#include "bridge/dwin/DwinCommands.hpp"
#include "bridge/core/Logger.hpp"

namespace bridge {
    DwinLogic::DwinLogic(const Settings &settings) : m_settings(settings) {
        LOG_UART_INFO << "DwinLogic initialized";
    }

    void DwinLogic::setCurrentOrderVolumeOnDisplay(MessageLayer &core, const std::string &value) const {
        std::string int_part_target_volume, dec_part_target_volume;
        std::tie(int_part_target_volume, dec_part_target_volume) = utility::splitFloatString(value, 2);

        for (uint16_t vp: m_settings.dwin.vp_current_order_volume_integer_pages) {
            if (vp == 0) continue;
            DwinCommands::sendRightAlignmentWithPadding(core, vp,
                                                        int_part_target_volume, m_settings.dwin.text_len_order_integer);
        }

        for (uint16_t vp: m_settings.dwin.vp_current_order_volume_decimal_pages) {
            if (vp == 0) continue;
            DwinCommands::sendRightAlignmentWithPadding(core, vp,
                                                        dec_part_target_volume, m_settings.dwin.text_len_order_decimal);
        }
    }

    void DwinLogic::setCurrentOrderAmountOnDisplay(MessageLayer &core, const std::string &value) const {
        std::string int_part_target_amount, dec_part_target_amount;
        std::tie(int_part_target_amount, dec_part_target_amount) = utility::splitFloatString(value, 2);

        for (uint16_t vp: m_settings.dwin.vp_current_order_amount_integer_pages) {
            if (vp == 0) continue;
            DwinCommands::sendRightAlignmentWithPadding(core, vp,
                                                        int_part_target_amount, m_settings.dwin.text_len_order_integer);
        }

        for (uint16_t vp: m_settings.dwin.vp_current_order_amount_decimal_pages) {
            if (vp == 0) continue;
            DwinCommands::sendRightAlignmentWithPadding(core, vp,
                                                        dec_part_target_amount, m_settings.dwin.text_len_order_decimal);
        }
    }

    void DwinLogic::setCurrentFuellingVolume(MessageLayer &core, const std::string &value) {
        std::string int_part_target_volume, dec_part_target_volume;
        std::tie(int_part_target_volume, dec_part_target_volume) = utility::splitFloatString(value, 2);

        for (uint16_t vp: m_settings.dwin.vp_current_fuel_volume_integer) {
            if (vp == 0) continue;
            DwinCommands::sendRightAlignmentWithPadding(core, vp,
                                                        int_part_target_volume, m_settings.dwin.text_len_order_integer);
        }

        for (uint16_t vp: m_settings.dwin.vp_current_fuel_volume_decimal) {
            if (vp == 0) continue;
            DwinCommands::sendRightAlignmentWithPadding(core, vp,
                                                        dec_part_target_volume, m_settings.dwin.text_len_order_decimal);
        }
    }

    void DwinLogic::setCurrentFuellingAmount(MessageLayer &core, const std::string &value) {
        std::string int_part_target_volume, dec_part_target_volume;
        std::tie(int_part_target_volume, dec_part_target_volume) = utility::splitFloatString(value, 2);
        DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_current_order_amount_integer,
                                                        int_part_target_volume, m_settings.dwin.text_len_order_integer);
        DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_current_order_amount_decimal,
                                                        dec_part_target_volume, m_settings.dwin.text_len_order_decimal);
    }

    void DwinLogic::handle(const Message& message, MessageLayer& core) {
        // Отправить ID и версию один раз при первом сообщении
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
        else if (message.type == UPDATE_CURRENT_ORDER_AMOUNT) {
            std::string str(message.payload.begin(), message.payload.end());
            setCurrentOrderAmountOnDisplay(core, str);
        }
        else if (message.type == UPDATE_CURRENT_ORDER_VOLUME) {
            std::string str(message.payload.begin(), message.payload.end());
            setCurrentOrderVolumeOnDisplay(core, str);
        }
        else if (message.type == UPDATE_CURRENT_FUELLING_VOLUME) {
            std::string str(message.payload.begin(), message.payload.end());
            setCurrentFuellingVolume(core, str);
        } else if (message.type == UPDATE_CURRENT_FUELLING_AMOUNT) {
            std::string str(message.payload.begin(), message.payload.end());
            setCurrentFuellingAmount(core, str);
        } else if (message.type == CLEAR_CURRENT_ORDER_AMOUNT_AND_VOLUME) {
            clearInputAmountAndVolumeText(core);
        } else if (message.type == ON_GET_BALANCE_RESPONSE) {
            handleGetBalanceResponse(message, core);
        }
    }

    void DwinLogic::handleGetBalanceResponse(const Message &message, MessageLayer& core) const {
        std::string receipt = std::string(message.payload.begin(), message.payload.end());
        ParsedReceipt parsed_receipt = utility::parseTerminalBalanceReceipt(receipt);

        // Отправка баланса карты.
        DwinCommands::sendTriPartFloatToDwin(m_settings.dwin.vp_card_balance_thousands, m_settings.dwin.vp_card_balance_hundreds,
            m_settings.dwin.vp_card_balance_tens, core, m_settings.dwin.text_len_fuel_integer, m_settings.dwin.text_len_fuel_integer, m_settings.dwin.text_len_order_decimal, parsed_receipt.balance_before);

        // Отправка типа карты.
        std::string wallet_type_to_dwin = utility::getWalletSymbol(parsed_receipt.wallet_type);
        DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_card_balance_type, wallet_type_to_dwin, 2); // m_settings.dwin.text_len_fuel_integer

        //Отправка номера карты.
        std::string first_part_card_number, second_part_card_number;
        std::tie(first_part_card_number, second_part_card_number) = utility::splitByFirstSpace(parsed_receipt.card_number);
        DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_card_balance_number_first_part, first_part_card_number, m_settings.dwin.text_len_fuel_type);
        DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_card_balance_number_second_part, second_part_card_number, m_settings.dwin.text_len_standalone_id);

        DwinCommands::sendPageToDwin(core, m_settings.dwin.page_card_balance);
    }

    // Обработка нажатий на дисплей.
    void DwinLogic::handleDwinEvent(const Message &message, MessageLayer &core) {
        // Проверка длины: VP(2) + Count(1) + Data(2 минимум) = 5 байт
        if (message.payload.size() < 5) {
            LOG_UART_WARN << "Invalid payload size: " << message.payload.size();
            return;
        }

        uint16_t vp = (message.payload[0] << 8) | message.payload[1];
        uint16_t value = (message.payload[3] << 8) | message.payload[4];
        
        //LOG_UART_INFO << "Touch event: VP=0x" << std::hex << vp << " Value=0x" << value << std::dec;

        if (vp == m_settings.dwin.vp_chosen_order_volume) {
            LOG_UART_INFO << "Order volume chosen";
            if (m_itp_logger) {
                m_itp_logger->trace(1, "DwinLogic", "Order volume chosen");
            }
            handleCreateOrder(message, core);
        }

        if (vp == m_settings.dwin.vp_button_get_balance) {
            LOG_UART_INFO << "Get balance button pressed";
            if (m_itp_logger) {
                m_itp_logger->trace(1, "DwinLogic", "Get balance button pressed");
            }
            handleGetCardBalance(core);
        }

        // Нажатие на кнопку "стрелки" выбора ТРК.
        if (vp == m_settings.dwin.vp_pagination_trk) {
            LOG_UART_INFO << "TRK pagination button pressed";
            handlePaginationButton(message, core, USER_TOUCH_PAGINATION_TRK_BUTTON);
        }

        if (vp == m_settings.dwin.vp_button_pagination_reception_tanker) {
            handlePaginationButton(message, core, USER_TOUCH_PAGINATION_RECEPTION_LEVEL_GAUGE_BUTTON);
        }

        if (vp == m_settings.dwin.vp_button_choose_reception_fuel) {
            if (m_itp_logger) {
                m_itp_logger->trace(1, "DwinLogic", "Reception fuel button pressed");
            }
            handleTouchReceptionTanker(core);
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
            LOG_UART_INFO << "Basic touch button pressed, value=" << value;
            handleBasicTouch(message, core);
        }

        if (vp == m_settings.dwin.vp_fuel_volume_pinpad ||
            vp == m_settings.dwin.vp_set_fuel_price_pinpad ||
            vp == m_settings.dwin.vp_enter_pin_code_pinpad ||
            vp == m_settings.dwin.vp_set_service_pincode_pinpad ||
            vp == m_settings.dwin.vp_button_number_document_pinpad) {
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

        if (vp == m_settings.dwin.vp_button_cancel_transaction) {
            handleCancelTransaction(message, core);
        }

        if (vp == m_settings.dwin.vp_button_finish_reception_fuel) {
            if (m_itp_logger) {
                m_itp_logger->trace(1, "DwinLogic", "Finish reception fuel button pressed");
            }
            handleFinishReceptionFuel(core);
        }
    }

    void DwinLogic::handleFinishReceptionFuel(MessageLayer &core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_FINISH_RECEPTION_FUEL_BUTTON;

        core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
    }

    void DwinLogic::handleGetCardBalance(MessageLayer &core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = PAY_GET_BALANCE;

        core.sendToLogicLayer(PIPE_LAYER, msg);
        core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
    }

    void DwinLogic::handleSelectedTRK(const Message &message, MessageLayer &core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_TAP_ON_USED_TRK_BUTTON;
        uint16_t value = (message.payload[3] << 8) | message.payload[4];

        msg.payload.push_back(static_cast<uint8_t>(value));

        core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
    }

    void DwinLogic::handleFuelTypeEditing(MessageLayer& core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_CHOOSE_FUEL_FOR_EDIT_BUTTON;

        core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
    }

    void DwinLogic::handleAmountTRK(const Message &message, MessageLayer &core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_AMOUNT_TRK_BUTTON;

        uint16_t value = (message.payload[3] << 8) | message.payload[4];

        msg.payload.push_back(static_cast<uint8_t>(value));

        core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
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
        } else if (value == 5) {
            msg.type = USER_TOUCH_SERVICE_MENU_RECEPTION_LEVEL_GAUGE_BUTTON;
        }
        core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
    }

    void DwinLogic::clearInputAmountAndVolumeText(MessageLayer &core) {
        setCurrentOrderVolumeOnDisplay(core, "0,00");
        setCurrentOrderAmountOnDisplay(core, "0,00");
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
                    DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_enter_pincode, "", m_settings.dwin.text_len_trk_id);
                } else if (vp == m_settings.dwin.vp_button_number_document_pinpad) {
                    msg.type = USER_TOUCH_PIN_PAD_ENTER_DOCUMENT_NUMBER_BUTTON;
                    DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_number_document_pinpad, "", m_settings.dwin.text_len_trk_id);
                }

                msg.payload.assign(d_pinpad_buffer.begin(), d_pinpad_buffer.end());
                d_pinpad_buffer.clear();
                core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
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
        } else if (vp == m_settings.dwin.vp_button_number_document_pinpad) {
            DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_number_document_pinpad, textToShow, m_settings.dwin.text_len_trk_id);
        }
    }

    void DwinLogic::clearAllInputPinPad(MessageLayer &core) const {
        DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_enterring_volume_order_pinpad,
                "", m_settings.dwin.text_len_order_integer);
        DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_text_enter_fuel_price,
               "", m_settings.dwin.text_len_order_integer);
        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_pinpad_service_code,
              "", m_settings.dwin.text_len_trk_id);
        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_enter_pincode,
               "", m_settings.dwin.text_len_trk_id);
        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_number_document_pinpad,
               "", m_settings.dwin.text_len_trk_id);
    }

    void DwinLogic::handleCreateOrder(const Message &message, MessageLayer& core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_VOLUME_BUTTON;

        msg.payload.push_back(message.payload[3]);
        msg.payload.push_back(message.payload[4]);

        core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
    }

    void DwinLogic::handleTouchMultiTRK(MessageLayer &core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_CHOOSE_MULTI_TRK_BUTTON;

        core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
    }

    void DwinLogic::handleTouchTRK(const Message &message, MessageLayer& core) {
        uint16_t key_value = (message.payload[3] << 8) | message.payload[4];

        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_CHOOSE_TRK_BUTTON;

        msg.payload.push_back(static_cast<uint8_t>(key_value));

        core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
    }

    void DwinLogic::handleCancelTransaction(const Message &message, MessageLayer& core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_BASIC_TOUCH_CANCEL_TRANSACTION_BUTTON;

        core.sendToLogicLayer(PIPE_LAYER, msg);
        core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
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

        // Кнопка отмены входа в сервисное меню. Необходимо для корректного отображения конфигурации ТРК (моно дуо мульти).
        if (key_value == 5) {
            DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_pinpad_service_code,
              "", m_settings.dwin.text_len_trk_id);
            d_pinpad_buffer.clear();
            msg.type = USER_TOUCH_BASIC_TOUCH_DECLINE_ENTER_SERVICE_CODE_BUTTON;
        }

        // Кнопка отмены транзакции при вводе пин-кода. Кейс - приложили карту, создали заказ, нажали с пин-кода назад.
        if (key_value == 6) {
            msg.type = USER_TOUCH_CANCEL_TRANSACTION_AFTER_CANCEL_ENTER_PIN;
        }

        // Кнопка ОК выхода с баланса карты
        if (key_value == 7) {
            // TODO или нет
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

        // Кнопка подтверждения начала приема топлива. Необходима печать чека текущего состояния резервуара.
        if (key_value == 11) {
            msg.type = USER_TOUCH_ACCEPT_RECEPTION_FUEL_BUTTON;
        }

        core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
    }

    void DwinLogic::handlePaginationButton(const Message &message, MessageLayer& core, const std::string message_type) {
        uint16_t value = (message.payload[3] << 8) | message.payload[4];

        Message msg;
        msg.source = UART_LAYER;
        msg.type = message_type;

        int direction = (value == 0x0001) ? 1 : -1;
        msg.payload.push_back(static_cast<uint8_t>(direction));

        core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
    }

    void DwinLogic::handleTouchReceptionTanker(MessageLayer& core) {
        Message msg;
        msg.source = UART_LAYER;
        msg.type = USER_TOUCH_CHOOSE_RECEPTION_LEVEL_GAUGE_BUTTON;

        core.sendToLogicLayer(PRIME_HTTP_LAYER, msg);
    }

    void DwinLogic::sendStandaloneIdAndVersion(MessageLayer& core) {
        LOG_UART_INFO << "Sending Standalone ID and Version to all pages";

        const std::string& standalone_id = m_settings.gas_station.standalone_id;
        
        // Отправить ID терминала на все страницы
        for (uint16_t vp : m_settings.dwin.vp_text_standalone_id_pages) {
            if (vp == 0) continue;
            DwinCommands::sendTextToDwin(core, vp, standalone_id, 20);
        }

        // Отправить версию программы на все страницы
        for (uint16_t vp : m_settings.dwin.vp_text_standalone_version_pages) {
            if (vp == 0) continue;
            DwinCommands::sendTextToDwin(core, vp, APP_VERSION, 30);
        }

        LOG_UART_INFO << "Standalone ID: " << standalone_id << ", Version: " << APP_VERSION;
    }

    void DwinLogic::setStandaloneIdAndVersion(MessageLayer &core) const {
        const std::string &standalone_id = m_settings.gas_station.standalone_id;

        for (uint16_t vp: m_settings.dwin.vp_text_standalone_id_pages) {
            if (vp == 0) continue;
            DwinCommands::sendTextToDwin(core, vp, standalone_id, m_settings.dwin.text_len_standalone_id);
        }

        for (uint16_t vp: m_settings.dwin.vp_text_standalone_version_pages) {
            if (vp == 0) continue;
            DwinCommands::sendTextToDwin(core, vp, APP_VERSION, m_settings.dwin.text_len_standalone_version);
        }
    }
}
