//
// Created by vadim.tissen on 12.12.2025.
//

#include "bridge/HttpLogic.hpp"
#include "bridge/MessageLayer.hpp"
#include <iostream>
#include "bridge/utility.hpp"
#include "bridge/constant.hpp"
#include "bridge/DwinCommands.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "bridge/PrimeCommands.hpp"
#include <tuple>
#include <utility>


// Для формирования запросов на дисплей. Тут я разбираю ответ от сервера и составляю Message для дисплея.
namespace bridge {
    HttpLogic::HttpLogic(const Settings &settings, boost::asio::io_service &ios) : m_settings(settings),
        m_timer(ios),
        m_ui_timer(ios),
        m_waitingResponse(false) {
    }

    void HttpLogic::getDevices(MessageLayer &core) {
        Message request;
        request.type = GET_DEVICES;

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::getParameters(MessageLayer &core, const std::string &id) {
        Message request;
        request.type = GET_IO_PARAMETERS;
        request.resource_id = id;

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::sendTaskToHttp(MessageLayer &core, std::string id) {
        Message request;
        request.type = GET_TASKS;
        request.resource_id = id;

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::getProducts(MessageLayer &core) {
        Message request;
        request.type = GET_PRODUCTS;

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::getTankers(MessageLayer &core) {
        Message request;
        request.type = GET_TANKERS;

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::getDispenserStatus(MessageLayer &core, const std::string &trk_id) {
        if (!trk_id.empty()) {
            Message request;
            request.type = GET_DISPENSER_STATUS;
            request.resource_id = trk_id;

            core.sendTo(HTTP_LAYER, request);
        }
    }

    void HttpLogic::startPolling(MessageLayer &core) const {
        Message request;
        request.type = GET_EVENTS;
        request.resource_id = std::to_string(m_lastId);

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::startLoop(MessageLayer &core) {
        getDevices(core);
        scheduleNextTick(core);
        getProducts(core);
        getTankers(core);

        m_amount_trk_png = utility::getIconForGasStation(m_settings.gas_station.amount_trk, m_settings);
        m_selected_dispensers = m_settings.gas_station.used_trks;
    }

    void HttpLogic::scheduleNextTick(MessageLayer &core) {
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

    void HttpLogic::onTimerExpired(const boost::system::error_code &ec, MessageLayer &core) {
        if (ec) return; // Таймер отменен

        // 1. Watchdog
        // Тут просто сбрасываем флаг, если зависли
        // (Для точного подсчета времени в секундах можно использовать std::chrono::steady_clock
        // внутри метода, он никак не конфликтует с boost::deadline_timer)
        if (m_waitingResponse.load()) {
            // Если нужно проверить таймаут > 5 сек, нужно хранить m_lastPollTime
            // Но для простоты примера: если в момент тика мы все еще ждем -
            // можно либо пропустить, либо форсировано сбросить.

            // Пример простой логики: если ждем ответа, новый запрос не шлем,
            // но если ждем ОЧЕНЬ долго (считаем счетчиком), то сбрасываем.
            // std::cerr << "Waiting for HTTP response..." << std::endl;
        }

        // 2. Polling
        // Если НЕ ждем ответа — шлем запрос
        if (!m_waitingResponse.load()) {
            startPolling(core);
            m_waitingResponse.store(true); // Ставим флаг
        }

        // 3. Проверка таймеров отложенных страниц
        checkTimers(core);

        // Перезавод таймера
        scheduleNextTick(core);
    }

    // Обработка входящего от сервера ответа. В паре с Parser.parse
    void HttpLogic::handle(const Message &message, MessageLayer &core) {
        m_waitingResponse.store(false);
        if (message.type == HTTP_RESPONSE) {
            processHandleHttp(message, core);
        } else if (message.type == USER_TOUCH_VOLUME_BUTTON) {
            uint16_t volume = (message.payload[0] << 8) | message.payload[1];
            handleCreateOrder(core, volume, 0);
        } else if (message.type == USER_TOUCH_PAGINATION_TRK_BUTTON) {
            int8_t direction = static_cast<int8_t>(message.payload[0]);
            handlePaginationDispenser(direction, core);
        } else if (message.type == USER_TOUCH_CHOOSE_MULTI_TRK_BUTTON) {
            handleChooseTRK(core, m_current_dispenser_index);
        } else if (message.type == USER_TOUCH_CHOOSE_TRK_BUTTON) {
            int8_t index = static_cast<int8_t>(message.payload[0]);
            handleChooseTRK(core, index);
        } else if (message.type == USER_TOUCH_BASIC_TOUCH_BUTTON) {
            handleBasicTouch(core);
        } else if (message.type == USER_TOUCH_BASIC_TOUCH_BEGIN_FUELLING_BUTTON) {
            m_dwin_button_next_for_start_fuel = true;
            authorizeOnServer(core);
        } else if (message.type == USER_TOUCH_BASIC_TOUCH_CREATE_NEW_ORDER_BUTTON) {
            // TODO
        } else if (message.type == USER_TOUCH_PIN_PAD_ENTER_FUEL_EDITING_BUTTON) {
            std::string pin_buffer(message.payload.begin(), message.payload.end());
            handleEnterFuelPricePinPad(core, pin_buffer);
        } else if (message.type == USER_TOUCH_PIN_PAD_ENTER_FUEL_VOLUME_BUTTON) {
            std::string pin_buffer(message.payload.begin(), message.payload.end());
            handleEnterFuelVolumeOrderPinPad(core, pin_buffer);
        } else if (message.type == USER_TOUCH_SERVICE_MENU_LEVEL_GAUGE_BUTTON) {
            handleLevelGaugeButton(message, core);
        } else if (message.type == USER_TOUCH_SERVICE_MENU_CHANGE_FUEL_PRICE_BUTTON) {
            fillPageEditingFuelTypeSecondVariable(m_products, core);
        } else if (message.type == USER_TOUCH_AMOUNT_TRK_BUTTON) {
            int8_t value = static_cast<int8_t>(message.payload[0]);
            handleAmountTRK(value);
        } else if (message.type == USER_TOUCH_CHOOSE_FUEL_FOR_EDIT_BUTTON) {
            handleEditFuelButton(core);
        } else if (message.type == USER_TOUCH_PAGINATION_FUEL_BUTTON) {
            int8_t direction = static_cast<int8_t>(message.payload[0]);
            changeFuelType(direction, core);
        } else if (message.type == USER_TOUCH_PAGINATION_LEVEL_GAUGE_BUTTON) {
            int8_t direction = static_cast<int8_t>(message.payload[0]);
            changeLevelGauge(direction, core);
        } else if (message.type == USER_TOUCH_RESET_SELECTED_TRK_BUTTON) {
            // Сброс выбора редактирования используемых ТРК.
            m_selected_dispensers.clear();
            renderTrkSelectionPage(core);

        } else if (message.type == USER_TOUCH_ACCEPT_SELECTED_TRK_BUTTON) {
            // Подтверждение выбора используемых ТРК.
            handleAcceptSelectedTRK(core);
        } else if (message.type == USER_TOUCH_SET_USED_TRK_BUTTON) {
            renderTrkSelectionPage(core);
        } else if (message.type == USER_TOUCH_TAP_ON_USED_TRK_BUTTON) {
            if (!message.payload.empty()) {
                uint8_t slot_index = message.payload[0];
                handleTrkSelectionToggle(core, slot_index);
            }
        } else if (message.type == USER_TOUCH_CLOSE_ORDER_BUTTON) {
            std::string command = m_settings.APIDispenser.close;
            PrimeCommands::handleCommand(core, command, m_current_dispenser_id);
            getDispenserStatus(core, m_current_dispenser_id);
        } else if (message.type == PAY_TRANSACTION_RESPONSE) {
            handlePaymentResponse(message, core);
        }
    }

    void HttpLogic::handleBasicTouch(MessageLayer& core) const {
            getDispenserStatus(core, m_current_dispenser_id);
    }

    void HttpLogic::handleAcceptSelectedTRK(MessageLayer& core) {
        if (m_selected_dispensers.empty()) {
            std::cout << "[Logic] Warning: Attempted to save empty TRK list. Ignored." << std::endl;
            return;
        }

        utility::saveUsedTRKsToConfig(CONFIG, m_selected_dispensers);

        m_amount_trk = m_selected_dispensers.size();

        m_current_dispenser_index = 0;
        m_current_dispenser_id.clear();
    }

    void HttpLogic::handleTrkSelectionToggle(MessageLayer& core, uint8_t slot_index) {
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

    void HttpLogic::renderTrkSelectionPage(MessageLayer& core) {
        // Проходим по всем 8 возможным слотам на экране DWIN
        for (size_t i = 0; i < 8; ++i) {
            std::string text_to_show = "";
            std::string prefix_to_show = "";

            // Если в этом слоте есть реальная ТРК
            if (i < m_dispensers.size()) {
                std::string trk_id = m_dispensers[i].id;
                text_to_show = trk_id;

                // Проверяем, выбрана ли она
                auto it = std::find(m_selected_dispensers.begin(), m_selected_dispensers.end(), trk_id);

                if (it != m_selected_dispensers.end()) {
                    // ВЫБРАНА: вычисляем порядковый номер (индекс в векторе + 1)
                    int order_number = std::distance(m_selected_dispensers.begin(), it) + 1;
                    prefix_to_show = std::to_string(order_number) + ".";
                } else {
                    prefix_to_show = ".";
                }
            }

            // Отправляем текст на дисплей
            DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_values_selected_trk[i], text_to_show,
                                                        m_settings.dwin.text_len_order_integer);
            DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_nums_selected_trk[i], prefix_to_show,
                                                        m_settings.dwin.text_len_order_decimal);
        }
    }

    void HttpLogic::handleEnterFuelPricePinPad(MessageLayer &core, std::string buffer) {
        if (!buffer.empty()) {
            int value, exponent;
            std::tie(value, exponent) = utility::formatFloatStringToInt(buffer);

            utility::saveFuelPriceToConfig(CONFIG, m_current_fuel_id_editing, buffer);
            for (auto &p: m_products) {
                if (p.id == m_current_fuel_id_editing) {
                    p.price = buffer;
                    sendFloatToDwin(m_settings.dwin.vp_fuel_price_for_editing_integer,
                                    m_settings.dwin.vp_fuel_price_for_editing_decimal,
                                    core, m_settings.dwin.text_len_fuel_integer, m_settings.dwin.text_len_order_decimal,
                                    p.price);
                    break;
                }
            }
        }
    }

    void HttpLogic::handleEnterFuelVolumeOrderPinPad(MessageLayer &core, std::string buffer) {
        if (!buffer.empty()) {
            int value, exponent;
            std::tie(value, exponent) = utility::formatFloatStringToInt(buffer);
            handleCreateOrder(core, value, exponent);
        }
    }

    void HttpLogic::handleChooseTRK(MessageLayer &core, int index) {
        bool fuel_price_set = utility::checkAllFuelPrice(m_products);
        if (fuel_price_set) {
            m_current_dispenser_id = m_selected_dispensers[index];
            Dispenser* dispenser = utility::getDispenserById(m_dispensers, m_current_dispenser_id);
            if (dispenser->status == DISPENSER_IDLE) {
                processDispenserIdleAfterUserTouch(core);
            } else if (dispenser->status == DISPENSER_NOZZLE_UP) {
                Product *product = utility::getProductById(m_products, dispenser->product_id);
                processDispenserNozzleUpAfterUserTouch(core, product);
                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_put_card_or_scan_code);
            }

            setTRKIdOnDisplay(core, utility::extractFirstInt(m_current_dispenser_id));
        } else {
            fillPageEditingFuelTypeSecondVariable(m_products, core);
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_set_fuel_price_another_variable);
        }
    }

    void HttpLogic::handleCreateOrder(MessageLayer &core, uint16_t volume, int exponent) {
        const Dispenser *dispenser = utility::getDispenserById(m_dispensers, m_current_dispenser_id);
        const Product *product = utility::getProductById(m_products, dispenser->product_id);

        int value_price, exponent_price;
        std::tie(value_price, exponent_price) = utility::formatFloatStringToInt(product->price);

        createOrder(core, volume, exponent, value_price, exponent_price);
    }

    void HttpLogic::createPayment(MessageLayer& core,
                                  const std::string& order_id,
                                  const std::string& product_id,
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
        message.source = HTTP_LAYER;
        message.type = PAY_TRANSACTION;
        message.resource_id = order_id;
        message.payload = serializePaymentRequest(payment);

        core.sendToLogicLayer(PIPE_LAYER, message);
        m_payment_requested_order_ids.insert(order_id);
    }

    void HttpLogic::processHandleHttpError(const Message &message, MessageLayer &core) {
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

    void HttpLogic::processHandleHttp(const Message &message, MessageLayer &core) {
        if (message.status_code >= 400) {
            processHandleHttpError(message, core);
        } else {
            std::string jsonStr(message.payload.begin(), message.payload.end());
            auto json = json::parse(jsonStr);
            if (json.is_object()) {
                processVariableHttp(json, core, message.url);
            }
        }
    }

    void HttpLogic::dispatchError(const ErrorEvent &error, MessageLayer &core) {
        if (error.source == ErrorSource::NETWORK) {
            // core.sendPageChange(m_settings.dwin.page_no_connection);
            return;
        }

        if (error.code == 401 || error.code == 403) {
            return;
        }

        if (error.code == 400 && error.description.find(LOW_VOLUME) != std::string::npos) {
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_error_low_fuel_level);
        }

        if (error.context.find("/order") != std::string::npos) {
            if (error.severity == ErrorSeverity::CRITICAL) {
                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_error_creating_order);
            }
        }
    }

    void HttpLogic::handleLevelGaugeButton(const Message &message, MessageLayer &core) {
        const LevelGauge *level_gauge = utility::getLevelGaugeById(m_level_gauge,
                                                                   m_level_gauge[m_current_level_gauge_index].id);
        setLevelGaugeParametersOnDisplay(core, *level_gauge);
    }

    void HttpLogic::handleEditFuelButton(MessageLayer &core) {
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

    void HttpLogic::checkTimers(MessageLayer &core) {
        auto now = std::chrono::steady_clock::now();

        for (auto it = m_page_timers.begin(); it != m_page_timers.end(); ) {
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

    void HttpLogic::startPageTimer(int timeout_seconds, uint16_t page_id, std::function<void()> on_expire) {
        PageTimer timer;
        timer.start = std::chrono::steady_clock::now();
        timer.timeout_seconds = timeout_seconds;
        timer.page_id = page_id;
        timer.on_expire = std::move(on_expire);
        m_page_timers.push_back(std::move(timer));
    }

    void HttpLogic::clearTimers() {
        m_page_timers.clear();
    }

    void HttpLogic::handleAmountTRK(int amount) {
        utility::saveTRKAmountToConfig(CONFIG, amount);
        m_amount_trk_png = utility::getIconForGasStation(m_amount_trk, m_settings);
        m_current_dispenser_index = 0;
        m_current_dispenser_id.clear();
    }

    void HttpLogic::handleFuelTypeEditing(const Message &message, MessageLayer &core) {
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

    void HttpLogic::createOrder(MessageLayer &core, int value, int exponent_volume, int price,
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

        Message request;
        request.resource_id = m_current_dispenser_id;
        request.type = CREATE_ORDER;
        request.payload.assign(json_string.begin(), json_string.end());

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::handlePaginationDispenser(int direction, MessageLayer &core) {
        if (m_selected_dispensers.empty()) {
            std::cout << "No TRK in map " << std::endl;
            return;
        }
        m_current_dispenser_index = utility::getPaginationIndex(m_current_dispenser_index, m_selected_dispensers.size(),
                                                                direction);

        std::string trk_id = m_selected_dispensers[m_current_dispenser_index];
        getDispenserStatus(core, trk_id);

        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_trk_id_first_page,
                                     utility::extractFirstInt(trk_id), m_settings.dwin.text_len_trk_id);
        renderDispenser(core);
    }

    void HttpLogic::changeLevelGauge(int direction, MessageLayer &core) {
        if (m_level_gauge.empty()) {
            std::cout << "No Level Gauge in map" << std::endl;
            return;
        }
        m_current_level_gauge_index = utility::getPaginationIndex(m_current_level_gauge_index, m_level_gauge.size(),
                                                                  direction);
        const auto &level_gauge = m_level_gauge[m_current_level_gauge_index];
        getParameters(core, level_gauge.id);
        setLevelGaugeParametersOnDisplay(core, level_gauge);
    }

    void HttpLogic::changeFuelType(int direction, MessageLayer &core) {
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

    void HttpLogic::processVariableHttp(const json &jArray, MessageLayer &core, const std::string &url) {
        if (jArray.contains("events")) {
            processEvent(jArray, core);
        } else if (jArray.contains("parameters")) {
            processParameters(jArray, url);
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

    void HttpLogic::processEvent(const json &jArray, MessageLayer &core) {
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
                    handleDispenserOrderStatus(event, core);
                }
            } else {
                if (event.contains("on_dsp_status_changed")) {
                    handleRandomDispenserStatus(event, core);
                } else if (event.contains("on_dsp_order_changed")) {
                    // TODO handle random dispenser order changed
                    handleRandomDispenserOrderStatus(event, core);
                }
            }
        }
        setFooterDateTime(core);
        checkTimers(core);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    void HttpLogic::handleRandomDispenserStatus(const json &event, MessageLayer &core) {
        try {
            auto data = event["on_dsp_status_changed"];
            std::string status = data.value("status", "");
            std::string dispenser_id = utility::parseStringFromJson(event["device_id"]);

            Dispenser *dispenser = utility::getDispenserById(m_dispensers, dispenser_id);
            dispenser->status = status;

            if (data.contains("nozzle")) {
                auto nozzle = data["nozzle"];
                std::string nozzle_string = utility::parseStringFromJson(nozzle["product_id"]);
                dispenser->product_id = nozzle_string;
            }

            if (m_selected_dispensers[m_current_dispenser_index] == dispenser_id || m_amount_trk != 3 ) {
                renderDispenser(core);
            }
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Random Dispenser Status: " << e.what() << std::endl;
        }
    }

    void HttpLogic::handleDispenserOrderCreated(const json &event, MessageLayer &core) {
        auto data = event["on_dsp_order_created"];
        std::string order_id = utility::parseStringFromJson(data["order_id"]);
        std::string dispenser_id = utility::parseStringFromJson(event["device_id"]);

        DatabaseOrder database_order(order_id);
        database_order.dispenser_id = dispenser_id;
        getDispenserStatus(core, dispenser_id);

        m_is_server_ready_for_start_fuel = true; // флаг на начало пролива только после нажатия на дисплей
        authorizeOnServer(core);

        m_orders[dispenser_id] = database_order;
    }

    void HttpLogic::handleDispenserDisplay(const json &event, MessageLayer &core) {
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
                m_orders[trk_id].amount = std::stod(amount_string);
                m_orders[trk_id].volume = std::stod(volume_string);
            }

            setCurrentFuelingVolume(core, volume_string);

            sendFloatToDwin(m_settings.dwin.vp_current_order_amount_integer,
                            m_settings.dwin.vp_current_order_amount_decimal,
                            core, m_settings.dwin.text_len_order_integer, m_settings.dwin.text_len_order_decimal,
                            amount_string);

            std::string current_volume_order_string;
            Dispenser *foundTrk = utility::getDispenserById(m_dispensers, trk_id);

            if (foundTrk != nullptr) {
                current_volume_order_string = foundTrk->order.volume;
            }

            int percent = 0;

            if (!volume_string.empty() && !current_volume_order_string.empty()) {
                double current = std::stod(volume_string);
                double total = std::stod(current_volume_order_string);

                if (total > 0.0) {
                    percent = static_cast<int>((current / total) * 100.0);
                }
            }
            std::cout << "Percent to bar" << percent << std::endl;
            std::string percent_str = std::to_string(percent);

            DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_progress_order_bar_eleventh_page, percent);
            DwinCommands::sendRightAlignmentWithPadding(
                core, m_settings.dwin.vp_progress_bar_percent_text_eleventh_page, percent_str,
                m_settings.dwin.text_len_percent_progress_bar);
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Dispenser Display: " << e.what() << std::endl;
        }
    }

    void HttpLogic::handleDispenserStatus(const json &event, MessageLayer &core) {
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
            uint8_t page_id = m_settings.dwin.page_put_card_or_scan_code;
            DwinCommands::sendPageToDwin(core, page_id);
            Product *product = utility::getProductById(m_products, dispenser->product_id);

            processDispenserNozzleUpAfterUserTouch(core, product);
        } else if (status == DISPENSER_IDLE) {
            clearTimers();

            if (dispenser->prev_status == DISPENSER_COMPLETE ||
                dispenser->prev_status == DISPENSER_FUELING ||
                dispenser->prev_status == DISPENSER_HALTED) {
                // После пролива — "Счастливого пути"
                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_good_trip);
            } else {
                // Первичный IDLE (пистолет не вставлен) — "Вставьте пистолет"
                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_set_nozzle_into_gasoline);
            }
            startPageTimer(5, m_settings.dwin.page_idle);
        } else if (status == DISPENSER_COMPLETE) {
            getDispenserStatus(core, m_current_dispenser_id);
            uint8_t page_id = m_settings.dwin.page_fuel_ended;
            if (!m_new_order_created) {
                DwinCommands::sendPageToDwin(core, page_id);
                DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_progress_order_bar_twelvth_page, 100);
                DwinCommands::sendRightAlignmentWithPadding(
                    core, m_settings.dwin.vp_progress_bar_percent_text_twelvth_page, "100",
                    m_settings.dwin.text_len_percent_progress_bar);
            }
            m_current_dispenser_id.clear();
        } else if (status == DISPENSER_FUELING) {
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_fuel_in_progress);
        }
    }

