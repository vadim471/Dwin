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

    void HttpLogic::getProducts(MessageLayer &core) {
        Message request;
        request.type = GET_PRODUCTS;

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::getDispenserStatus(MessageLayer &core, const std::string &trk_id) {
        Message request;
        request.type = GET_DISPENSER_STATUS;
        request.resource_id = trk_id;

        core.sendTo(HTTP_LAYER, request);
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

        // Перезавод таймера
        scheduleNextTick(core);
    }

    // Обработка входящего от сервера ответа. В паре с Parser.parse
    void HttpLogic::handle(const Message &message, MessageLayer &core) {
        m_waitingResponse.store(false);
        if (message.type == HTTP_RESPONSE) {
            try {
                std::string jsonStr(message.payload.begin(), message.payload.end());
                auto json = json::parse(jsonStr);
                if (json.is_object()) {
                    processHandleHttp(json, core, message.url);
                }
            } catch (std::exception &e) {
                std::cerr << "[HTTP Logic] Json parse error: " << e.what() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        } else if (message.type == USER_TOUCH) {
            processHandleUserTouch(message, core);
        }
    }

    void HttpLogic::processHandleUserTouch(const Message &message, MessageLayer &core) {
        if (message.payload.size() < 2) return;
        uint16_t vp = (message.payload[0] << 8) | message.payload[1];
        if (vp == m_settings.dwin.vp_pagination_trk) {
            int8_t direction = static_cast<int8_t>(message.payload[2]);
            changeDispenser(direction, core);
            // Выбрали ТРК.
        } else if (vp == m_settings.dwin.vp_choose_trk) {
            auto trk = m_dispensers[m_current_dispenser_index];
            m_current_dispenser_id = trk.id;
            if (trk.status == DISPENSER_IDLE) {
                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_set_nozzle_into_gasoline);
                m_is_idle_timer_running = true;
                m_idle_timer_start = std::chrono::steady_clock::now();

            } else if (trk.status == DISPENSER_NOZZLE_UP) {
                m_is_idle_timer_running = false;

                const Product *product = utility::getProductById(m_products, trk.product_id);

                int icon_fuel = utility::getIconForProduct(product->rating, m_settings);
                setProductIdOnDisplay(core, icon_fuel);
                setFuelTypePriceOnDisplay(core, product->price);
                setProductTextOnDisplay(core, utility::extractFirstInt(product->id));

                DwinCommands::sendPageToDwin(core, m_settings.dwin.page_put_card_or_scan_code);
            }

            setTRKIdOnDisplay(core, utility::extractFirstInt(trk.id));
        } else if (vp == m_settings.dwin.vp_chosen_order_volume) {
            uint16_t volume = (message.payload[2] << 8) | message.payload[3];
            createOrder(core, volume, 0);
        } else if (vp == m_settings.dwin.vp_basic_touch) {
            uint16_t value = (message.payload[2] << 8) | message.payload[3];
            if (value == 10) {
                std::string command = m_settings.APIDispenser.close;
                PrimeCommands::handleCommand(core, command, m_current_dispenser_id);
            }
            getDispenserStatus(core, m_current_dispenser_id);
        } else if (vp == m_settings.dwin.vp_next_button) {
            m_dwin_button_next_for_start_fuel = true;
            authorizeOnServer(core);
        } else if (vp == m_settings.dwin.vp_pinpad) {
            handlePinpadButton(message, core);
        }
    }

    void HttpLogic::checkIdleTimeout(MessageLayer &core) {
        if (!m_is_idle_timer_running) return;

        // Считаем сколько прошло времени
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_idle_timer_start).count();

        if (elapsed >= m_settings.business_logic.sleep_after_chosen_trk_page) {

            DwinCommands::sendPageToDwin(core, m_settings.dwin.page_choose_trk);
            m_current_dispenser_id.clear();

            m_is_idle_timer_running = false;
        }
    }

    void HttpLogic::handlePinpadButton(const Message &message, MessageLayer &core) {
        uint16_t value = (message.payload[2] << 8) | message.payload[3];
        int number = static_cast<int>(value);

        if (number >= 0 && number <= 9 ) {
            size_t dot_position = m_pinpad_buffer.find(",");
            if (dot_position != std::string::npos) {
                if (m_pinpad_buffer.length() - dot_position > 2) {
                    return;
                }
            }

            if (m_pinpad_buffer.length() < 6) {
                // Если ввели '0' первым символом, и вводят еще цифру -> заменяем (чтобы не было 05).
                if (m_pinpad_buffer == "0") {
                    m_pinpad_buffer = std::to_string(number);
                } else {
                    m_pinpad_buffer += std::to_string(number);
                }
            }
            // Dot.
        } else if (number == 10) {
            if (m_pinpad_buffer.find(',') == std::string::npos) {
                if (m_pinpad_buffer.empty()) {
                    m_pinpad_buffer = "0,"; // Если нажали точку сразу, делаем "0."
                } else {
                    m_pinpad_buffer += ",";
                }
            }
            // Decline.
        } else if (number == 11) {
            m_pinpad_buffer.clear();
            // Backspace.
        } else if (number == 12) {
            if (!m_pinpad_buffer.empty()) {
                m_pinpad_buffer.pop_back();
            }
            // Enter.
        } else if (number == 13) {
            if (!m_pinpad_buffer.empty()) {
                int value = 0;
                int exponent = 0;

                size_t dot_position = m_pinpad_buffer.find(",");
                if (dot_position != std::string::npos) {
                    exponent = m_pinpad_buffer.length() - dot_position - 1;

                    std::string string_value = m_pinpad_buffer;
                    string_value.erase(dot_position, 1);
                    try {
                        value = std::stol(string_value);
                    } catch (std::exception &e) {
                        value = 0;
                    }
                } else {
                    try {
                        value = std::stol(m_pinpad_buffer);
                    } catch (std::exception &e) {
                        value = 0;
                    }
                }
                createOrder(core, value, exponent);
                m_pinpad_buffer.clear();
                DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_enterring_volume_order_pinpad, "0", m_settings.dwin.text_len_order_volume);
            }
            return;
        }
        std::string textToShow = m_pinpad_buffer.empty() ? "0" : m_pinpad_buffer;

        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_enterring_volume_order_pinpad, textToShow, m_settings.dwin.text_len_order_volume);
    }

    void HttpLogic::createOrder(MessageLayer &core, int value, int exponent) {
        json json_object;
        json_object["price"] = {
            {"value", TRK_FUEL_PRICE},
            {"exponent", 2}
        };

        json_object["type"] = FUEL_VOLUME_ORDER_TYPE;

        json_object["value"] = {
            {"value", value},
            {"exponent", exponent}
        };

        std::string json_string = json_object.dump();

        Message request;
        request.resource_id = m_current_dispenser_id;
        request.type = CREATE_ORDER;
        request.payload.assign(json_string.begin(), json_string.end());

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::changeDispenser(int direction, MessageLayer &core) {
        if (m_dispensers.empty()) {
            std::cout << "No TRK in map " << std::endl;
            return;
        }
        m_current_dispenser_index += direction;

        if (m_current_dispenser_index >= (int) m_dispensers.size()) {
            m_current_dispenser_index = 0;
        } else if (m_current_dispenser_index < 0) {
            m_current_dispenser_index = (int) m_dispensers.size() - 1;
        }

        const auto &trk = m_dispensers[m_current_dispenser_index];
        getDispenserStatus(core, trk.id);

        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_trk_id_first_page, std::to_string(m_current_dispenser_index + 1), m_settings.dwin.text_len_trk_id);
        renderDispenser(core);
    }

    // Распределяет ответ JSON от сервера по парсерам.
    void HttpLogic::processHandleHttp(const json &jArray, MessageLayer &core, const std::string &url) {
        if (jArray.contains("events")) {
            processEvent(jArray, core);
        } else if (jArray.contains("parameters")) {
            processParameters(jArray, core);
        } else if (jArray.contains("products")) {
            processProducts(jArray);
        } else if (jArray.contains("status")) {
            processDispenserStatus(jArray, core, url);
        } else if (jArray.contains("task_id")) {
            processTaskId(jArray, core);
        } else if (jArray.contains("task")) {
            processTask(jArray, core);
        } else if (jArray.contains("devices")) {
            processDevices(jArray, core);
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

                    handleDispenserOrderCreated(core);
                } else if (event.contains("on_dsp_order_changed")) {
                    //handleDispenserOrderChanged(event, core);
                }
            } else {
                if (event.contains("on_dsp_status_changed")) {
                    handleRandomDispenserStatus(event, core);
                }
            }
        }
        setFooterDateTime(core);
        checkIdleTimeout(core);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    void HttpLogic::handleRandomDispenserStatus(const json& event, MessageLayer& core) {
        try {
            auto data = event["on_dsp_status_changed"];
            std::string status = data.value("status", "");
            std::string dispenser_id = utility::parseStringFromJson(event["device_id"]);

            Dispenser *dispenser = utility::getDispenserById(m_dispensers, dispenser_id);
            dispenser->status = status;

            if (m_dispensers[m_current_dispenser_index].status == dispenser->status) {
                renderDispenser(core);
            }

        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Random Dispenser Status: " << e.what() << std::endl;
        }
    }

    // void HttpLogic::handleDispenserOrderChanged(const json& jArray, MessageLayer &core) const {
    //     try {
    //         // std::string trk_id = jArray["device_id"];
    //         auto data = jArray["on_dsp_order_changed"];
    //
    //         float amount = utility::parseFloatFromJson(data["amount"]);
    //         float volume = utility::parseFloatFromJson(data["volume"]);
    //
    //         DwinCommands::sendFloatToDwin(core, m_settings.dwin.vp_current_order_amount, amount);
    //         DwinCommands::sendFloatToDwin(core, m_settings.dwin.vp_current_fuel_volume, volume);
    //         DwinCommands::sendFloatToDwin(core, m_settings.dwin.vp_current_price_per_liter_last_order, 11);
    //
    //     } catch (const std::exception& e) {
    //         std::cerr << "[Logic] Error Dispenser Order Changed: " << e.what() << std::endl;
    //     }
    // }

    void HttpLogic::handleDispenserOrderCreated(MessageLayer &core) {
        m_is_server_ready_for_start_fuel = true;
        authorizeOnServer(core);
    }

    void HttpLogic::handleDispenserDisplay(const json &event, MessageLayer &core) {
        try {
            std::string trk_id = event["device_id"];
            auto data = event["on_dsp_display_changed"];

            // float amount = utility::parseFloatFromJson(data["amount"]);
            // float volume = utility::parseFloatFromJson(data["volume"]);
            // float price = utility::parseFloatFromJson(data["price"]);

            auto amount = data["amount"];
            std::string amount_string = utility::getFormattedStringFromJson(
                utility::parseStringFromJson(amount["value"]), utility::parseIntFromJson(amount["exponent"]));

            auto volume = data["volume"];
            std::string volume_string = utility::getFormattedStringFromJson(
                utility::parseStringFromJson(volume["value"]), utility::parseIntFromJson(volume["exponent"]));

            DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_current_order_amount, amount_string, amount_string.length());
            DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_current_fuel_volume, volume_string, volume_string.length());
            // DwinCommands::sendFloatToDwin(core, m_settings.dwin.vp_current_price_amount, price);

            std::string current_volume_order_string;
            Dispenser *foundTrk = utility::getDispenserById(m_dispensers, trk_id);

            if (foundTrk != nullptr) {
                current_volume_order_string = foundTrk->order.volume;
            }

            int percent = 0;

            if (!volume_string.empty() && !current_volume_order_string.empty()) {
                double current  = std::stod(volume_string);
                double total    = std::stod(current_volume_order_string);

                if (total > 0.0) {
                    percent = static_cast<int>((current / total) * 100.0);
                }
            }
            std::cout << "Percent to bar" << percent << std::endl;
            std::string percent_str = std::to_string(percent) + "%";

            //DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_progress_order_bar_eleventh_page, percent);
            DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_progress_order_bar_eleventh_page, percent);
            DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_progress_bar_percent_text_eleventh_page, percent_str, percent_str.length());
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Dispenser Display: " << e.what() << std::endl;
        }
    }


    //{
    //  "id": 30,
    //  "on_dsp_status_changed": {
    //      "status": "DSP_NOZZLE_UP",
    //      "nozzle": {
    //          "number": 1,
    //          "product_id": "ai92"
    //         }
    //  },
    //  "device_id": "trk1"
    // }

    void HttpLogic::handleDispenserStatus(const json &event, MessageLayer &core) {
        auto data = event["on_dsp_status_changed"];

        auto status = data.value("status", "");
        auto nozzle = data["nozzle"];
        std::string product_id = utility::parseStringFromJson(nozzle["product_id"]);
        auto number = nozzle["number"];

        // Обработка налива топлива.
        if (status == DISPENSER_NOZZLE_UP) {
            // Выключаем таймер ожидания снятия пистолета.
            m_is_idle_timer_running = false;

            // Показываем страницу "Вставьте/приложите карту".
            uint8_t page_id = m_settings.dwin.page_put_card_or_scan_code;
            DwinCommands::sendPageToDwin(core, page_id);

            // Передача текста (название топлива)
            const Product *product = utility::getProductById(m_products, product_id);
            int icon_fuel = utility::getIconForProduct(product->rating, m_settings);
            setProductIdOnDisplay(core, icon_fuel);
            setFuelTypePriceOnDisplay(core, product->price);
            setProductTextOnDisplay(core, utility::extractFirstInt(product->id));
        } else if (status == DISPENSER_IDLE) {
            m_flowState = DispenserFlowState::IDLE;
            uint8_t page_id = m_settings.dwin.page_set_nozzle_into_gasoline;
            DwinCommands::sendPageToDwin(core, page_id);
        } else if (status == DISPENSER_COMPLETE) {
            uint8_t page_id = m_settings.dwin.page_fuel_ended;
            DwinCommands::sendPageToDwin(core, page_id);
            DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_progress_order_bar_twelvth_page, 100);
            DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_progress_bar_percent_text_twelvth_page, "100%", 4);
        }
    }

    void HttpLogic::renderDispenser(MessageLayer &core) {
        if (m_dispensers.empty()) return;

        const auto &trk = m_dispensers[m_current_dispenser_index];
        int icon_id = utility::getIconForStatus(trk.status, m_settings);
        std::string trk_id = utility::extractFirstInt(trk.id);
        DwinCommands::sendInt16ToDwin(core, m_settings.dwin.vp_icon_trk, icon_id);
        DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_text_trk, trk_id, m_settings.dwin.text_len_trk_id);
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

            DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_volume_last_order, amount_string,
                                         20);
            DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_price_last_order, volume_string,
                                         20);
            DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_current_price_per_liter_last_order, price_string,
                                         20);

            bool is_current_trk_updated = false;

            for (size_t i = 0; i < m_dispensers.size(); i++) {
                if (m_dispensers[i].id == trk_id) {
                    m_dispensers[i].status = status;

                    if (jObj.contains("nozzle")) {
                        auto nozzle = jObj["nozzle"];
                        std::string nozzle_string = utility::parseStringFromJson(nozzle["product_id"]);
                        m_dispensers[i].product_id = nozzle_string;
                    }

                    if (jObj.contains("order")) {
                        auto order = jObj["order"];

                        auto target_amount = order["target_amount"];
                        std::string target_amount_string = utility::getFormattedStringFromJson(
                            utility::parseStringFromJson(target_amount["value"]),
                            utility::parseIntFromJson(target_amount["exponent"]));

                        auto target_volume = order["target_volume"];
                        std::string target_volume_string = utility::getFormattedStringFromJson(
                            utility::parseStringFromJson(target_volume["value"]),
                            utility::parseIntFromJson(target_volume["exponent"]));

                        m_dispensers[i].order.volume = target_volume_string;
                        m_dispensers[i].order.amount = target_amount_string;
                        m_dispensers[i].order.order_id = utility::parseStringFromJson(order["id"]);


                        setCurrentOrderAmountOnDisplay(core, m_dispensers[i].order.amount);
                        setCurrentOrderVolumeOnDisplay(core, m_dispensers[i].order.volume);
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

    void HttpLogic::processParameters(const json &jArray, MessageLayer &core) {
        try {
            for (const auto &parameter: jArray["parameters"]) {
                // TODO
            }
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Parameters: " << e.what() << std::endl;
        }
    }

    void HttpLogic::processDevices(const json &jArray, MessageLayer &core) {
        m_dispensers.clear();
        try {
            for (const auto &device: jArray["devices"]) {
                if (device["api"] == API_DISPENSER) {
                    std::string trk_id = device["id"];
                    Dispenser new_dispenser_model(trk_id);
                    m_dispensers.push_back(new_dispenser_model);
                    getDispenserStatus(core, trk_id);
                }
            }
            renderDispenser(core);
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Devices: " << e.what() << std::endl;
        }
    }

    void HttpLogic::processTaskId(const json &jArray, MessageLayer &core) {
        try {
            int task_id = utility::parseIntFromJson(jArray["task_id"]);
            sendTaskToHttp(core, std::to_string(task_id));
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing TaskId: " << e.what() << std::endl;
        }
    }

    void HttpLogic::processTask(const json &jArray, MessageLayer &core) {
        try {
            auto task = jArray["task"];
            bool ok = utility::parseBoolFromJson(task["ok"]);
            int task_id = utility::parseIntFromJson(task["id"]);

            if (!ok) {
                sendTaskToHttp(core, std::to_string(task_id));
            } else {
                getDispenserStatus(core, m_current_dispenser_id);
            }
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Task: " << e.what() << std::endl;
        }
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
                new_product.price = "54,67";
                m_products.push_back(new_product);
            }
        } catch (const std::exception &e) {
            std::cerr << "[Logic] Error parsing Products: " << e.what() << std::endl;
        }
    }

    void HttpLogic::sendTaskToHttp(MessageLayer &core, std::string id) {
        Message request;
        request.type = GET_TASKS;
        request.resource_id = id;

        core.sendTo(HTTP_LAYER, request);
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
        for (uint16_t vp: m_settings.dwin.vp_current_order_amount_pages) {
            if (vp == 0) continue;
            DwinCommands::sendTextToDwin(core, vp, value, value.length());
        }
    }

    void HttpLogic::setCurrentOrderVolumeOnDisplay(MessageLayer &core, const std::string &value) const {
        for (uint16_t vp: m_settings.dwin.vp_current_order_volume_pages) {
            if (vp == 0) continue;
            DwinCommands::sendTextToDwin(core, vp, value, value.length());
        }
    }

    void HttpLogic::setFuelTypePriceOnDisplay(MessageLayer &core, const std::string &value) const {
        for (uint16_t vp : m_settings.dwin.vp_current_fuel_type_price_pages) {
            if (vp == 0) continue;
            DwinCommands::sendTextToDwin(core, vp, value, value.length());
        }
    }

    void HttpLogic::setProductTextOnDisplay(MessageLayer &core, const std::string &text) const {
        for (uint16_t vp : m_settings.dwin.vp_product_text_pages) {
            if (vp == 0) continue;
            DwinCommands::sendTextToDwin(core, vp, text, 3);
        }
    }

    void HttpLogic::setFooterDateTime(MessageLayer &core) {
        std::string current_time = utility::getCurrentTimeString();

        // 2. Проверяем, изменилось ли время (прошла ли секунда)
        if (current_time != m_last_time_str) {
            m_last_time_str = current_time;

            // 3. Отправляем на дисплей
            // Убедись, что в Settings есть адрес для поля времени (например, vp_system_time)
            // Длина строки "dd.mm.yyyy hh:mm:ss" = 19 символов.

            for (uint16_t vp : m_settings.dwin.vp_current_date_time_pages) {
                if (vp == 0) continue;
                DwinCommands::sendTextToDwin(core, vp, current_time, m_settings.dwin.text_len_date_time_footer);
            }

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
}
