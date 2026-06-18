//
// Created by vadim.tissen on 12.12.2025.
//

#include "bridge/logic/PrimeLogic.hpp"
#include "bridge/core/MessageLayer.hpp"
#include <iostream>
#include "bridge/core/utility.hpp"
#include "bridge/core/constant.hpp"
#include "bridge/dwin/DwinCommands.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "bridge/dwin/PrimeCommands.hpp"
#include "bridge/payment/PaymentData.hpp"
#include <algorithm>
#include <iomanip>
#include <set>
#include <sstream>
#include <tuple>
#include <utility>

#include "bridge/bos/SalesData.hpp"
#include "bridge/payment/CardData.hpp"


// Для формирования запросов на дисплей. Тут я разбираю ответ от сервера и составляю Message для дисплея.
namespace bridge {
    PrimeLogic::PrimeLogic(const Settings &settings, boost::asio::io_service &ios) :

        m_settings(settings),
        m_timer(ios),
        m_ios(ios),
        m_ui_timer(ios),
        m_waitingResponse(false) {
    }

    void PrimeLogic::getDevices(MessageLayer &core) {
        Message request;
        request.type = GET_DEVICES;

        core.sendTo(PRIME_HTTP_LAYER, request);
    }

    void PrimeLogic::getParameters(MessageLayer &core, const std::string &id) {
        Message request;
        request.type = GET_IO_PARAMETERS;
        request.resource_id = id;

        core.sendTo(PRIME_HTTP_LAYER, request);
    }

    void PrimeLogic::sendTaskToHttp(MessageLayer &core, std::string id) {
        Message request;
        request.type = GET_TASKS;
        request.resource_id = id;

        core.sendTo(PRIME_HTTP_LAYER, request);
    }

    void PrimeLogic::getProducts(MessageLayer &core) {
        Message request;
        request.type = GET_PRODUCTS;

        core.sendTo(PRIME_HTTP_LAYER, request);
    }

    void PrimeLogic::getTankers(MessageLayer &core) {
        Message request;
        request.type = GET_TANKERS;

        core.sendTo(PRIME_HTTP_LAYER, request);
    }

    void PrimeLogic::getDispenserStatus(MessageLayer &core, const std::string &trk_id) {
        if (!trk_id.empty()) {
            Message request;
            request.type = GET_DISPENSER_STATUS;
            request.resource_id = trk_id;

            core.sendTo(PRIME_HTTP_LAYER, request);
        }
    }

    void PrimeLogic::startPolling(MessageLayer &core) const {
        Message request;
        request.type = GET_EVENTS;
        request.resource_id = std::to_string(m_lastId);

        core.sendTo(PRIME_HTTP_LAYER, request);
    }

    void PrimeLogic::startLoop(MessageLayer &core) {
        if (m_itp_logger) {
            m_itp_logger->trace(1, "HttpLogic", "Starting HTTP logic loop");
        }

        getDevices(core);
        scheduleNextTick(core);
        getProducts(core);
        getTankers(core);

        clearInputTextDwin(core);

        m_amount_trk = m_settings.gas_station.amount_trk;
        m_amount_trk_png = utility::getIconForGasStation(m_amount_trk, m_settings);
        m_selected_dispensers = m_settings.gas_station.used_trks;

        // После считывания конфига количества ТРК.
        DwinCommands::sendPageToDwin(core, getStartPage());
        //setStandaloneIdAndVersion(core);

        if (m_itp_logger) {
            m_itp_logger->trace(1, "HttpLogic", "HTTP logic loop started successfully");
        }
    }

    void PrimeLogic::scheduleNextTick(MessageLayer &core) {
        m_timer.expires_from_now(boost::posix_time::milliseconds(1000));

        auto self = shared_from_this();

        // async_wait работает так же
        m_timer.async_wait([self, &core](const boost::system::error_code &ec) {
            // Проверка, жив ли еще объект core теоретически не нужна, если он в main,
            // но в идеале лучше передавать weak_ptr на core или гарантировать порядок уничтожения.
            if (!ec) {
                self->onTimerExpired(ec, core);
            }
        });
    }

    void PrimeLogic::onTimerExpired(const boost::system::error_code &ec, MessageLayer &core) {
        if (ec) return;

        // Watchdog
        // сбрасываем флаг, если зависли
        if (m_waitingResponse.load()) {
        }

        if (!m_waitingResponse.load()) {
            startPolling(core);
            m_waitingResponse.store(true);
        }
        checkTimers(core);

        // Таймер останова налива топлива.
        checkFuellingProcess(core);
        if (m_reception_active && !m_reception_level_gauge_id.empty()) {
            getParameters(core, m_reception_level_gauge_id);
        }
        setFooterDateTime(core);
        // Перезавод таймера
        scheduleNextTick(core);
    }

    void PrimeLogic::handle(const Message &message, MessageLayer &core) {
        m_waitingResponse.store(false);
        if (message.type == ARKAIM_INITIALIZED) {
            std::cout << "[PrimeLogic] Arkaim is ready, starting HTTP loop..." << std::endl;
            startLoop(core);
            LOG_SYSTEM_INFO << "Starting Prime logic loop";
            return;
        }
        if (message.type == USER_TOUCH_ACCEPT_RECEPTION_FUEL_BUTTON) {
            processReceptionTanker(core);
            return;
        }
        if (message.type == USER_TOUCH_FINISH_RECEPTION_FUEL_BUTTON) {
            handleFinishReceptionFuel(core);
            return;
        }
        if (message.type == HTTP_RESPONSE) {
            processHandleHttp(message, core);
            return;
        }
        if (message.type == USER_TOUCH_VOLUME_BUTTON) {
            uint16_t volume = (message.payload[0] << 8) | message.payload[1];
            handleCreateOrder(core, volume, 0);
            return;
        }
        if (message.type == USER_TOUCH_PAGINATION_TRK_BUTTON) {
            int8_t direction = static_cast<int8_t>(message.payload[0]);
            handlePaginationDispenser(direction, core);
            return;
        }
        if (message.type == USER_TOUCH_CHOOSE_MULTI_TRK_BUTTON) {
            handleChooseTRK(core, m_current_dispenser_index);
            return;
        }
        if (message.type == USER_TOUCH_CHOOSE_TRK_BUTTON) {
            int8_t index = static_cast<int8_t>(message.payload[0]);
            handleChooseTRK(core, index);
            return;
        }
        if (message.type == USER_TOUCH_BASIC_TOUCH_BUTTON) {
            handleBasicTouch(core);
            return;
        }
        if (message.type == USER_TOUCH_BASIC_TOUCH_BEGIN_FUELLING_BUTTON) {
            m_dwin_button_next_for_start_fuel = true;
            authorizeOnServer(core);
            printDebetReceipt(core);
            return;
        }
        if (message.type == USER_TOUCH_BASIC_TOUCH_CREATE_NEW_ORDER_BUTTON) {
            handleCreateNewOrder(core);
            // TODO
            return;
        }
        if (message.type == USER_TOUCH_PIN_PAD_ENTER_FUEL_EDITING_BUTTON) {
            std::string pin_buffer(message.payload.begin(), message.payload.end());
            handleEnterFuelPricePinPad(core, pin_buffer);
            return;
        }
        if (message.type == USER_TOUCH_PIN_PAD_ENTER_FUEL_VOLUME_BUTTON) {
            std::string pin_buffer(message.payload.begin(), message.payload.end());
            handleEnterFuelVolumeOrderPinPad(core, pin_buffer);
            return;
        }
        if (message.type == USER_TOUCH_SERVICE_MENU_LEVEL_GAUGE_BUTTON || message.type ==
            USER_TOUCH_SERVICE_MENU_RECEPTION_LEVEL_GAUGE_BUTTON) {
            handleLevelGaugeButton(message, core);
            return;
        }
        if (message.type == USER_TOUCH_SERVICE_MENU_CHANGE_FUEL_PRICE_BUTTON) {
            fillPageEditingFuelTypeSecondVariable(m_products, core);
            return;
        }
        if (message.type == USER_TOUCH_AMOUNT_TRK_BUTTON) {
            int8_t value = static_cast<int8_t>(message.payload[0]);
            handleAmountTRK(value);
            m_amount_trk = value;
            return;
        }
        if (message.type == USER_TOUCH_CHOOSE_FUEL_FOR_EDIT_BUTTON) {
            handleEditFuelButton(core);
            return;
        }
        if (message.type == USER_TOUCH_PAGINATION_FUEL_BUTTON) {
            int8_t direction = static_cast<int8_t>(message.payload[0]);
            changeFuelType(direction, core);
            return;
        }
        if (message.type == USER_TOUCH_PAGINATION_LEVEL_GAUGE_BUTTON || message.type ==
            USER_TOUCH_PAGINATION_RECEPTION_LEVEL_GAUGE_BUTTON) {
            int8_t direction = static_cast<int8_t>(message.payload[0]);
            changeLevelGauge(direction, core, message.type);
            return;
        }
        if (message.type == USER_TOUCH_RESET_SELECTED_TRK_BUTTON) {
            // Сброс выбора редактирования используемых ТРК.
            m_selected_dispensers.clear();
            renderTrkSelectionPage(core);
            return;
        }
        if (message.type == USER_TOUCH_ACCEPT_SELECTED_TRK_BUTTON) {
            // Подтверждение выбора используемых ТРК.
            handleAcceptSelectedTRK();
            return;
        }
        if (message.type == USER_TOUCH_SET_USED_TRK_BUTTON) {
            renderTrkSelectionPage(core);
            return;
        }
        if (message.type == USER_TOUCH_TAP_ON_USED_TRK_BUTTON) {
            if (!message.payload.empty()) {
                uint8_t slot_index = message.payload[0];
                handleTrkSelectionToggle(core, slot_index);
            }
            return;
        }
        if (message.type == USER_TOUCH_CLOSE_ORDER_BUTTON) {
            std::string command = m_settings.APIDispenser.close;
            PrimeCommands::handleCommand(core, command, m_current_dispenser_id);
            getDispenserStatus(core, m_current_dispenser_id);
            return;
        }
        if (message.type == PAY_TRANSACTION_RESPONSE) {
            handlePaymentResponse(message, core);
            return;
        }
        if (message.type == PAY_CARD_RESOLVED) {
            handleCardResolved(core);
            return;
        }
        if (message.type == PAY_CONFIRM_RESPONSE_SUCCESS) {
            handlePayConfirmOrder(core);
            return;
        }
        if (message.type == PAY_CANCEL_RESPONSE_SUCCESS) {
            // Возврат успешен — через 5с на стартовую страницу
            clearTimers();
            startPageTimer(m_settings.business_logic.waiting_cancel_success, getStartPage(), [this, &core]() {
                m_current_dispenser_id.clear();
                renderDispenser(core);
                clearInputTextDwin( core);
            });
            return;
        }
        if (message.type == USER_TOUCH_BASIC_TOUCH_CANCEL_TRANSACTION_BUTTON) {
            // fill returning money pages
            handleImmediatlyCancelTransaction(core);
            return;
        }
        if (message.type == USER_TOUCH_BASIC_TOUCH_ATTACH_CARD_BACK_BUTTON) {
            DwinCommands::sendPageToDwin(core, getStartPage());
            m_current_dispenser_id.clear();
            renderDispenser(core);
            return;
        }
        if (message.type == USER_TOUCH_PIN_PAD_ENTER_PIN_CODE_BUTTON) {
            handleEnterPinCodeButton(core, message);
            return;
        }
        if (message.type == USER_TOUCH_PIN_PAD_ENTER_DOCUMENT_NUMBER_BUTTON) {
            m_reception_document_number.assign(message.payload.begin(), message.payload.end());
            return;
        }
        if (message.type == USER_TOUCH_BASIC_TOUCH_DECLINE_ENTER_SERVICE_CODE_BUTTON) {
            DwinCommands::sendPageToDwin(core, getStartPage());
            return;
        }
        if (message.type == USER_TOUCH_ACCEPT_RECEPTION_FUEL_BUTTON) {
            // TODO вызов состояния выбранного резервуара для печати информации на чек
            processReceptionTanker(core);
            return;
        }
        if (message.type == USER_TOUCH_FINISH_RECEPTION_FUEL_BUTTON) {
            // TODO вызов состояния (по идее 1в1 что и accept, сбрасывать текущий резервуар нет смысла, он перезапишется)
        }
        if (message.type == USER_TOUCH_CHOOSE_RECEPTION_LEVEL_GAUGE_BUTTON) {
            if (!m_level_gauge.empty()) {
                const LevelGauge &level_gauge = m_level_gauge[m_current_level_gauge_index];
                const Tanker *tanker = utility::getTankerByLevelGaugeId(m_tankers, level_gauge.id);
                if (tanker != nullptr) {
                    m_current_tanker_id = tanker->id;
                }
            }
            return;
        }
        if (message.type == PAY_GET_BALANCE) {
            m_waiting_balance_response = false;
            Message message_balance;
            message_balance.source = PRIME_HTTP_LAYER;
            message_balance.type = ON_GET_BALANCE_RESPONSE;
            message_balance.payload = serializeCardInfo(m_parsed_card_balance);

            core.sendToLogicLayer(UART_LAYER, message_balance);
        }
        if (message.type == ON_GET_BALANCE_RESPONSE) {
            handleGetBalanceCard(core, message);
        }
        if (message.type == USER_TOUCH_CANCEL_TRANSACTION_AFTER_CANCEL_ENTER_PIN) {
            handleCancelEnterPin(core);
        }
    }