    void HttpLogic::handleDispenserOrderStatus(const json &jArray, MessageLayer &core) {
        try {
            std::string device_id = jArray["device_id"];
            auto data = jArray["on_dsp_order_changed"];
            auto status = utility::parseStringFromJson(data["status"]);

            m_orders[device_id].status = status;

            if (status == ORDER_INTERRUPTED || status == ORDER_DELIVERED) {
                if (status == ORDER_INTERRUPTED) {
                    // TODO добавить таймер на возврат пистолета в налив.
                    DwinCommands::sendPageToDwin(core, m_settings.dwin.page_return_money_process);
                }

                auto& order = m_orders[device_id];

                // Парсим фактические данные из event
                uint32_t fact_amount_value = 0;
                uint8_t fact_amount_decimal = 0;
                uint32_t fact_volume_value = 0;
                uint8_t fact_volume_decimal = 0;

                if (data.contains("amount")) {
                    auto amount = data["amount"];
                    fact_amount_value = static_cast<uint32_t>(std::stoul(utility::parseStringFromJson(amount["value"])));
                    fact_amount_decimal = static_cast<uint8_t>(utility::parseIntFromJson(amount["exponent"]));
                }
                if (data.contains("volume")) {
                    auto volume = data["volume"];
                    fact_volume_value = static_cast<uint32_t>(std::stoul(utility::parseStringFromJson(volume["value"])));
                    fact_volume_decimal = static_cast<uint8_t>(utility::parseIntFromJson(volume["exponent"]));
                }

                // Цена за литр (исходная)
                int price_val = 0, price_exp = 0;
                Product* product = utility::getProductById(m_products, order.product_id);
                if (product) {
                    std::tie(price_val, price_exp) = utility::formatFloatStringToInt(product->price);
                }

                // Исходные данные заказа (заказанные объём/сумма)
                int order_volume_val = 0, order_volume_exp = 0;
                int order_amount_val = 0, order_amount_exp = 0;
                Dispenser* dispenser = utility::getDispenserById(m_dispensers, device_id);
                if (dispenser) {
                    std::tie(order_volume_val, order_volume_exp) = utility::formatFloatStringToInt(dispenser->order.volume);
                    std::tie(order_amount_val, order_amount_exp) = utility::formatFloatStringToInt(dispenser->order.amount);
                }

                PaymentRequestData payment;
                payment.product_id = order.product_id;
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
                msg.source = HTTP_LAYER;
                msg.type = PAY_CONFIRM;
                msg.resource_id = order.id;
                msg.payload = serializePaymentRequest(payment);

                core.sendToLogicLayer(PIPE_LAYER, msg);

            } else if (status == ORDER_AUTHORIZED) {
                std::string time_begin_string = data["time_begin"];
                m_orders[device_id].time_begin = std::stoull(time_begin_string);
            }

        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error Dispenser Order Status: " << e.what() << std::endl;
        }
    }

