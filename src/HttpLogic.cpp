//
// Created by vadim.tissen on 12.12.2025.
//

#include "bridge/HttpLogic.hpp"
#include "bridge/MessageLayer.hpp"
#include <iostream>
#include "bridge/utility.hpp"
#include "bridge/constant.hpp"
#include "bridge/DwinCommands.hpp"


// Для формирования запросов на дисплей. Тут я разбираю ответ от сервера и составляю Message для дисплея.
namespace bridge {

    HttpLogic::HttpLogic(const Settings& settings) :
    m_lastId(0), m_settings(settings) {
    }

    void HttpLogic::getDevices(MessageLayer& core) {
        Message request;
        request.type = GET_DEVICES;

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::getParameters(MessageLayer& core, std::string& id) {
        Message request;
        request.type = GET_IO_PARAMETERS;
        request.resource_id = id;

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::getProducts(MessageLayer& core) {
        Message request;
        request.type = GET_PRODUCTS;

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::getDispenserStatus(MessageLayer &core) {
        Message request;
        request.type = GET_DISPENSER_STATUS;
        request.resource_id = TRK_ID;

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::startPolling(MessageLayer &core) const {
        Message request;
        request.type = GET_EVENTS;
        request.resource_id = std::to_string(m_lastId);

        core.sendTo(HTTP_LAYER, request);
    }

    void HttpLogic::tick(MessageLayer &core) {
        auto now = std::chrono::steady_clock::now();

        // Если прошло > 1 сек с последнего запроса И мы не ждем ответа прямо сейчас
        if (!m_waitingForHttpResponse && (now - m_lastPollTime > std::chrono::seconds(1))) {
            startPolling(core);
            m_lastPollTime = now;
            m_waitingForHttpResponse = true;
        }
    }

    // Обработка входящего от сервера ответа. В паре с Parser.parse
    void HttpLogic::handle(const Message& message, MessageLayer& core) {
        m_waitingForHttpResponse = false;
        try {
            std::string jsonStr(message.payload.begin(), message.payload.end());
            auto json = json::parse(jsonStr);
            if (json.is_object()) {
                processHandle(json, core);
            }
        } catch (std::exception &e) {
            std::cerr << "[HTTP Logic] Json parse error: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    // Распределяет ответ JSON от сервера по парсерам.
    void HttpLogic::processHandle(const json &jArray, MessageLayer &core) {
        if (jArray.contains("events")) {
            processEvent(jArray, core);
        } else if (jArray.contains("parameters")) {
            processParameters(jArray, core);
        } else if (jArray.contains("products")) {
            processProducts(jArray);
        } else if (jArray.contains("status")) {
            processDispenserStatus(jArray, core);
        } else if (jArray.contains("task_id")) {
            processTaskId(jArray, core);
        } else if (jArray.contains("task")) {
            processTask(jArray, core);
        }
        // startPolling(core);
    }

    void HttpLogic::processEvent(const json &jArray, MessageLayer &core) {
        for (const auto &event: jArray["events"]) {
            if (event.contains("id")) {
                long long currentId = event["id"];
                if (currentId) {
                    m_lastId = currentId;
                }
            }

            if (event.contains("on_dsp_status_changed")) {
                handleDispenserStatus(event, core);

            } else if (event.contains("on_dsp_display_changed")) {
                hanldeDispenserDisplay(event, core);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    void HttpLogic::hanldeDispenserDisplay(const json& event, MessageLayer &core) const {
        try {
            auto data = event["on_dsp_display_changed"];

            float amount = utility::parseFloatFromJson(data["amount"]);
            float volume = utility::parseFloatFromJson(data["volume"]);
            float price  = utility::parseFloatFromJson(data["price"]);

            DwinCommands::sendFloatToDwin(core, m_settings.dwin.vp_current_order_amount, amount);
            DwinCommands::sendFloatToDwin(core, m_settings.dwin.vp_current_fuel_volume, volume);
            DwinCommands::sendFloatToDwin(core, m_settings.dwin.vp_current_price_amount, price);

            int percent = 0;
            percent = static_cast<int>((volume / m_current_order.volume) * 100.0f);

            // Заполнять прогресс бар состояния налива.
            sendInt16ToDwin(core, m_settings.dwin.vp_progress_order_bar, percent);
        } catch (const std::exception& e) {
            std::cerr << "[Logic] Error parsing Dispenser Display: " << e.what() << std::endl;
        }
    }

    void HttpLogic::handleDispenserStatus(const json& event, MessageLayer &core) const {
        auto data = event["on_dsp_status_changed"];
        //std::cout << "[HttpLogic] Found Pistol Up event! ID: " << event["id"] << std::endl;

        auto status = data.value("status", "");
        auto nozzle = data["nozzle"];
        std::string product_id = utility::parseStringFromJson(nozzle["product_id"]);
        auto number = nozzle["number"];

        // Обработка налива топлива.
        if (status == DISPENSER_PISTOL_UP) {
            // Загрузка страницы выбранного топлива.
            uint8_t page_id = m_settings.dwin.page_chosen_fuel;
            DwinCommands::sendPageToDwin(core, page_id);

            // Передача информации о выбранном топливе.
            DwinCommands::sendTextToDwin(core, m_settings.dwin.vp_fuel_type, product_id, FUEL_TYPE_LEN);

            // Сон на х секунд (бизнес-логика) и страница с выбором количества топлива.


            // Загрузка страницы выбора объема топлива. После нажатия кнопки объема отправляется запрос на заказ.
            page_id = m_settings.dwin.page_set_fuel_volume_order;
            DwinCommands::sendPageToDwin(core, page_id);
        }

        else if (status == DISPENSER_IDLE) {
            uint8_t page_id = m_settings.dwin.page_main_order;
            DwinCommands::sendPageToDwin(core, page_id);
        }
    }

    void HttpLogic::processDispenserStatus(const json &jObj, MessageLayer &core) {
        try {
            float amount = utility::parseFloatFromJson(jObj["amount"]);
            float volume = utility::parseFloatFromJson(jObj["volume"]);
            float price  = utility::parseFloatFromJson(jObj["price"]);

            m_current_order.volume = volume;
            m_current_order.amount = amount;

            DwinCommands::sendFloatToDwin(core, m_settings.dwin.vp_sum_order, amount);
            DwinCommands::sendFloatToDwin(core, m_settings.dwin.vp_volume_order, volume);
            DwinCommands::sendFloatToDwin(core, m_settings.dwin.vp_price_order, price);

            std::cout << "[Logic] Updated Dispenser Status: A=" << amount << " V=" << volume << " P=" << price << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[Logic] Error parsing Dispenser Status: " << e.what() << std::endl;
        }
    }

    void HttpLogic::processParameters(const json &jArray, MessageLayer &core) {
        try {
            for (const auto& parameter : jArray["parameters"]) {
                // TODO
            }
        } catch (const std::exception& e) {
            std::cerr << "[Logic] Error parsing Parameters: " << e.what() << std::endl;
        }
    }

    void HttpLogic::processDevices(const json &jArray, MessageLayer &core) {
        // TODO
    }

    void HttpLogic::processTaskId(const json &jArray, MessageLayer &core) {
        try {
            int task_id = utility::parseIntFromJson(jArray["task_id"]);

            sendTaskToHttp(core, task_id);
        } catch (const std::exception& e) {
            std::cerr << "[Logic] Error parsing TaskId: " << e.what() << std::endl;
        }
    }

    void HttpLogic::processTask(const json &jArray, MessageLayer &core) {
        try {
            auto task = jArray["task"];
            bool ok = utility::parseBoolFromJson(jArray["ok"]);
            int task_id = utility::parseIntFromJson(jArray["id"]);

            if (!ok) {
                sendTaskToHttp(core, task_id);
            } else {
                getDispenserStatus(core);
            }
        } catch (const std::exception& e) {
            std::cerr << "[Logic] Error parsing Task: " << e.what() << std::endl;
        }
    }

    // Fill fuel map
    void HttpLogic::processProducts(const json &jArray) {
        try {
            for (const auto& product : jArray["products"]) {
                std::string fuel_id = utility::parseStringFromJson(product["id"]);

                std::string fuel_title;
                if (product.contains("title")) {
                    fuel_title = utility::parseStringFromJson(product["title"]);
                } else if (product.contains("rating")) {
                    fuel_title = utility::parseStringFromJson(product["rating"]);
                } else {
                    fuel_title = fuel_id;
                }

                m_product_names[fuel_id] = fuel_title;
            }
        } catch (const std::exception& e) {
            std::cerr << "[Logic] Error parsing Products: " << e.what() << std::endl;
        }
    }

    void HttpLogic::sendTaskToHttp(MessageLayer &core, int id) {
        Message request;
        request.type = GET_TASKS;
        request.id = id;

        core.sendTo(HTTP_LAYER, request);
    }
}