    void PrimeLogic::handleEnterPinCodeButton(MessageLayer &core, const Message &message) {
        std::string pin_code;
        pin_code.assign(message.payload.begin(), message.payload.end());
        if (pin_code.length() != 4) {
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_error_incorrect_pincode);
            startPageTimer(m_settings.business_logic.show_incorrect_pin_page, m_settings.dwin.page_print_pin);
        }
        if (!m_waiting_balance_response) {
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_processing_fuel_card);
        } else {
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_waiting_card_operation);
        }

        Message pin_msg;
        pin_msg.source = PRIME_HTTP_LAYER;
        pin_msg.type = PAY_PIN_ENTERED;
        pin_msg.payload = message.payload;
        core.sendToLogicLayer(PIPE_LAYER, pin_msg);
    }

    void PrimeLogic::handleCancelEnterPin(MessageLayer &core) {
        std::string command = m_settings.APIDispenser.close;
        PrimeCommands::handleCommand(core, command, m_current_dispenser_id);
    }

    void PrimeLogic::handleGetBalanceCard(MessageLayer &core, const Message &message) {
        std::string json_str(message.payload.begin(), message.payload.end());
        auto json = nlohmann::json::parse(json_str);

        bool success = json["success"];
        std::string json_message = json["message"];

        if (success) {
            m_parsed_card_balance = utility::parseTerminalBalanceReceipt(json_message);

            if (!m_current_dispenser_id.empty()) {
                Dispenser *dispenser = utility::getDispenserById(m_dispensers, m_current_dispenser_id);
                if (dispenser->status == DISPENSER_NOZZLE_UP) {
                    DwinCommands::sendPageToDwin(core, m_settings.dwin.page_set_fuel_volume);
                }
            }
        } else {
            if (json_message == NO_PINPAD) {
                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_print_pin);
            } else if (json_message == INCORECT_PINCODE) {
                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_error_incorrect_pincode);
                startPageTimer(m_settings.business_logic.show_incorrect_pin_page, m_settings.dwin.page_print_pin);
            }
        }
    }

    void PrimeLogic::handleCreateNewOrder(MessageLayer &core) {
        renderDispenser(core);
        DwinCommands::sendPageToDwin(core, getStartPage());
        m_current_dispenser_id.clear();
    }

    void PrimeLogic::handleImmediatlyCancelTransaction(MessageLayer &core) {
        // Печать чека возврата при отмене транзакции
        if (!m_current_dispenser_id.empty() && m_orders.count(m_current_dispenser_id) > 0) {
            auto &order = m_orders[m_current_dispenser_id];
            Product *product = utility::getProductById(m_products, order.product_id);

            if (product) {
                ReceiptData receipt;
                receipt.address = m_settings.gas_station.standalone_address;
                receipt.product_name = product->title;
                receipt.volume_liters = order.volume;
                receipt.ordered_volume_liters = order.order_volume;
                receipt.price_per_liter = order.order_price;
                receipt.is_refund = true;

                // Печать чека возврата.
                Message refund_msg;
                refund_msg.source = PRIME_HTTP_LAYER;
                refund_msg.type = PRINT_REFUND_RECEIPT;
                refund_msg.resource_id = order.id;
                refund_msg.payload = serializeReceiptData(receipt);
                //

                // Заполнение информации для отчета в BOS. Возможно нужно указать Fact значения объема
                auto it = m_pending_info_sales.find(order.id);
                if (it != m_pending_info_sales.end()) {
                    it->second.type_sale = std::to_string(TypeSale::Revert);
                    auto now = std::chrono::system_clock::now();
                    auto duration = now.time_since_epoch();
                    auto millis = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
                    it->second.date = utility::getTimeStringForBos(millis);
                    it->second.fact_amount = it->second.rqs_amount;
                    it->second.fact_quantity = it->second.rqs_quantity;
                }

                core.sendToLogicLayer(PIPE_LAYER, refund_msg);
                std::string command = m_settings.APIDispenser.close;
                PrimeCommands::handleCommand(core, command, m_current_dispenser_id);
            }
        }
    }

    void PrimeLogic::printDebetReceipt(MessageLayer &core) {
        if (!m_current_dispenser_id.empty() && m_orders.count(m_current_dispenser_id) > 0) {
            auto &order = m_orders[m_current_dispenser_id];

            Product *product = utility::getProductById(m_products, order.product_id);

            if (product) {
                ReceiptData receipt;
                receipt.address = m_settings.gas_station.standalone_address;
                receipt.product_name = product->title;
                receipt.volume_liters = order.order_volume;
                receipt.price_per_liter = order.order_price;
                receipt.is_refund = false;
                receipt.wallet_type = m_parsed_card_balance.wallet_type;

                Message msg;
                msg.source = PRIME_HTTP_LAYER;
                msg.type = PRINT_DEBIT_RECEIPT;
                msg.resource_id = order.id;
                msg.payload = serializeReceiptData(receipt);

                m_pending_info_sales[order.id].receipt = serializeReceiptDataToString(receipt);

                core.sendToLogicLayer(PIPE_LAYER, msg);
            }
        }
    }

    void PrimeLogic::handleFinishReceptionFuel(MessageLayer &core) {
        if (m_reception_active) {
            const LevelGauge *level_gauge = utility::getLevelGaugeById(m_level_gauge, m_reception_level_gauge_id);
            const Tanker *tanker = level_gauge != nullptr
                                       ? utility::getTankerByLevelGaugeId(m_tankers, level_gauge->id)
                                       : nullptr;

            if (level_gauge != nullptr && tanker != nullptr) {
                std::string product_name;
                if (Product *product = utility::getProductById(m_products, tanker->product_id)) {
                    product_name = !product->title.empty() ? product->title : product->id;
                }

                if (m_itp_logger) {
                    m_itp_logger->trace(1, "HttpLogic", "Finishing fuel reception for tanker: ", tanker->id,
                                        " level gauge: ", level_gauge->id, " document: ", m_reception_document_number);
                }

                LevelGauge initial_gauge(level_gauge->id);
                initial_gauge.upper_level = m_reception_initial_state.upper_level;
                initial_gauge.lower_level = m_reception_initial_state.lower_level;
                initial_gauge.upper_volume = m_reception_initial_state.upper_volume;
                initial_gauge.lower_volume = m_reception_initial_state.lower_volume;
                initial_gauge.total_volume = m_reception_initial_state.total_volume;
                initial_gauge.filling = m_reception_initial_state.filling;
                initial_gauge.density = m_reception_initial_state.density;
                initial_gauge.weight = m_reception_initial_state.weight;

                std::string receipt = utility::createSecondReceiptFromLevelGauge(
                    product_name,
                    *tanker,
                    *level_gauge,
                    initial_gauge,
                    m_reception_document_number,
                    m_reception_start_time,
                    utility::getCurrentTimeString()
                );

                Message print_message;
                print_message.source = PRIME_HTTP_LAYER;
                print_message.type = PAY_PRINT_RECEIPT;
                print_message.resource_id = tanker->id;
                print_message.payload.assign(receipt.begin(), receipt.end());
                core.sendToLogicLayer(PIPE_LAYER, print_message);
            }
        }
        m_reception_active = false;
        m_reception_level_gauge_id.clear();
        m_reception_document_number.clear();
    }

    void PrimeLogic::processReceptionTanker(MessageLayer &core) {
        if (m_reception_document_number.empty()) {
            std::cerr << "[HttpLogic] Document number is required before starting reception" << std::endl;
            return;
        }

        if (m_level_gauge.empty()) {
            std::cerr << "[HttpLogic] No level gauges available for receipt printing" << std::endl;
            return;
        }

        const LevelGauge &level_gauge = m_level_gauge[m_current_level_gauge_index];
        const Tanker *tanker = utility::getTankerByLevelGaugeId(m_tankers, level_gauge.id);
        if (tanker == nullptr) {
            std::cerr << "[HttpLogic] No tanker linked to level gauge " << level_gauge.id << std::endl;
            return;
        }

        m_current_tanker_id = tanker->id;

        std::string product_name;
        if (Product *product = utility::getProductById(m_products, tanker->product_id)) {
            product_name = !product->title.empty() ? product->title : product->id;
        }

        // Сохраняем начальное состояние уровнемера и время начала приемки
        m_reception_initial_state.upper_level = level_gauge.upper_level;
        m_reception_initial_state.lower_level = level_gauge.lower_level;
        m_reception_initial_state.upper_volume = level_gauge.upper_volume;
        m_reception_initial_state.lower_volume = level_gauge.lower_volume;
        m_reception_initial_state.total_volume = level_gauge.total_volume;
        m_reception_initial_state.filling = level_gauge.filling;
        m_reception_initial_state.density = level_gauge.density;
        m_reception_initial_state.weight = level_gauge.weight;
        m_reception_start_time = utility::getCurrentTimeString();

        std::string receipt = utility::createFirstReceiptFromLevelGauge(
            level_gauge,
            *tanker,
            product_name,
            m_reception_start_time
        );

        Message print_message;
        print_message.source = PRIME_HTTP_LAYER;
        print_message.type = PAY_PRINT_RECEIPT;
        print_message.resource_id = tanker->id;
        print_message.payload.assign(receipt.begin(), receipt.end());

        core.sendToLogicLayer(PIPE_LAYER, print_message);
        m_reception_active = true;
        m_reception_level_gauge_id = level_gauge.id;
        getParameters(core, level_gauge.id);
        setLevelGaugeParametersOnDisplay(core, level_gauge, USER_TOUCH_ACCEPT_RECEPTION_FUEL_BUTTON);
    }

    void PrimeLogic::handlePayConfirmOrder(MessageLayer &core) {
        if (m_orders.count(m_current_dispenser_id) &&
            m_orders[m_current_dispenser_id].status == ORDER_INTERRUPTED) {
            // Частичный пролив — "Возврат средств завершён".

            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_return_money_process_end);
            clearTimers();
            startPageTimer(m_settings.business_logic.show_return_money_process_end, getStartPage(), [this, &core]() {
                m_current_dispenser_id.clear();
                renderDispenser(core);
                clearInputTextDwin( core);
            });
        } else {
            // Полный пролив — "Счастливого пути".
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_fuel_ended);
            DwinCommands::sendPlaySoundToDwin(core, m_settings.dwin.audio_id_fuelling_end);
        }
    }

    void PrimeLogic::handleCardResolved(MessageLayer &core) {
        // Запрос баланса карты
        m_waiting_balance_response = true;
        Message msg;
        msg.source = PRIME_HTTP_LAYER;
        msg.type = PAY_GET_BALANCE;

        core.sendToLogicLayer(PIPE_LAYER, msg);

        DwinCommands::sendPageToDwin(core, m_settings.dwin.page_waiting_card_operation);
        //
    }

    void PrimeLogic::handleBasicTouch(MessageLayer &core) {
        getDispenserStatus(core, m_current_dispenser_id);
        m_waiting_balance_response = false;
    }

    void PrimeLogic::handleAcceptSelectedTRK() {
        if (m_selected_dispensers.empty()) {
            std::cout << "[Logic] Warning: Attempted to save empty TRK list. Ignored." << std::endl;
            return;
        }

        utility::saveUsedTRKsToConfig(CONFIG, m_selected_dispensers);

        m_amount_trk = m_selected_dispensers.size();

        m_current_dispenser_index = 0;
        m_current_dispenser_id.clear();
    }

    void PrimeLogic::handleTrkSelectionToggle(MessageLayer &core, uint8_t slot_index) {
        if (slot_index >= m_dispensers.size()) {
            return;
        }
        std::string tapped_trk_id = m_dispensers[slot_index].id;
        auto it = std::find(m_selected_dispensers.begin(), m_selected_dispensers.end(), tapped_trk_id);

        if (it != m_selected_dispensers.end()) {
            m_selected_dispensers.erase(it);
        } else {
            m_selected_dispensers.push_back(tapped_trk_id);
        }
        renderTrkSelectionPage(core);
    }

    void PrimeLogic::renderTrkSelectionPage(MessageLayer &core) {
        // Проходим по всем 8 возможным слотам на экране DWIN
        for (size_t i = 0; i < 8; ++i) {
            std::string text_to_show = "";
            std::string prefix_to_show = "";
            if (i < m_dispensers.size()) {
                std::string trk_id = m_dispensers[i].id;
                text_to_show = trk_id;

                auto it = std::find(m_selected_dispensers.begin(), m_selected_dispensers.end(), trk_id);

                if (it != m_selected_dispensers.end()) {
                    int order_number = std::distance(m_selected_dispensers.begin(), it) + 1;
                    prefix_to_show = std::to_string(order_number) + ".";
                } else {
                    prefix_to_show = ".";
                }
            }
            DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_values_selected_trk[i], text_to_show,
                                                        m_settings.dwin.text_len_order_integer);
            DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_nums_selected_trk[i], prefix_to_show,
                                                        m_settings.dwin.text_len_order_decimal);
        }
    }

    void PrimeLogic::handleEnterFuelPricePinPad(MessageLayer &core, std::string buffer) {
        if (!buffer.empty()) {
            int value, exponent;
            std::tie(value, exponent) = utility::formatFloatStringToInt(buffer);

            // Логика сохранения адекватной стоимости за литр. Если целое значение больше 3 (больше на дисплей не влезет), не сохраняем
            int digit_count = (value == 0) ? 1 : (int) log10(abs(value)) + 1 - exponent;

            if (digit_count < m_settings.dwin.text_len_fuel_integer) {
                utility::saveFuelPriceToConfig(CONFIG, m_current_fuel_id_editing, buffer);
                for (auto &p: m_products) {
                    if (p.id == m_current_fuel_id_editing) {
                        p.price = buffer;
                        sendFloatToDwin(m_settings.dwin.vp_fuel_price_for_editing_integer,
                                        m_settings.dwin.vp_fuel_price_for_editing_decimal,
                                        core, m_settings.dwin.text_len_fuel_integer,
                                        m_settings.dwin.text_len_order_decimal,
                                        p.price);
                        break;
                    }
                }
            }
        }
    }

    void PrimeLogic::handleEnterFuelVolumeOrderPinPad(MessageLayer &core, std::string buffer) {
        if (!buffer.empty()) {
            int value, exponent;
            std::tie(value, exponent) = utility::formatFloatStringToInt(buffer);
            handleCreateOrder(core, value, exponent);
        }
    }

    void PrimeLogic::handleChooseTRK(MessageLayer &core, int index) {
        bool fuel_price_set = utility::checkAllFuelPrice(m_products);
        if (fuel_price_set) {
            m_current_dispenser_id = m_selected_dispensers[index];
            Dispenser *dispenser = utility::getDispenserById(m_dispensers, m_current_dispenser_id);
            if (dispenser->status == DISPENSER_IDLE) {
                processDispenserIdleAfterUserTouch(core);
            } else if (dispenser->status == DISPENSER_NOZZLE_UP) {
                Product *product = utility::getProductById(m_products, dispenser->product_id);
                processDispenserNozzleUpAfterUserTouch(core, product);
                //DwinCommands::sendPageToDwin(core, m_settings.dwin.page_put_card_or_scan_code);
            }
            // TODO реализовать создание еще одного заказа.
            else if (dispenser->status == DISPENSER_FUELLING) {
                // Выставить текущий продукт.
                Product *product = utility::getProductById(m_products, dispenser->product_id);
                int icon_fuel = utility::getIconForProduct(product->rating, m_settings);
                setProductIdOnDisplay(core, icon_fuel);
                setFuelTypePriceOnDisplay(core, product->price);
                setProductTextOnDisplay(core, utility::extractFirstInt(product->id));
                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_fuel_in_progress);

                //TODO ОТКРЫТЬ ЭКРАН НАЛИВА (11) page_fuel_in_progress
            } else if (dispenser->status == DISPENSER_COMPLETE) {
                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_fuel_ended);
                //TODO ОТКРЫТЬ ЗАПРАВКА ЗАВЕРШЕНА (12) page_fuel_ended
                Product *product = utility::getProductById(m_products, dispenser->product_id);
                int icon_fuel = utility::getIconForProduct(product->rating, m_settings);
                setProductIdOnDisplay(core, icon_fuel);
                setFuelTypePriceOnDisplay(core, product->price);
                setProductTextOnDisplay(core, utility::extractFirstInt(product->id));
                DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_progress_order_bar_twelvth_page, 100);
                DwinCommands::sendRightAlignmentWithPadding(
                    core, m_settings.dwin.vp_progress_bar_percent_text_twelvth_page, "100",
                    m_settings.dwin.text_len_percent_progress_bar);
            }

            setTRKIdOnDisplay(core, utility::extractFirstInt(m_current_dispenser_id));
        } else {
            fillPageEditingFuelTypeSecondVariable(m_products, core);
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_set_fuel_price_another_variable);
        }
    }

    void PrimeLogic::handleCreateOrder(MessageLayer &core, uint16_t volume, int exponent) {
        const Dispenser *dispenser = utility::getDispenserById(m_dispensers, m_current_dispenser_id);
        const Product *product = utility::getProductById(m_products, dispenser->product_id);

        int value_price, exponent_price;
        std::tie(value_price, exponent_price) = utility::formatFloatStringToInt(product->price);

        createOrder(core, volume, exponent, value_price, exponent_price);
    }

    void PrimeLogic::createPayment(MessageLayer &core,
                                   const std::string &order_id,
                                   const std::string &product_id,
                                   uint32_t int_price_per_fuel,
                                   uint8_t dec_price_per_fuel,
                                   uint32_t int_value_fuel,
                                   uint8_t dec_value_fuel,
                                   uint32_t int_order_price,
                                   uint8_t dec_order_price) {
        if (order_id.empty()) {
            return;
        }

        if (m_payment_requested_order_ids.find(order_id) != m_payment_requested_order_ids.end()) {
            return;
        }

        PaymentRequestData payment;
        payment.product_id = product_id;
        payment.price_value = int_price_per_fuel;
        payment.price_decimal = dec_price_per_fuel;
        payment.volume_value = int_value_fuel;
        payment.volume_decimal = dec_value_fuel;
        payment.amount_value = int_order_price;
        payment.amount_decimal = dec_order_price;

        Message message;
        message.source = PRIME_HTTP_LAYER;
        message.type = PAY_TRANSACTION;
        message.resource_id = order_id;
        message.payload = serializePaymentRequest(payment);

        core.sendToLogicLayer(PIPE_LAYER, message);
        m_payment_requested_order_ids.insert(order_id);
    }

    void PrimeLogic::processHandleHttpError(const Message &message, MessageLayer &core) {
        ErrorEvent err;
        err.code = message.status_code;
        err.context = message.url;
        err.description = std::string(message.payload.begin(), message.payload.end());

        if (message.status_code == POCO_ERROR) {
            err.source = ErrorSource::NETWORK;
            err.severity = ErrorSeverity::FATAL;
        } else if (message.status_code >= 500) {
            err.source = ErrorSource::HTTP;
            err.severity = ErrorSeverity::CRITICAL;
        } else if (message.status_code == 401 || message.status_code == 403) {
            err.source = ErrorSource::HTTP;
            err.severity = ErrorSeverity::CRITICAL;
        } else {
            err.source = ErrorSource::HTTP;
            err.severity = ErrorSeverity::WARNING;
        }

        dispatchError(err, core);
    }

    void PrimeLogic::processHandleHttp(const Message &message, MessageLayer &core) {
        if (message.status_code >= 400) {
            processHandleHttpError(message, core);
        } else {
            std::string jsonStr(message.payload.begin(), message.payload.end());
            if (jsonStr != "" && !jsonStr.empty()) {
                auto json = json::parse(jsonStr);
                if (json.is_object()) {
                    processVariableHttp(json, core, message.url);
                }
            }
        }
    }

    void PrimeLogic::dispatchError(const ErrorEvent &error, MessageLayer &core) {
        if (error.source == ErrorSource::NETWORK) {
            // core.sendPageChange(m_settings.dwin.page_no_connection);
            return;
        }

        if (error.code == 401 || error.code == 403) {
            return;
        }

        if (error.code == 400 && error.description.find(LOW_VOLUME) != std::string::npos) {
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_error_low_fuel_level);

            // Закрытие заказа на Prime
            std::string command = m_settings.APIDispenser.close;
            PrimeCommands::handleCommand(core, command, m_current_dispenser_id);

            // Отправка PAY_CANCEL в ArkaimLogic (пустой payload — возьмёт из m_pending)
            Message cancel_msg;
            cancel_msg.source = PRIME_HTTP_LAYER;
            cancel_msg.type = USER_TOUCH_BASIC_TOUCH_CANCEL_TRANSACTION_BUTTON;
            core.sendToLogicLayer(PIPE_LAYER, cancel_msg);
        }

        if (error.context.find("/order") != std::string::npos) {
            if (error.severity == ErrorSeverity::CRITICAL) {
                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_error_creating_order);
            }
        }
    }

    void PrimeLogic::handleLevelGaugeButton(const Message &message, MessageLayer &core) {
        const LevelGauge *level_gauge = utility::getLevelGaugeById(m_level_gauge,
                                                                   m_level_gauge[m_current_level_gauge_index].id);
        setLevelGaugeParametersOnDisplay(core, *level_gauge, message.type);
    }

    void PrimeLogic::handleEditFuelButton(MessageLayer &core) {
        const Product *product = utility::getProductById(m_products, m_products[m_current_fuel_type_index].id);
        m_current_fuel_id_editing = product->id;
        int icon_fuel = utility::getIconForProduct(product->rating, m_settings);
        DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_editing_fuel_type, icon_fuel);

        if (product->price != "0") {
            sendFloatToDwin(m_settings.dwin.vp_editing_fuel_integer_part, m_settings.dwin.vp_editing_fuel_decimal_part,
                            core, m_settings.dwin.text_len_fuel_integer, m_settings.dwin.text_len_order_decimal,
                            product->price);
        }
    }

    void PrimeLogic::checkTimers(MessageLayer &core) {
        auto now = std::chrono::steady_clock::now();

        for (auto it = m_page_timers.begin(); it != m_page_timers.end();) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->start).count();
            if (elapsed >= it->timeout_seconds) {
                DwinCommands::sendPageToDwin(core, it->page_id);
                if (it->on_expire) {
                    it->on_expire();
                }
                it = m_page_timers.erase(it);
            } else {
                ++it;
            }
        }
    }

    void PrimeLogic::startPageTimer(int timeout_seconds, uint16_t page_id, std::function<void()> on_expire) {
        PageTimer timer;
        timer.start = std::chrono::steady_clock::now();
        timer.timeout_seconds = timeout_seconds;
        timer.page_id = page_id;
        timer.on_expire = std::move(on_expire);
        m_page_timers.push_back(std::move(timer));
    }

    void PrimeLogic::clearTimers() {
        m_page_timers.clear();
    }

    uint16_t PrimeLogic::getStartPage() const {
        switch (m_amount_trk) {
            case 1: return m_settings.dwin.page_choose_trk_config_uno_trk;
            case 2: return m_settings.dwin.page_choose_trk_config_double_trk;
            default: return m_settings.dwin.page_choose_trk;
        }
    }

    void PrimeLogic::handleAmountTRK(int amount) {
        utility::saveTRKAmountToConfig(CONFIG, amount);
        m_amount_trk_png = utility::getIconForGasStation(m_amount_trk, m_settings);
        m_current_dispenser_index = 0;
        m_current_dispenser_id.clear();
    }

    void PrimeLogic::handleFuelTypeEditing(const Message &message, MessageLayer &core) {
        uint16_t value = (message.payload[2] << 8) | message.payload[3];
        int row_number = static_cast<int>(value);
        int index = row_number - 1;

        if (index >= 0 && index < m_edit_page_row_to_fuel_id.size()) {
            std::string fuel_to_edit = m_edit_page_row_to_fuel_id[index];
            m_current_fuel_id_editing = fuel_to_edit;

            const Product *product = utility::getProductById(m_products, fuel_to_edit);
            int icon_fuel = utility::getIconForProduct(product->rating, m_settings);
            DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_editing_fuel_type, icon_fuel);

            if (product->price != "0") {
                sendFloatToDwin(m_settings.dwin.vp_editing_fuel_integer_part,
                                m_settings.dwin.vp_editing_fuel_decimal_part,
                                core, m_settings.dwin.text_len_fuel_integer, m_settings.dwin.text_len_order_decimal,
                                product->price);
            }

            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_set_fuel_price);
        } else {
            std::cerr << "[Warning] Clicked empty row: " << row_number << std::endl;
        }
    }

    void PrimeLogic::createOrder(MessageLayer &core, int value, int exponent_volume, int price,
                                 int price_exponent) const {
        json json_object;

        json_object["price"] = {
            {"value", price},
            {"exponent", price_exponent}
        };

        json_object["type"] = FUEL_VOLUME_ORDER_TYPE;

        json_object["value"] = {
            {"value", value},
            {"exponent", exponent_volume}
        };

        std::string json_string = json_object.dump();

        if (m_itp_logger) {
            m_itp_logger->trace(1, "HttpLogic", "Creating order for dispenser: ", m_current_dispenser_id,
                                " volume: ", value, " price: ", price);
        }

        Message request;
        request.resource_id = m_current_dispenser_id;
        request.type = CREATE_ORDER;
        request.payload.assign(json_string.begin(), json_string.end());

        core.sendTo(PRIME_HTTP_LAYER, request);
    }

    void PrimeLogic::handlePaginationDispenser(int direction, MessageLayer &core) {
        if (m_amount_trk < 3) return;

        if (m_selected_dispensers.empty()) {
            std::cout << "No TRK in map " << std::endl;
            return;
        }
        m_current_dispenser_index = utility::getPaginationIndex(m_current_dispenser_index, m_selected_dispensers.size(),
                                                                direction);

        std::string trk_id = m_selected_dispensers[m_current_dispenser_index];
        getDispenserStatus(core, trk_id);

        renderDispenser(core);
    }

    void PrimeLogic::changeLevelGauge(int direction, MessageLayer &core, const std::string &type) {
        if (m_level_gauge.empty()) {
            std::cout << "No Level Gauge in map" << std::endl;
            return;
        }
        m_current_level_gauge_index = utility::getPaginationIndex(m_current_level_gauge_index, m_level_gauge.size(),
                                                                  direction);
        const auto &level_gauge = m_level_gauge[m_current_level_gauge_index];
        getParameters(core, level_gauge.id);
        setLevelGaugeParametersOnDisplay(core, level_gauge, type);
    }

    void PrimeLogic::changeFuelType(int direction, MessageLayer &core) {
        if (m_products.empty()) {
            std::cout << "No Products in map " << std::endl;
            return;
        }
        m_current_fuel_type_index =
                utility::getPaginationIndex(m_current_fuel_type_index, m_products.size(), direction);

        Product &p = m_products[m_current_fuel_type_index];
        int icon_fuel = utility::getIconForProduct(p.rating, m_settings);
        DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_icon_fuel_type_for_editing, icon_fuel);
        sendFloatToDwin(m_settings.dwin.vp_fuel_price_for_editing_integer,
                        m_settings.dwin.vp_fuel_price_for_editing_decimal,
                        core, m_settings.dwin.text_len_fuel_integer, m_settings.dwin.text_len_order_decimal, p.price);
    }

    void PrimeLogic::processVariableHttp(const json &jArray, MessageLayer &core, const std::string &url) {
        if (jArray.contains("events")) {
            processEvent(jArray, core);
        } else if (jArray.contains("parameters")) {
            processParameters(jArray, url, core);
            if (m_reception_active && !m_reception_level_gauge_id.empty()) {
                LevelGauge *level_gauge = utility::getLevelGaugeById(m_level_gauge, m_reception_level_gauge_id);
                if (level_gauge != nullptr) {
                    setLevelGaugeParametersOnDisplay(core, *level_gauge, USER_TOUCH_ACCEPT_RECEPTION_FUEL_BUTTON);
                }
            }
        } else if (jArray.contains("products")) {
            processProducts(jArray);
        } else if (jArray.contains("status")) {
            processDispenserStatus(jArray, core, url);
        } else if (jArray.contains("task_id")) {
            processTaskId(jArray, core, url);
        } else if (jArray.contains("task")) {
            processTask(jArray, core);
        } else if (jArray.contains("devices")) {
            processDevices(jArray, core);
        } else if (jArray.contains("tankers")) {
            processTankers(jArray);
        }
    }

    void PrimeLogic::processEvent(const json &jArray, MessageLayer &core) {
        // Создано для ловли последнего пакета с данными о заказе, т.к. сначала заказ закрывается, а потом уже последняя информация.
        std::set<std::string> completed_dispensers;

        for (const auto &event: jArray["events"]) {
            if (event.contains("id")) {
                long long currentId = event["id"];
                if (currentId) {
                    m_lastId = currentId;
                }
            }
            // Проверка, чтобы при изменении статуса на ТРК, которую на дисплее не трогают, дисплей не изменял состояния.
            std::string trk_id = "";
            if (event.contains("device_id")) {
                trk_id = utility::parseStringFromJson(event["device_id"]);
            }

            // Методы, если ТРК выбрана (не 1 страница).
            if (trk_id == m_current_dispenser_id) {
                if (event.contains("on_dsp_status_changed")) {
                    handleDispenserStatus(event, core);
                } else if (event.contains("on_dsp_display_changed")) {
                    handleDispenserDisplay(event, core);
                } else if (event.contains("on_dsp_order_created")) {
                    // Запрос возможность начала пролива. Пока нет проверки оплаты обычный флаг, пока он false отображать одно состояние
                    // изменился - другое. Начало пролива после нажатия на дисплей.
                    handleDispenserOrderCreated(event, core);
                } else if (event.contains("on_dsp_order_changed")) {
                    handleDispenserOrderStatus(event, core, completed_dispensers);
                }
            } else {
                if (event.contains("on_dsp_status_changed")) {
                    handleRandomDispenserStatus(event, core);
                } else if (event.contains("on_dsp_order_changed")) {
                    // TODO handle random dispenser order changed
                    handleRandomDispenserOrderStatus(event);
                }
            }
        }
        for (const auto &device_id: completed_dispensers) {
            processOrderFinalization(device_id, core);
        }
    }

    void PrimeLogic::handleRandomDispenserStatus(const json &event, MessageLayer &core) {
        try {
            auto data = event["on_dsp_status_changed"];
            std::string status = data.value("status", "");
            std::string dispenser_id = utility::parseStringFromJson(event["device_id"]);

            Dispenser *dispenser = utility::getDispenserById(m_dispensers, dispenser_id);
            dispenser->prev_status = dispenser->status;
            dispenser->status = status;

            if (data.contains("nozzle")) {
                auto nozzle = data["nozzle"];
                std::string nozzle_string = utility::parseStringFromJson(nozzle["product_id"]);
                dispenser->product_id = nozzle_string;
            }

            if (m_selected_dispensers[m_current_dispenser_index] == dispenser_id || m_amount_trk < 3) {
                renderDispenser(core);
            }

            if (status == DISPENSER_NOZZLE_UP && m_current_dispenser_id.empty()) {
                DwinCommands::sendPlaySoundToDwin(core, m_settings.dwin.audio_id_welcome_nozzle_up);
            }
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Random Dispenser Status: " << e.what() << std::endl;
        }
    }

    void PrimeLogic::handleDispenserOrderCreated(const json &event, MessageLayer &core) {
        auto data = event["on_dsp_order_created"];
        std::string order_id = utility::parseStringFromJson(data["order_id"]);
        std::string dispenser_id = utility::parseStringFromJson(event["device_id"]);

        DatabaseOrder database_order(order_id);
        database_order.dispenser_id = dispenser_id;

        m_is_server_ready_for_start_fuel = true; // флаг на начало пролива только после нажатия на дисплей
        authorizeOnServer(core);
        m_orders[dispenser_id] = database_order;

        getDispenserStatus(core, dispenser_id);
    }

    void PrimeLogic::handleDispenserDisplay(const json &event, MessageLayer &core) {
        try {
            std::string trk_id = event["device_id"];
            auto data = event["on_dsp_display_changed"];

            auto amount = data["amount"];
            std::string amount_string = utility::getFormattedStringFromJson(
                utility::parseStringFromJson(amount["value"]), utility::parseIntFromJson(amount["exponent"]));

            auto volume = data["volume"];
            std::string volume_string = utility::getFormattedStringFromJson(
                utility::parseStringFromJson(volume["value"]), utility::parseIntFromJson(volume["exponent"]));

            if (m_orders.count(trk_id)) {
                if (m_orders[trk_id].amount < std::stod(amount_string)) {
                    m_orders[trk_id].amount = std::stod(amount_string);
                    m_orders[trk_id].volume = std::stod(volume_string);
                    m_orders[trk_id].fact_amount_raw = static_cast<uint32_t>(std::stoul(
                        utility::parseStringFromJson(amount["value"])));
                    m_orders[trk_id].fact_amount_exp = static_cast<uint8_t>(utility::parseIntFromJson(
                        amount["exponent"]));
                    m_orders[trk_id].fact_volume_raw = static_cast<uint32_t>(std::stoul(
                        utility::parseStringFromJson(volume["value"])));
                    m_orders[trk_id].fact_volume_exp = static_cast<uint8_t>(utility::parseIntFromJson(
                        volume["exponent"]));
                }
            }

            // Проверка, что налив продолжается.
            Dispenser *dispenser = utility::getDispenserById(m_dispensers, trk_id);
            dispenser->m_last_volume_change_time = std::chrono::steady_clock::now();
            dispenser->is_fuelling_paused = false;

            // Отображение текущего состояние налива. Проверка, чтобы отображать налив выбранной трк
            Message msg_order_fuelling_volume;
            msg_order_fuelling_volume.type = UPDATE_CURRENT_FUELLING_VOLUME;
            msg_order_fuelling_volume.payload.assign(volume_string.begin(), volume_string.end());

            core.sendToLogicLayer(UART_LAYER, msg_order_fuelling_volume);

            Message msg_order_fuelling_amount;
            msg_order_fuelling_amount.type = UPDATE_CURRENT_FUELLING_AMOUNT;
            msg_order_fuelling_amount.payload.assign(amount_string.begin(), amount_string.end());

            core.sendToLogicLayer(UART_LAYER, msg_order_fuelling_amount);
            // sendFloatToDwin(m_settings.dwin.vp_current_order_amount_integer,
            //                 m_settings.dwin.vp_current_order_amount_decimal,
            //                 core, m_settings.dwin.text_len_order_integer, m_settings.dwin.text_len_order_decimal,
            //                 amount_string);

            // Отображение тулбара прогресса налива топлива.
            double current_volume_order = 0.0;

            auto it = m_orders.find(m_current_dispenser_id);
            if (it != m_orders.end()) {
                DatabaseOrder &order = it->second;
                current_volume_order = order.order_volume;
            }

            int percent = 0;
            std::string percent_str = "0";

            if (m_orders.count(trk_id) && current_volume_order > 0.0) {
                auto &order = m_orders[trk_id];
                double current = order.fact_volume_raw / std::pow(10.0, order.fact_volume_exp);

                if (current >= current_volume_order) {
                    percent = 100;
                } else {
                    double exact_percent = (current / current_volume_order) * 100.0;
                    percent = static_cast<int>(std::round(exact_percent));
                    if (percent >= 100) {
                        percent = 99;
                    }
                }

                percent_str = std::to_string(percent);
                std::cout << "Percent to bar: " << percent << std::endl;
            }

            DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_progress_order_bar_eleventh_page, percent);
            DwinCommands::sendRightAlignmentWithPadding(
                core, m_settings.dwin.vp_progress_bar_percent_text_eleventh_page, percent_str,
                m_settings.dwin.text_len_percent_progress_bar);
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Dispenser Display: " << e.what() << std::endl;
        }
    }

    void PrimeLogic::handleDispenserStatus(const json &event, MessageLayer &core) {
        auto data = event["on_dsp_status_changed"];
        auto status = data.value("status", "");

        auto device_id = utility::parseStringFromJson(event["device_id"]);

        Dispenser *dispenser = utility::getDispenserById(m_dispensers, device_id);
        dispenser->prev_status = dispenser->status;
        dispenser->status = status;

        if (data.contains("nozzle")) {
            auto nozzle = data["nozzle"];
            std::string nozzle_string = utility::parseStringFromJson(nozzle["product_id"]);
            dispenser->product_id = nozzle_string;
        }

        // Обработка налива топлива.
        if (status == DISPENSER_NOZZLE_UP) {
            // Показываем страницу "Вставьте/приложите карту".
            Product *product = utility::getProductById(m_products, dispenser->product_id);

            processDispenserNozzleUpAfterUserTouch(core, product);
            //DwinCommands::sendPageToDwin(core, m_settings.dwin.page_put_card_or_scan_code);
        } else if (status == DISPENSER_IDLE) {
            clearTimers();
            if (dispenser->prev_status == DISPENSER_FUELLING || dispenser->prev_status == DISPENSER_COMPLETE) {
                std::string command = m_settings.APIDispenser.close;
                PrimeCommands::handleCommand(core, command, m_current_dispenser_id);

                // Подготовка данных для заполнения информации уровнемерами.
                std::string level_gauge_id = utility::getLevelGaugeIdByProductId(m_tankers, dispenser->product_id);
                m_pending_level_gauge_orders[level_gauge_id].push({
                    dispenser->order.order_id, dispenser->id, GaugeTaskType::AFTER_FUELLING
                });
                getParameters(core, level_gauge_id);
            }
            if (dispenser->prev_status == DISPENSER_COMPLETE ||
                //dispenser->prev_status == DISPENSER_FUELLING ||
                dispenser->prev_status == DISPENSER_HALTED) {
                // После пролива — страницу покажет ORD_DELIVERED/ORD_INTERRUPTED
                // Здесь только таймер на возврат к стартовой
                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_good_trip);
                m_current_dispenser_id.clear();
                startPageTimer(5, getStartPage(), [&core, this] {
                    clearInputTextDwin( core);
                });
            } else if (dispenser->prev_status != DISPENSER_FUELLING) {
                // Первичный IDLE (пистолет не вставлен) — "Вставьте пистолет"
                processDispenserIdleAfterUserTouch(core);
            }
        } else if (status == DISPENSER_COMPLETE) {
            getDispenserStatus(core, m_current_dispenser_id);
            if (device_id == m_current_dispenser_id) {
                DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_progress_order_bar_twelvth_page, 100);
                DwinCommands::sendRightAlignmentWithPadding(
                    core, m_settings.dwin.vp_progress_bar_percent_text_twelvth_page, "100",
                    m_settings.dwin.text_len_percent_progress_bar);
            }
        } else if (status == DISPENSER_FUELLING) {
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_fuel_in_progress);
            dispenser->m_last_volume_change_time = std::chrono::steady_clock::now();
            dispenser->is_fuelling_paused = false;
            m_last_fueling_sound = std::chrono::steady_clock::now();
            renderDispenser(core);
        }
    }

    void PrimeLogic::handleDispenserOrderStatus(const json &jArray, MessageLayer &core,
                                                std::set<std::string> &completed_dispensers) {
        try {
            std::string status = "";
            std::string device_id = jArray["device_id"];
            auto data = jArray["on_dsp_order_changed"];
            auto &order = m_orders[device_id];

            auto &product_id = m_orders[device_id].product_id; // Для извлечения level_gauge.
            std::string level_gauge_id = utility::getLevelGaugeIdByProductId(m_tankers, product_id);
            std::string order_id = order.id;

            //auto it = m_pending_info_sales.find(order_id);
            if (data.contains("status")) {
                status = utility::parseStringFromJson(data["status"]);
                order.status = status;
            }

            if (data.contains("amount")) {
                auto amount = data["amount"];
                order.fact_amount_raw = static_cast<uint32_t>(
                    std::stoul(utility::parseStringFromJson(amount["value"])));
                order.fact_amount_exp = static_cast<uint8_t>(utility::parseIntFromJson(amount["exponent"]));
            }

            if (data.contains("volume")) {
                auto volume = data["volume"];
                order.fact_volume_raw = static_cast<uint32_t>(
                    std::stoul(utility::parseStringFromJson(volume["value"])));
                order.fact_volume_exp = static_cast<uint8_t>(utility::parseIntFromJson(volume["exponent"]));
            }

            if (status == ORDER_INTERRUPTED || status == ORDER_DELIVERED) {
                completed_dispensers.insert(device_id);
                std::string time_end_string = data["time_end"];
                order.time_end = std::stoull(time_end_string);
                getDispenserStatus(core, device_id);

            } else if (status == ORDER_AUTHORIZED) {
                std::string time_begin_string = data["time_begin"];
                auto tanker_begin = data["tanker_begin"];
                std::string tanker_begin_string = utility::getFormattedStringFromJson(
                    utility::parseStringFromJson(tanker_begin["value"]),
                    utility::parseIntFromJson(tanker_begin["exponent"]));
                auto pending_sale = m_pending_info_sales.find(order_id);
                pending_sale->second.dispenser_volume_begin = tanker_begin_string;

                uint64_t timestamp_date = std::stoull(time_begin_string);
                m_orders[device_id].time_begin = timestamp_date;
                m_pending_info_sales[order_id].date = utility::getTimeStringForBos(timestamp_date);
                m_pending_level_gauge_orders[level_gauge_id].push({
                    order_id, device_id, GaugeTaskType::BEFORE_FUELLING
                });
                getParameters(core, level_gauge_id);
            }
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error Dispenser Order Status: " << e.what() << std::endl;
        }
    }

    void PrimeLogic::processOrderFinalization(const std::string &device_id, MessageLayer &core) {
        auto &order = m_orders[device_id];
        std::string status = order.status;
        std::string order_id = order.id;
        std::string level_gauge_id = utility::getLevelGaugeIdByProductId(m_tankers, order.product_id);
        auto it = m_pending_info_sales.find(order_id);

        uint32_t fact_amount_value = order.fact_amount_raw;
        uint8_t fact_amount_decimal = order.fact_amount_exp;
        uint32_t fact_volume_value = order.fact_volume_raw;
        uint8_t fact_volume_decimal = order.fact_volume_exp;

        int price_val = 0, price_exp = 0;
        Product *product = utility::getProductById(m_products, order.product_id);
        if (product) {
            std::tie(price_val, price_exp) = utility::formatFloatStringToInt(product->price);
        }
        int order_volume_val = 0, order_volume_exp = 0;
        int order_amount_val = 0, order_amount_exp = 0;
        auto format_order_decimal = [](double value) {
            std::ostringstream stream;
            stream << std::fixed << std::setprecision(2) << value;
            std::string formatted = stream.str();
            std::replace(formatted.begin(), formatted.end(), '.', ',');
            return formatted;
        };
        std::tie(order_volume_val, order_volume_exp) = utility::formatFloatStringToInt(
            format_order_decimal(order.order_volume));
        std::tie(order_amount_val, order_amount_exp) = utility::formatFloatStringToInt(
            format_order_decimal(order.order_amount));

        std::string product_id_processing = utility::primeIdToProcessingId(
            order.product_id, m_settings.gas_station.prime_standalone,
            m_settings.gas_station.processing_standalone);
        PaymentRequestData payment;
        payment.product_id = product_id_processing;
        payment.price_value = static_cast<uint32_t>(price_val);
        payment.price_decimal = static_cast<uint8_t>(price_exp);
        payment.volume_value = static_cast<uint32_t>(order_volume_val);
        payment.volume_decimal = static_cast<uint8_t>(order_volume_exp);
        payment.amount_value = static_cast<uint32_t>(order_amount_val);
        payment.amount_decimal = static_cast<uint8_t>(order_amount_exp);
        payment.not_complete = (status == ORDER_INTERRUPTED) ? 1 : 0;
        payment.fact_volume_value = fact_volume_value;
        payment.fact_volume_decimal = fact_volume_decimal;
        payment.fact_amount_value = fact_amount_value;
        payment.fact_amount_decimal = fact_amount_decimal;
        Message msg;
        msg.source = PRIME_HTTP_LAYER;
        msg.type = PAY_CONFIRM;
        msg.resource_id = order.id;
        msg.payload = serializePaymentRequest(payment);

        if (it != m_pending_info_sales.end()) {
            it->second.type_sale = std::to_string(TypeSale::Full);
            it->second.fact_quantity = utility::getFormattedStringFromJson(
                std::to_string(fact_volume_value), fact_volume_decimal);
            it->second.fact_amount = utility::getFormattedStringFromJson(
                std::to_string(fact_amount_value), fact_amount_decimal);
        }

        if (status == ORDER_INTERRUPTED) {
            // ВАЖНО: Рисуем экраны возврата ТОЛЬКО если эта колонка сейчас открыта на экране
            if (device_id == m_current_dispenser_id) {
                Message revert_amount_message;
                std::string amount_revert_fuel_liters = utility::calculateRevertFuelString(payment.volume_value, payment.volume_decimal, fact_volume_value, fact_volume_decimal);
                revert_amount_message.source = PRIME_HTTP_LAYER;
                revert_amount_message.type = PAY_REVERT;
                revert_amount_message.payload.assign(amount_revert_fuel_liters.begin(), amount_revert_fuel_liters.end());

                core.sendToLogicLayer(UART_LAYER, revert_amount_message);

                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_return_money_process);
            }

            // Печать чека возврата при прерывании заказа
            Product *product = utility::getProductById(m_products, order.product_id);
            if (product) {
                double actual_volume = utility::formatDecimalValue(fact_volume_value, fact_volume_decimal) !=
                                       "0"
                                           ? std::stod(
                                               utility::formatDecimalValue(
                                                   fact_volume_value, fact_volume_decimal))
                                           : order.volume;

                ReceiptData receipt;
                receipt.address = m_settings.gas_station.standalone_address;
                receipt.product_name = product->title;
                receipt.volume_liters = actual_volume;
                receipt.ordered_volume_liters = order.order_volume;
                receipt.price_per_liter = order.order_price;
                receipt.is_refund = true;

                Message refund_msg;
                refund_msg.source = PRIME_HTTP_LAYER;
                refund_msg.type = PRINT_REFUND_RECEIPT;
                refund_msg.resource_id = order.id;
                refund_msg.payload = serializeReceiptData(receipt);

                if (it != m_pending_info_sales.end()) {
                    it->second.type_sale = std::to_string(TypeSale::Partial);
                    it->second.fact_quantity = utility::getFormattedStringFromJson(
                        std::to_string(fact_volume_value), fact_volume_decimal);
                    it->second.fact_amount = utility::getFormattedStringFromJson(
                        std::to_string(fact_amount_value), fact_amount_decimal);
                }
                core.sendToLogicLayer(PIPE_LAYER, refund_msg);
            }
        }
        core.sendToLogicLayer(PIPE_LAYER, msg);
    }

    void PrimeLogic::handleRandomDispenserOrderStatus(const json &jArray) {
        try {
            std::string device_id = jArray["device_id"];
            auto data = jArray["on_dsp_order_changed"];
            auto status = utility::parseStringFromJson(data["status"]);

            m_orders[device_id].status = status;
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error Dispenser Random Order Status: " << e.what() << std::endl;
        }
    }

    void PrimeLogic::authorizeOnServer(MessageLayer &core) {
        try {
            if (m_is_server_ready_for_start_fuel && m_dwin_button_next_for_start_fuel) {
                std::string command = m_settings.APIDispenser.authorize;
                PrimeCommands::handleCommand(core, command, m_current_dispenser_id);

                m_is_server_ready_for_start_fuel = false;
                m_dwin_button_next_for_start_fuel = false;
            } else {
                if (m_is_server_ready_for_start_fuel && !m_dwin_button_next_for_start_fuel) {
                    std::cout << "[Logic] Server Ready. Waiting for User..." << std::endl;
                } else if (!m_is_server_ready_for_start_fuel && m_dwin_button_next_for_start_fuel) {
                    std::cout << "[Logic] User Pressed. Waiting for Server..." << std::endl;
                }
            }
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error Dispenser Order Created: " << e.what() << std::endl;
        }
    }

    void PrimeLogic::renderDispenser(MessageLayer &core) {
        if (m_selected_dispensers.empty()) return;
        std::cout << "RENDER DISPENSER " << std::endl;
        int amount = m_amount_trk;
        if (amount == 1) {
            renderMonoDispenser(core);
        } else if (amount == 2) {
            renderDuoDispenser(core);
        } else {
            renderMultiDispenser(core);
        }
    }

    void PrimeLogic::renderMultiDispenser(MessageLayer &core) {
        std::string trk_id = m_selected_dispensers[m_current_dispenser_index];
        Dispenser *dispenser = utility::getDispenserById(m_dispensers, trk_id);

        int icon_id = utility::getIconForStatus(dispenser->status, m_settings);
        std::string trk_id_number = utility::extractFirstInt(trk_id);
        DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_icon_trk, icon_id);
        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_trk, trk_id_number, m_settings.dwin.text_len_trk_id);
        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_trk_id_first_page,
                                     utility::extractFirstInt(trk_id_number), m_settings.dwin.text_len_trk_id);
    }

    void PrimeLogic::renderMonoDispenser(MessageLayer &core) {
        std::string trk_id = m_selected_dispensers[0];
        Dispenser *dispenser = utility::getDispenserById(m_dispensers, trk_id);

        int icon_id = utility::getIconForStatus(dispenser->status, m_settings);
        std::string trk_id_number = utility::extractFirstInt(trk_id);
        DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_icon_trk_uno, icon_id);
        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_trk_uno, trk_id_number,
                                     m_settings.dwin.text_len_trk_id);
    }

    void PrimeLogic::renderDuoDispenser(MessageLayer &core) {
        for (int i = 0; i < 2 && i < (int) m_selected_dispensers.size(); ++i) {
            std::string trk_id = m_selected_dispensers[i];
            Dispenser *dispenser = utility::getDispenserById(m_dispensers, trk_id);

            int icon_id = utility::getIconForStatus(dispenser->status, m_settings);
            std::string trk_id_number = utility::extractFirstInt(trk_id);

            if (i == 0) {
                DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_icon_trk_duo_left, icon_id);
                DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_trk_duo_left, trk_id_number,
                                             m_settings.dwin.text_len_trk_id);
            } else {
                DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_icon_trk_duo_right, icon_id);
                DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_trk_duo_right, trk_id_number,
                                             m_settings.dwin.text_len_trk_id);
            }
        }
    }

    void PrimeLogic::processDispenserIdleAfterUserTouch(MessageLayer &core) {
        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_trk_id_second_page,
                                     utility::extractFirstInt(m_current_dispenser_id), m_settings.dwin.text_len_trk_id);
        DwinCommands::sendPageToDwin(core, m_settings.dwin.page_set_nozzle_into_gasoline);
        clearTimers();
        startPageTimer(m_settings.business_logic.sleep_after_chosen_trk_page,
                       getStartPage(),
                       [this, &core]() {
                           m_current_dispenser_id.clear();
                           renderDispenser(core);
                       });
    }

    void PrimeLogic::processDispenserNozzleUpAfterUserTouch(MessageLayer &core, Product *product) {
        clearTimers();

        int icon_fuel = utility::getIconForProduct(product->rating, m_settings);
        setProductIdOnDisplay(core, icon_fuel);
        setFuelTypePriceOnDisplay(core, product->price);
        setProductTextOnDisplay(core, utility::extractFirstInt(product->id));
        DwinCommands::sendPlaySoundToDwin(core, m_settings.dwin.audio_id_welcome_nozzle_up);
        DwinCommands::sendPageToDwin(core, m_settings.dwin.page_put_card_or_scan_code);
    }

    void PrimeLogic::processDispenserStatus(const json &jObj, MessageLayer &core, const std::string &url) {
        try {
            std::string trk_id = utility::getIdFromUrl(url, "/devices/"); // хардкод с маркером
            std::string status = utility::parseStringFromJson(jObj["status"]);

            // Вывод на экран "Установите топливный кран" цены, объема и цены за литр последнего заказа ТРК.
            auto amount = jObj["amount"];
            std::string amount_string = utility::getFormattedStringFromJson(
                utility::parseStringFromJson(amount["value"]), utility::parseIntFromJson(amount["exponent"]));

            auto volume = jObj["volume"];
            std::string volume_string = utility::getFormattedStringFromJson(
                utility::parseStringFromJson(volume["value"]), utility::parseIntFromJson(volume["exponent"]));

            auto price = jObj["price"];
            std::string price_string = utility::getFormattedStringFromJson(
                utility::parseStringFromJson(price["value"]), utility::parseIntFromJson(price["exponent"]));

            sendFloatToDwin(m_settings.dwin.vp_volume_last_order_integer, m_settings.dwin.vp_volume_last_order_decimal,
                            core, m_settings.dwin.text_len_order_integer, m_settings.dwin.text_len_order_decimal,
                            volume_string);

            sendFloatToDwin(m_settings.dwin.vp_price_last_order_integer, m_settings.dwin.vp_price_last_order_decimal,
                            core, m_settings.dwin.text_len_order_integer, m_settings.dwin.text_len_order_decimal,
                            amount_string);

            sendFloatToDwin(m_settings.dwin.vp_current_price_per_liter_last_order_integer,
                            m_settings.dwin.vp_current_price_per_liter_last_order_decimal,
                            core, m_settings.dwin.text_len_order_integer, m_settings.dwin.text_len_order_decimal,
                            price_string);

            bool is_current_trk_updated = false;

            for (size_t i = 0; i < m_dispensers.size(); i++) {
                if (m_dispensers[i].id == trk_id) {
                    m_dispensers[i].prev_status = m_dispensers[i].status;
                    m_dispensers[i].status = status;
                    if (status == DISPENSER_FUELLING) {
                        m_dispensers[i].m_last_volume_change_time = std::chrono::steady_clock::now();
                        m_dispensers[i].is_fuelling_paused = false;
                    }

                    if (jObj.contains("nozzle")) {
                        auto nozzle = jObj["nozzle"];
                        std::string nozzle_string = utility::parseStringFromJson(nozzle["product_id"]);
                        m_dispensers[i].product_id = nozzle_string;
                        m_orders[trk_id].product_id = nozzle_string;
                    }

                    if (jObj.contains("order")) {
                        auto order = jObj["order"];
                        std::string order_id = utility::parseStringFromJson(order["id"]);

                        if (m_orders.find(trk_id) == m_orders.end() || m_orders[trk_id].id != order_id) {
                            // Deadly заказы закрывать.
                            std::string command = m_settings.APIDispenser.close;
                            PrimeCommands::handleCommand(core, command, trk_id);
                            break;
                        }

                        auto nozzle = order["nozzle"];
                        auto product_id = nozzle["product_id"];

                        auto target_amount = order["target_amount"];
                        std::string target_amount_string = utility::getFormattedStringFromJson(
                            utility::parseStringFromJson(target_amount["value"]),
                            utility::parseIntFromJson(target_amount["exponent"]));
                        std::string target_amount_value = target_amount["value"];
                        int target_amount_exponent = target_amount["exponent"];
                        //DwinCommands::sendPageToDwin(core, m_settings.dwin.page_waiting_card_operation);

                        auto target_volume = order["target_volume"];
                        std::string target_volume_string = utility::getFormattedStringFromJson(
                            utility::parseStringFromJson(target_volume["value"]),
                            utility::parseIntFromJson(target_volume["exponent"]));
                        std::string target_volume_value = target_volume["value"];
                        int target_volume_exponent = target_volume["exponent"];

                        auto price = order["price"];
                        //std::string price_value = price["value"];
                        std::string price_value = utility::getFormattedStringFromJson(
                            utility::parseStringFromJson(price["value"]),
                            utility::parseIntFromJson(price["exponent"]));
                        int price_exponent = price["exponent"];

                        m_dispensers[i].order.volume = target_volume_string;
                        m_dispensers[i].order.amount = target_amount_string;
                        m_dispensers[i].order.order_id = utility::parseStringFromJson(order["id"]);
                        m_dispensers[i].order.price = price_value;

                        m_orders[trk_id].order_type = utility::parseStringFromJson(order["type"]);
                        m_orders[trk_id].order_amount = utility::parseDoubleFromJson(
                            target_amount["value"], target_amount["exponent"]);
                        m_orders[trk_id].order_value = utility::parseDoubleFromJson(
                            target_volume["value"], target_volume["exponent"]);
                        m_orders[trk_id].order_volume = utility::parseDoubleFromJson(
                            target_volume["value"], target_volume["exponent"]);
                        m_orders[trk_id].order_price = utility::parseDoubleFromJson(price["value"], price["exponent"]);
                        m_orders[trk_id].complete = utility::parseBoolFromJson(order["complete"]);
                        m_orders[trk_id].status = utility::parseStringFromJson(order["status"]);
                        m_orders[trk_id].id = order_id;


                        //setCurrentOrderAmountOnDisplay(core, target_amount_string);
                        Message msg_order_amount;
                        msg_order_amount.type = UPDATE_CURRENT_ORDER_AMOUNT;
                        msg_order_amount.payload.assign(target_amount_string.begin(), target_amount_string.end());

                        core.sendToLogicLayer(UART_LAYER, msg_order_amount);

                        //setCurrentOrderVolumeOnDisplay(core, target_volume_string);
                        Message msg_order_volume;
                        msg_order_volume.type = UPDATE_CURRENT_ORDER_VOLUME;
                        msg_order_volume.payload.assign(target_volume_string.begin(), target_volume_string.end());

                        core.sendToLogicLayer(UART_LAYER, msg_order_volume);

                        //setCurrentFuelingVolume(core, target_volume_string);
                        // Message msg_order_fuelling_volume;
                        // msg_order_fuelling_volume.type = UPDATE_CURRENT_FUELLING_VOLUME;
                        // msg_order_fuelling_volume.payload.assign(target_volume_string.begin(),
                        //                                          target_volume_string.end());
                        //
                        // core.sendToLogicLayer(UART_LAYER, msg_order_fuelling_volume);


                        std::string product_id_processing = utility::primeIdToProcessingId(
                            product_id, m_settings.gas_station.prime_standalone,
                            m_settings.gas_station.processing_standalone);

                        Product *product = utility::getProductById(m_products, product_id);

                        if (m_pending_info_sales.find(order_id) == m_pending_info_sales.end()) {
                            SalesData sales_data;

                            sales_data.rqs_amount = target_amount_string;
                            sales_data.rqs_price = price_value;
                            sales_data.rqs_quantity = target_volume_string;
                            sales_data.fuel_name = product->title != "" ? product->title : product->id;
                            sales_data.id_tso = m_settings.gas_station.standalone_id;
                            sales_data.trk_name = m_settings.gas_station.standalone_id;
                            sales_data.processing = "optima-pc.init-plus";
                            sales_data.discount = "";
                            sales_data.dsc = "";
                            sales_data.dispenser_volume_end = "0";
                            // хз зачем это поле.
                            sales_data.fact_price = price_value;
                            m_pending_info_sales[order_id] = sales_data;
                        }

                        createPayment(core, order_id, product_id_processing,
                                      std::stoi(price_value),
                                      price_exponent,
                                      std::stoi(target_volume_value),
                                      target_volume_exponent,
                                      std::stoi(target_amount_value),
                                      target_amount_exponent);
                    }
                    if (static_cast<int>(i) == m_current_dispenser_index) {
                        is_current_trk_updated = true;
                    }
                    break;
                }
            }
            if (is_current_trk_updated) {
                renderDispenser(core);
            }
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Dispenser Status: " << e.what() << std::endl;
        }
    }

    void PrimeLogic::processParameters(const json &jArray, std::string url, MessageLayer &core) {
        try {
            std::string level_gauge_id = utility::getIdFromUrl(url, "/devices/"); // хардкод с маркером
            LevelGauge *level_gauge = utility::getLevelGaugeById(m_level_gauge, level_gauge_id);
            auto it = m_pending_level_gauge_orders.find(level_gauge_id);

            for (const auto &parameter: jArray["parameters"]) {
                std::string key = parameter["key"].get<std::string>();
                auto &decimal_node = parameter["decimal"];
                std::string val_str = decimal_node.value("value", "0");
                int exponent = decimal_node.value("exponent", 0);
                std::string formatted_value = utility::getFormattedStringFromJson(val_str, exponent);

                if (key == DENSITY) {
                    level_gauge->density = formatted_value;
                } else if (key == FILLING) {
                    level_gauge->filling = formatted_value;
                } else if (key == LOWER_LEVEL) {
                    level_gauge->lower_level = formatted_value;
                } else if (key == LOWER_VOLUME) {
                    level_gauge->lower_volume = formatted_value;
                } else if (key == TOTAL_VOLUME) {
                    level_gauge->total_volume = formatted_value;
                } else if (key == UPPER_LEVEL) {
                    level_gauge->upper_level = formatted_value;
                } else if (key == UPPER_VOLUME) {
                    level_gauge->upper_volume = formatted_value;
                } else if (key == WEIGHT) {
                    level_gauge->weight = formatted_value;
                }
            }

            // Обновление информации об уровнемере в m_sales.
            if (it != m_pending_level_gauge_orders.end() && !it->second.empty()) {
                PendingGaugeTask task = it->second.front();
                it->second.pop();
                // Заполнение для INIT BOS начального состояния уровнемера.
                auto pending_sale = m_pending_info_sales.find(task.order_id);
                if (task.task_type == GaugeTaskType::BEFORE_FUELLING) {
                    if (pending_sale != m_pending_info_sales.end()) {
                        pending_sale->second.before_density = level_gauge->density;
                        pending_sale->second.before_filling = level_gauge->filling;
                        pending_sale->second.before_lower_level = level_gauge->lower_level;
                        pending_sale->second.before_lower_volume = level_gauge->lower_volume;
                        pending_sale->second.before_temperature = "12";
                        pending_sale->second.before_total_volume = level_gauge->total_volume;
                        pending_sale->second.before_weight = level_gauge->weight;
                        pending_sale->second.before_upper_level = level_gauge->upper_level;
                        pending_sale->second.before_upper_volume = level_gauge->upper_volume;
                    }
                    if (it->second.empty()) {
                        m_pending_level_gauge_orders.erase(it);
                    }
                } else if (task.task_type == GaugeTaskType::AFTER_FUELLING) {
                    // Заполнение для INIT BOS конечного состояния уровнемера.
                    // После этого можно отправлять данные в BOS.
                    if (pending_sale != m_pending_info_sales.end()) {
                        pending_sale->second.after_density = level_gauge->density;
                        pending_sale->second.after_filling = level_gauge->filling;
                        pending_sale->second.after_lower_level = level_gauge->lower_level;
                        pending_sale->second.after_lower_volume = level_gauge->lower_volume;
                        pending_sale->second.after_temperature = "12";
                        pending_sale->second.after_total_volume = level_gauge->total_volume;
                        pending_sale->second.after_weight = level_gauge->weight;
                        pending_sale->second.after_upper_level = level_gauge->upper_level;
                        pending_sale->second.after_upper_volume = level_gauge->upper_volume;
                        if (pending_sale->second.type_sale == std::to_string(TypeSale::Revert)) {
                            pending_sale->second.before_density = level_gauge->density;
                            pending_sale->second.before_filling = level_gauge->filling;
                            pending_sale->second.before_lower_level = level_gauge->lower_level;
                            pending_sale->second.before_lower_volume = level_gauge->lower_volume;
                            pending_sale->second.before_temperature = "12";
                            pending_sale->second.before_total_volume = level_gauge->total_volume;
                            pending_sale->second.before_weight = level_gauge->weight;
                            pending_sale->second.before_upper_level = level_gauge->upper_level;
                            pending_sale->second.before_upper_volume = level_gauge->upper_volume;
                        }

                        Message bos_message;
                        bos_message.type = BOS_MESSAGE_SET_SALES;
                        bos_message.source = PRIME_HTTP_LAYER;
                        auto payload_bytes = serializeSalesData(pending_sale->second);
                        bos_message.payload.assign(payload_bytes.begin(), payload_bytes.end());

                        core.sendToLogicLayer(BOS_HTTP_LAYER, bos_message);
                        m_pending_info_sales.erase(pending_sale);
                        m_pending_level_gauge_orders.erase(it);
                    }
                }
            }
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Parameters: " << e.what() << std::endl;
        }
    }

    void PrimeLogic::processDevices(const json &jArray, MessageLayer &core) {
        if (m_settings.gas_station.amount_trk == 0) {
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_set_amount_trk);
        }
        m_dispensers.clear();
        m_level_gauge.clear();
        try {
            for (const auto &device: jArray["devices"]) {
                if (device["api"] == API_DISPENSER) {
                    std::string trk_id = device["id"];
                    Dispenser new_dispenser_model(trk_id);
                    m_dispensers.push_back(new_dispenser_model);
                    getDispenserStatus(core, trk_id);
                } else if (device["api"] == API_IO) {
                    std::string device_id = device["id"];

                    if (device_id.find("vlg") != std::string::npos) {
                        LevelGauge new_level_gauge(device_id);
                        m_level_gauge.push_back(new_level_gauge);
                        getParameters(core, device_id);
                    }
                }
            }

            if (m_selected_dispensers.empty()) {
                for (const auto &trk: m_dispensers) {
                    m_selected_dispensers.push_back(trk.id);
                }
            }
            renderDispenser(core);
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Devices: " << e.what() << std::endl;
        }
    }

    void PrimeLogic::processTaskId(const json &jArray, MessageLayer &core, std::string url) {
        try {
            int task_id = utility::parseIntFromJson(jArray["task_id"]);
            if (url.find("order") != std::string::npos) {
                m_current_order_task = task_id;
            }
            sendTaskToHttp(core, std::to_string(task_id));
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing TaskId: " << e.what() << std::endl;
        }
    }

    void PrimeLogic::processTask(const json &jArray, MessageLayer &core) const {
        try {
            auto task = jArray["task"];
            bool ok = utility::parseBoolFromJson(task["ok"]);
            bool done = utility::parseBoolFromJson(task["done"]);
            int task_id = utility::parseIntFromJson(task["id"]);

            if (!done) {
                auto task_timer = std::make_shared<boost::asio::deadline_timer>(
                m_ios,
                boost::posix_time::milliseconds(200)
            );

                auto self = shared_from_this();

                task_timer->async_wait([self, &core, task_id, task_timer](const boost::system::error_code &ec) {
                    if (!ec) {
                        self->sendTaskToHttp(core, std::to_string(task_id));
                    }
                });
                sendTaskToHttp(core, std::to_string(task_id));
            } else {
                // if (task_id == m_current_order_task) {
                //      Перелистываем на страницу "операция по карте выполняется".
                //      DwinCommands::sendPageToDwin(core, m_settings.dwin.page_processing_fuel_card);
                // }
                //getDispenserStatus(core, m_current_dispenser_id);
            }
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Task: " << e.what() << std::endl;
        }
    }

    void PrimeLogic::fillPageEditingFuelTypeSecondVariable(std::vector<Product> &products, MessageLayer &core) {
        Product &p = products[m_current_fuel_type_index];

        int id_product = utility::getIconForProduct(p.rating, m_settings);
        DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_icon_fuel_type_for_editing, id_product);

        sendFloatToDwin(m_settings.dwin.vp_fuel_price_for_editing_integer,
                        m_settings.dwin.vp_fuel_price_for_editing_decimal,
                        core, m_settings.dwin.text_len_fuel_integer, m_settings.dwin.text_len_order_decimal, p.price);
    }

    void PrimeLogic::processProducts(const json &jArray) {
        try {
            for (const auto &product: jArray["products"]) {
                std::string fuel_id = utility::parseStringFromJson(product["id"]);
                Product new_product(fuel_id);

                std::string fuel_title;
                if (product.contains("title")) {
                    new_product.title = utility::parseStringFromJson(product["title"]);
                }
                if (product.contains("rating")) {
                    new_product.rating = utility::parseStringFromJson(product["rating"]);
                }

                // Проверка конфига. Если нет цены на вид топлива - отправить заполнять ее.
                auto it = m_settings.gas_station.fuel_prices.find(fuel_id);
                if (it != m_settings.gas_station.fuel_prices.end()) {
                    // заполнять ее с запятыми
                    new_product.price = it->second;
                } else {
                    new_product.price = "0";
                }

                m_products.push_back(new_product);
            }
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Products: " << e.what() << std::endl;
        }
    }

    void PrimeLogic::processTankers(const json &jArray) {
        try {
            for (const auto &tanker: jArray["tankers"]) {
                std::string tanker_id = utility::parseStringFromJson(tanker["id"]);
                Tanker new_tanker(tanker_id);

                new_tanker.product_id = utility::parseStringFromJson(tanker["product_id"]);
                if (tanker.contains("title")) {
                    new_tanker.title = utility::parseStringFromJson(tanker["title"]);
                }
                new_tanker.active = utility::parseBoolFromJson(tanker["active"]);
                new_tanker.filled = utility::parseBoolFromJson(tanker["filled"]);

                auto minimal = tanker["minimal"];
                new_tanker.minimal = utility::getFormattedStringFromJson(
                    utility::parseStringFromJson(minimal["value"]), utility::parseIntFromJson(minimal["exponent"]));

                auto lock = tanker["lock"];
                new_tanker.lock = utility::getFormattedStringFromJson(
                    utility::parseStringFromJson(lock["value"]), utility::parseIntFromJson(lock["exponent"]));

                for (const auto &device_id: tanker["device_ids"]) {
                    new_tanker.level_gauge_ids.push_back(device_id.get<std::string>());
                }

                m_tankers.push_back(new_tanker);
            }
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Tankers: " << e.what() << std::endl;
        }
    }

    void PrimeLogic::setTRKIdOnDisplay(MessageLayer &core, const std::string &trk_id) const {
        for (uint16_t vp: m_settings.dwin.vp_trk_id_pages) {
            DwinCommands::sendTextToDwin(core, vp, trk_id, m_settings.dwin.text_len_trk_id);
        }
    }

    void PrimeLogic::setProductIdOnDisplay(MessageLayer &core, int value) const {
        for (uint16_t vp: m_settings.dwin.vp_product_id_pages) {
            if (vp == 0) continue;
            DwinCommands::sendInt16ToDwin(core, vp, value);
        }
    }

    void PrimeLogic::setFuelTypePriceOnDisplay(MessageLayer &core, const std::string &value) const {
        std::string int_part_fuel_price, dec_part_fuel_price;
        std::tie(int_part_fuel_price, dec_part_fuel_price) = utility::splitFloatString(value, 2);

        for (uint16_t vp: m_settings.dwin.vp_current_fuel_type_price_pages_integer) {
            if (vp == 0) continue;
            DwinCommands::sendRightAlignmentWithPadding(core, vp, int_part_fuel_price,
                                                        m_settings.dwin.text_len_fuel_integer);
        }

        for (uint16_t vp: m_settings.dwin.vp_current_fuel_type_price_pages_decimal) {
            if (vp == 0) continue;
            DwinCommands::sendRightAlignmentWithPadding(core, vp, dec_part_fuel_price,
                                                        m_settings.dwin.text_len_order_decimal);
        }
    }

    void PrimeLogic::setProductTextOnDisplay(MessageLayer &core, const std::string &text) const {
        for (uint16_t vp: m_settings.dwin.vp_product_text_pages) {
            if (vp == 0) continue;
            DwinCommands::sendTextToDwin(core, vp, text, m_settings.dwin.text_len_fuel_type);
        }
    }

    void PrimeLogic::setFooterDateTime(MessageLayer &core) {
        std::string current_time = utility::getCurrentTimeString();

        if (current_time != m_last_time_str) {
            m_last_time_str = current_time;

            for (uint16_t vp: m_settings.dwin.vp_current_date_time_pages) {
                if (vp == 0) continue;
                DwinCommands::sendTextToDwin(core, vp, current_time, m_settings.dwin.text_len_date_time_footer);
            }
        }
    }

    void PrimeLogic::setLevelGaugeParametersOnDisplay(MessageLayer &core, const LevelGauge &level_gauge,
                                                      const std::string &type) {
        const Tanker *tanker = utility::getTankerByLevelGaugeId(m_tankers, level_gauge.id);
        if (tanker != nullptr) {
            Product *product = utility::getProductById(m_products, tanker->product_id);
            if (product != nullptr) {
                int filling_percent = utility::ceilStringToInt(level_gauge.filling);
                std::string percent_str = std::to_string(filling_percent);

                if (type == USER_TOUCH_SERVICE_MENU_LEVEL_GAUGE_BUTTON || type ==
                    USER_TOUCH_PAGINATION_LEVEL_GAUGE_BUTTON) {
                    DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_level_gauge_id,
                                                 utility::extractFirstInt(product->id),
                                                 m_settings.dwin.text_len_fuel_integer);

                    DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_text_gauges_filling_percent,
                                                                percent_str,
                                                                m_settings.dwin.text_len_percent_progress_bar);
                    DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_icon_level_gauges_volume, filling_percent);

                    sendFloatToDwin(m_settings.dwin.vp_text_upper_level_gauges_integer,
                                    m_settings.dwin.vp_text_upper_level_gauges_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.upper_level);
                    sendFloatToDwin(m_settings.dwin.vp_text_upper_volume_gauges_integer,
                                    m_settings.dwin.vp_text_upper_volume_gauges_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.upper_volume);
                    sendFloatToDwin(m_settings.dwin.vp_text_weight_gauge_integer,
                                    m_settings.dwin.vp_text_weight_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.weight);
                    sendFloatToDwin(m_settings.dwin.vp_text_density_gauge_integer,
                                    m_settings.dwin.vp_text_density_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.density);
                    sendFloatToDwin(m_settings.dwin.vp_text_lower_level_gauge_integer,
                                    m_settings.dwin.vp_text_lower_level_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.lower_level);
                    sendFloatToDwin(m_settings.dwin.vp_text_lower_volume_gauge_integer,
                                    m_settings.dwin.vp_text_lower_volume_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.lower_volume);
                    sendFloatToDwin(m_settings.dwin.vp_text_total_volume_gauge_integer,
                                    m_settings.dwin.vp_text_total_volume_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.total_volume);
                } else if (type == USER_TOUCH_PAGINATION_RECEPTION_LEVEL_GAUGE_BUTTON) {
                    DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_choose_reception_gauge_id,
                                                 utility::extractFirstInt(product->id),
                                                 m_settings.dwin.text_len_fuel_integer);
                    DwinCommands::sendRightAlignmentWithPadding(
                        core, m_settings.dwin.vp_text_choose_reception_gauge_filling_percent, percent_str,
                        m_settings.dwin.text_len_percent_progress_bar);
                    DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_icon_choose_reception_gauge,
                                                  filling_percent);

                    sendFloatToDwin(m_settings.dwin.vp_text_upper_level_choose_reception_gauge_integer,
                                    m_settings.dwin.vp_text_upper_level_choose_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.upper_level);
                    sendFloatToDwin(m_settings.dwin.vp_text_upper_volume_choose_reception_gauge_integer,
                                    m_settings.dwin.vp_text_upper_volume_choose_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.upper_volume);
                    sendFloatToDwin(m_settings.dwin.vp_text_weight_choose_reception_gauge_integer,
                                    m_settings.dwin.vp_text_weight_choose_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.weight);
                    sendFloatToDwin(m_settings.dwin.vp_text_density_choose_reception_gauge_integer,
                                    m_settings.dwin.vp_text_density_choose_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.density);
                    sendFloatToDwin(m_settings.dwin.vp_text_lower_level_choose_reception_gauge_integer,
                                    m_settings.dwin.vp_text_lower_level_choose_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.lower_level);
                    sendFloatToDwin(m_settings.dwin.vp_text_lower_volume_choose_reception_gauge_integer,
                                    m_settings.dwin.vp_text_lower_volume_choose_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.lower_volume);
                    sendFloatToDwin(m_settings.dwin.vp_text_total_volume_choose_reception_gauge_integer,
                                    m_settings.dwin.vp_text_total_volume_choose_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.total_volume);
                } else if (type == USER_TOUCH_ACCEPT_RECEPTION_FUEL_BUTTON) {
                    DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_reception_gauge_id,
                                                 utility::extractFirstInt(product->id),
                                                 m_settings.dwin.text_len_fuel_integer);
                    DwinCommands::sendRightAlignmentWithPadding(
                        core, m_settings.dwin.vp_text_reception_gauge_filling_percent, percent_str,
                        m_settings.dwin.text_len_percent_progress_bar);
                    DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_icon_reception_gauge, filling_percent);

                    sendFloatToDwin(m_settings.dwin.vp_text_upper_level_reception_gauge_integer,
                                    m_settings.dwin.vp_text_upper_level_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.upper_level);
                    sendFloatToDwin(m_settings.dwin.vp_text_upper_volume_reception_gauge_integer,
                                    m_settings.dwin.vp_text_upper_volume_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.upper_volume);
                    sendFloatToDwin(m_settings.dwin.vp_text_weight_reception_gauge_integer,
                                    m_settings.dwin.vp_text_weight_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.weight);
                    sendFloatToDwin(m_settings.dwin.vp_text_density_reception_gauge_integer,
                                    m_settings.dwin.vp_text_density_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.density);
                    sendFloatToDwin(m_settings.dwin.vp_text_lower_level_reception_gauge_integer,
                                    m_settings.dwin.vp_text_lower_level_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.lower_level);
                    sendFloatToDwin(m_settings.dwin.vp_text_lower_volume_reception_gauge_integer,
                                    m_settings.dwin.vp_text_lower_volume_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.lower_volume);
                    sendFloatToDwin(m_settings.dwin.vp_text_total_volume_reception_gauge_integer,
                                    m_settings.dwin.vp_text_total_volume_reception_gauge_decimal,
                                    core, m_settings.dwin.text_len_order_integer, 1, level_gauge.total_volume);
                }
            }
        }
    }

    void PrimeLogic::sendFloatToDwin(uint16_t int_vp, uint16_t dec_vp, MessageLayer &core, int int_length,
                                     int dec_length, std::string value) {
        std::string int_part, dec_part;
        std::tie(int_part, dec_part) = utility::splitFloatString(value, dec_length);

        DwinCommands::sendRightAlignmentWithPadding(core, int_vp, int_part, int_length);
        DwinCommands::sendRightAlignmentWithPadding(core, dec_vp, dec_part, dec_length);
    }

    void PrimeLogic::handlePaymentResponse(const Message &msg, MessageLayer &core) {
        std::string order_id = msg.resource_id;

        std::string json_str(msg.payload.begin(), msg.payload.end());
        auto json = nlohmann::json::parse(json_str);

        bool success = json["success"];

        if (success) {
            uint64_t transaction_id = json["transaction_id"];
            std::string card_number = json["card_number"];
            std::string message = json["message"];

            auto it = m_pending_info_sales.find(order_id);
            if (it != m_pending_info_sales.end()) {
                it->second.card_number = card_number;
                it->second.transaction_id = std::to_string(transaction_id);
            }

            std::cout << "[PrimeLogic] Payment SUCCESS for order " << order_id
                    << ", transaction_id=" << transaction_id << std::endl;

            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_success_processing_fuel_card);
        } else {
            std::string error_msg = json["message"];

            std::cerr << "[PrimeLogic] Payment FAILED for order " << order_id
                    << ", message=" << error_msg << std::endl;

            if (error_msg == NO_PINPAD) {
                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_print_pin);
                return;
            } else if (error_msg == INCORECT_PINCODE) {
                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_error_incorrect_pincode);
                startPageTimer(m_settings.business_logic.show_incorrect_pin_page, m_settings.dwin.page_print_pin);
                return;
            }
            std::string command = m_settings.APIDispenser.close;
            PrimeCommands::handleCommand(core, command, m_current_dispenser_id);
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_error_transaction_failed);
            startPageTimer(m_settings.business_logic.show_return_money_process_end, getStartPage(), [&core, this]() {
                clearInputTextDwin( core);
            });
            m_pending_info_sales.erase(order_id);
        }
    }

    void PrimeLogic::checkFuellingProcess(MessageLayer &core) {
        if (m_current_dispenser_id.empty()) return;

        Dispenser *dispenser = utility::getDispenserById(m_dispensers, m_current_dispenser_id);

        if (dispenser && dispenser->status == DISPENSER_FUELLING) {
            auto now = std::chrono::steady_clock::now();

            auto timeout = std::chrono::seconds(m_settings.business_logic.waiting_dispenser_fuelling);
            // Допустим, 3-5 секунд
            if ((now - dispenser->m_last_volume_change_time) >= timeout) {
                if (!dispenser->is_fuelling_paused) {
                    dispenser->is_fuelling_paused = true;

                    std::cout << "[HttpLogic] Fuelling paused on TRK: " << m_current_dispenser_id << std::endl;
                    DwinCommands::sendPlaySoundToDwin(core, m_settings.dwin.audio_id_fuelling_interrupted);
                    m_last_fueling_sound = std::chrono::steady_clock::now();
                } else {
                    if (now - m_last_fueling_sound >= timeout) {
                        DwinCommands::sendPlaySoundToDwin(core, m_settings.dwin.audio_id_fuelling_interrupted);
                        m_last_fueling_sound = std::chrono::steady_clock::now();
                    }
                }
            }
        }
    }

    void PrimeLogic::clearInputTextDwin(MessageLayer &core) {
        Message clear_input_blocks_display;
        clear_input_blocks_display.type = CLEAR_CURRENT_ORDER_AMOUNT_AND_VOLUME;
        clear_input_blocks_display.source = PRIME_HTTP_LAYER;

        core.sendToLogicLayer(UART_LAYER, clear_input_blocks_display);
    }
}