    void HttpLogic::handleRandomDispenserOrderStatus(const json &jArray, MessageLayer &core) {
        try {
            std::string device_id = jArray["device_id"];
            auto data = jArray["on_dsp_order_changed"];
            auto status = utility::parseStringFromJson(data["status"]);

            m_orders[device_id].status = status;

        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error Dispenser Random Order Status: " << e.what() << std::endl;
        }
    }

    void HttpLogic::authorizeOnServer(MessageLayer &core) {
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

    void HttpLogic::renderDispenser(MessageLayer &core) {
        // TODO поддержка экранов с моно/дуо ТРК
        if (m_selected_dispensers.empty()) return;

        //const auto &trk = m_dispensers[m_current_dispenser_index];
        std::string trk_id = m_selected_dispensers[m_current_dispenser_index];

        Dispenser* dispenser = utility::getDispenserById(m_dispensers, trk_id);

        int icon_id = utility::getIconForStatus(dispenser->status, m_settings);
        std::string trk_id_number = utility::extractFirstInt(trk_id);
        DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_icon_trk, icon_id);
        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_trk, trk_id_number, m_settings.dwin.text_len_trk_id);
    }

    void HttpLogic::processDispenserIdleAfterUserTouch(MessageLayer &core) {
        DwinCommands::sendPageToDwin(core, m_settings.dwin.page_set_nozzle_into_gasoline);
        clearTimers();
        startPageTimer(m_settings.business_logic.sleep_after_chosen_trk_page,
                       m_settings.dwin.page_choose_trk,
                       [this, &core]() {
                           m_current_dispenser_id.clear();
                           renderDispenser(core);
                       });
    }

    void HttpLogic::processDispenserNozzleUpAfterUserTouch(MessageLayer &core, Product *product) {
        clearTimers();

        int icon_fuel = utility::getIconForProduct(product->rating, m_settings);
        setProductIdOnDisplay(core, icon_fuel);
        setFuelTypePriceOnDisplay(core, product->price);
        setProductTextOnDisplay(core, utility::extractFirstInt(product->id));

        DwinCommands::sendPageToDwin(core, m_settings.dwin.page_put_card_or_scan_code);
    }

    void HttpLogic::processDispenserStatus(const json &jObj, MessageLayer &core, const std::string &url) {
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
                    m_dispensers[i].status = status;

                    if (jObj.contains("nozzle")) {
                        auto nozzle = jObj["nozzle"];
                        std::string nozzle_string = utility::parseStringFromJson(nozzle["product_id"]);
                        m_dispensers[i].product_id = nozzle_string;
                        m_orders[trk_id].product_id = nozzle_string;
                    }

                    if (jObj.contains("order")) {
                        auto order = jObj["order"];

                        auto nozzle = jObj["nozzle"];
                        auto product_id = nozzle["product_id"];

                        auto target_amount = order["target_amount"];
                        std::string target_amount_string = utility::getFormattedStringFromJson(
                            utility::parseStringFromJson(target_amount["value"]),
                            utility::parseIntFromJson(target_amount["exponent"]));
                        std::string target_amount_value = target_amount["value"];
                        int target_amount_exponent = target_amount["exponent"];


                        auto target_volume = order["target_volume"];
                        std::string target_volume_string = utility::getFormattedStringFromJson(
                            utility::parseStringFromJson(target_volume["value"]),
                            utility::parseIntFromJson(target_volume["exponent"]));
                        std::string target_volume_value = target_volume["value"];
                        int target_volume_exponent = target_volume["exponent"];

                        auto price = jObj["price"];
                        std::string price_value = price["value"];
                        int price_exponent = price["exponent"];

                        m_dispensers[i].order.volume    = target_volume_string;
                        m_dispensers[i].order.amount    = target_amount_string;
                        m_dispensers[i].order.order_id  = utility::parseStringFromJson(order["id"]);

                        m_orders[trk_id].order_type     = utility::parseStringFromJson(order["type"]);
                        m_orders[trk_id].order_amount   = utility::parseDoubleFromJson(target_amount["value"], target_amount["exponent"]);
                        m_orders[trk_id].order_value    = utility::parseDoubleFromJson(target_volume["value"], target_volume["exponent"]);
                        m_orders[trk_id].order_price    = utility::parseDoubleFromJson(price["value"], price["exponent"]);
                        m_orders[trk_id].complete       = utility::parseBoolFromJson(order["complete"]);
                        m_orders[trk_id].status         = utility::parseStringFromJson(order["status"]);
                        m_orders[trk_id].id             = utility::parseStringFromJson(order["id"]);

                        setCurrentOrderAmountOnDisplay(core, m_dispensers[i].order.amount);
                        setCurrentOrderVolumeOnDisplay(core, m_dispensers[i].order.volume);

                        createPayment(core, m_orders[trk_id].id, product_id, std::stoi(price_value), price_exponent,
                            std::stoi(target_volume_value), target_volume_exponent, std::stoi(target_amount_value),
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

    void HttpLogic::processParameters(const json &jArray, std::string url) {
        try {
            std::string level_gauge_id = utility::getIdFromUrl(url, "/devices/"); // хардкод с маркером
            LevelGauge *level_gauge = utility::getLevelGaugeById(m_level_gauge, level_gauge_id);
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
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Parameters: " << e.what() << std::endl;
        }
    }

    void HttpLogic::processDevices(const json &jArray, MessageLayer &core) {
        if (m_settings.gas_station.amount_trk == 0) {
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_set_amount_trk);
        }
        m_dispensers.clear();
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
                for (const auto& trk : m_dispensers) {
                    m_selected_dispensers.push_back(trk.id);
                }
            }
            renderDispenser(core);
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Devices: " << e.what() << std::endl;
        }
    }

    void HttpLogic::processTaskId(const json &jArray, MessageLayer &core, std::string url) {
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

    void HttpLogic::processTask(const json &jArray, MessageLayer &core) const {
        try {
            auto task = jArray["task"];
            bool ok = utility::parseBoolFromJson(task["ok"]);
            int task_id = utility::parseIntFromJson(task["id"]);

            if (!ok) {
                sendTaskToHttp(core, std::to_string(task_id));
            } else {
                if (task_id == m_current_order_task) {
                    // Перелистываем на страницу "операция по карте выполняется".
                    DwinCommands::sendPageToDwin(core, m_settings.dwin.page_processing_fuel_card);
                }
                getDispenserStatus(core, m_current_dispenser_id);
            }
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Task: " << e.what() << std::endl;
        }
    }

    void HttpLogic::fillPageEditingFuelType(std::vector<Product> &products, MessageLayer &core) {
        m_edit_page_row_to_fuel_id.clear();

        size_t max_rows = 4;

        for (size_t i = 0; i < products.size(); ++i) {
            if (i >= max_rows) {
                break;
            }

            Product &p = products[i];

            // Сохраняем ID продукта.
            m_edit_page_row_to_fuel_id.push_back(p.id);


            std::string name_to_show = utility::extractFirstInt(p.id);
            DwinCommands::sendRightAlignmentWithPadding(core,
                                                        m_settings.dwin.vp_text_fuel_price_edit_page[i],
                                                        name_to_show,
                                                        m_settings.dwin.text_len_fuel_type);


            sendFloatToDwin(m_settings.dwin.vp_text_fuel_integer_price_edit_page[i],
                            m_settings.dwin.vp_text_fuel_decimal_price_edit_page[i],
                            core, m_settings.dwin.text_len_fuel_type, m_settings.dwin.text_len_order_decimal, p.price);
        }

        // Добавить очистку других видов топлива, если их стало меньше
    }

    void HttpLogic::fillPageEditingFuelTypeSecondVariable(std::vector<Product> &products, MessageLayer &core) {
        Product &p = products[m_current_fuel_type_index];

        int id_product = utility::getIconForProduct(p.rating, m_settings);
        DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_icon_fuel_type_for_editing, id_product);

        sendFloatToDwin(m_settings.dwin.vp_fuel_price_for_editing_integer,
                        m_settings.dwin.vp_fuel_price_for_editing_decimal,
                        core, m_settings.dwin.text_len_fuel_integer, m_settings.dwin.text_len_order_decimal, p.price);
    }

    void HttpLogic::processProducts(const json &jArray) {
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

    void HttpLogic::processTankers(const json &jArray) {
        try {
            for (const auto &tanker: jArray["tankers"]) {
                std::string tanker_id = utility::parseStringFromJson(tanker["id"]);
                Tanker new_tanker(tanker_id);

                new_tanker.product_id = utility::parseStringFromJson(tanker["product_id"]);
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

    void HttpLogic::setTRKIdOnDisplay(MessageLayer &core, std::string trk_id) const {
        for (uint16_t vp: m_settings.dwin.vp_trk_id_pages) {
            DwinCommands::sendTextToDwin(core, vp, trk_id, m_settings.dwin.text_len_trk_id);
        }
    }

    void HttpLogic::setProductIdOnDisplay(MessageLayer &core, int value) const {
        for (uint16_t vp: m_settings.dwin.vp_product_id_pages) {
            if (vp == 0) continue;
            DwinCommands::sendInt16ToDwin(core, vp, value);
        }
    }

    void HttpLogic::setCurrentOrderAmountOnDisplay(MessageLayer &core, const std::string &value) const {
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

    void HttpLogic::setCurrentOrderVolumeOnDisplay(MessageLayer &core, const std::string &value) const {
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

    void HttpLogic::setFuelTypePriceOnDisplay(MessageLayer &core, const std::string &value) const {
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

    void HttpLogic::setProductTextOnDisplay(MessageLayer &core, const std::string &text) const {
        for (uint16_t vp: m_settings.dwin.vp_product_text_pages) {
            if (vp == 0) continue;
            DwinCommands::sendTextToDwin(core, vp, text, m_settings.dwin.text_len_fuel_type);
        }
    }

    void HttpLogic::setFooterDateTime(MessageLayer &core) {
        std::string current_time = utility::getCurrentTimeString();

        if (current_time != m_last_time_str) {
            m_last_time_str = current_time;

            for (uint16_t vp: m_settings.dwin.vp_current_date_time_pages) {
                if (vp == 0) continue;
                DwinCommands::sendTextToDwin(core, vp, current_time, m_settings.dwin.text_len_date_time_footer);
            }
        }
    }

    void HttpLogic::setLevelGaugeParametersOnDisplay(MessageLayer &core, const LevelGauge &level_gauge) {
        const Tanker *tanker = utility::getTankerByLevelGaugeId(m_tankers, level_gauge.id);
        if (tanker != nullptr) {
            Product *product = utility::getProductById(m_products, tanker->product_id);

            if (product != nullptr) {
                DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_level_gauge_id, utility::extractFirstInt(product->id),
                                             m_settings.dwin.text_len_fuel_integer);
            }
        }

        int filling_percent = utility::ceilStringToInt(level_gauge.filling);
        std::string percent_str = std::to_string(filling_percent);

        DwinCommands::sendRightAlignmentWithPadding(core, m_settings.dwin.vp_text_gauges_filling_percent, percent_str,
                                                    m_settings.dwin.text_len_percent_progress_bar);
        DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_icon_level_gauges_volume, filling_percent);

        sendFloatToDwin(m_settings.dwin.vp_text_upper_level_gauges_integer,
                        m_settings.dwin.vp_text_upper_level_gauges_decimal,
                        core, m_settings.dwin.text_len_order_integer, 1, level_gauge.upper_level);
        sendFloatToDwin(m_settings.dwin.vp_text_upper_volume_gauges_integer,
                        m_settings.dwin.vp_text_upper_volume_gauges_decimal,
                        core, m_settings.dwin.text_len_order_integer, 1, level_gauge.upper_volume);
        sendFloatToDwin(m_settings.dwin.vp_text_weight_gauge_integer, m_settings.dwin.vp_text_weight_gauge_decimal,
                        core, m_settings.dwin.text_len_order_integer, 1, level_gauge.weight);
        sendFloatToDwin(m_settings.dwin.vp_text_density_gauge_integer, m_settings.dwin.vp_text_density_gauge_decimal,
                        core, m_settings.dwin.text_len_order_integer, 1, level_gauge.density);
        sendFloatToDwin(m_settings.dwin.vp_text_lower_level_gauge_integer,
                        m_settings.dwin.vp_text_lower_level_gauge_decimal,
                        core, m_settings.dwin.text_len_order_integer, 1, level_gauge.lower_level);
        sendFloatToDwin(m_settings.dwin.vp_text_lower_volume_gauge_integer,
                        m_settings.dwin.vp_text_lower_volume_gauge_decimal,
                        core, m_settings.dwin.text_len_order_integer, 1, level_gauge.upper_volume);
        sendFloatToDwin(m_settings.dwin.vp_text_total_volume_gauge_integer,
                        m_settings.dwin.vp_text_total_volume_gauge_decimal,
                        core, m_settings.dwin.text_len_order_integer, 1, level_gauge.total_volume);
    }

    void HttpLogic::setCurrentFuelingVolume(MessageLayer &core, const std::string &value) {
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

    void HttpLogic::sendFloatToDwin(uint16_t int_vp, uint16_t dec_vp, MessageLayer &core, int int_length,
                                    int dec_length, std::string value) {
        std::string int_part, dec_part;
        std::tie(int_part, dec_part) = utility::splitFloatString(value, dec_length);

        DwinCommands::sendRightAlignmentWithPadding(core, int_vp, int_part, int_length);
        DwinCommands::sendRightAlignmentWithPadding(core, dec_vp, dec_part, dec_length);
    }

    void HttpLogic::handlePaymentResponse(const Message& msg, MessageLayer& core) {
        std::string order_id = msg.resource_id;
        
        // Parse JSON from payload
        std::string json_str(msg.payload.begin(), msg.payload.end());
        auto json = nlohmann::json::parse(json_str);
        
        bool success = json["success"];
        
        if (success) {
            uint64_t transaction_id = json["transaction_id"];
            std::string message = json["message"];
            
            std::cout << "[HttpLogic] Payment SUCCESS for order " << order_id
                      << ", transaction_id=" << transaction_id << std::endl;
            
            // TODO: Show "Оплата подтверждена" on DWIN
            // TODO: Allow fueling to start
            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_success_processing_fuel_card);
            
        } else {
            std::string error_msg = json["message"];
            
            std::cerr << "[HttpLogic] Payment FAILED for order " << order_id
                      << ", message=" << error_msg << std::endl;
            
            // TODO: Show error on DWIN
            // TODO: Cancel order on Prime
            std::string command = m_settings.APIDispenser.close;
            PrimeCommands::handleCommand(core, command, m_current_dispenser_id);
        }
    }
}
