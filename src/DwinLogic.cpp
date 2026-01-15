//
// Created by vadim.tissen on 04.12.2025.
//

#include "bridge/DwinLogic.hpp"
#include "bridge/utility.hpp"
#include <iostream>

#include "bridge/constant.hpp"
#include "bridge/MessageLayer.hpp"
#include "bridge/types.hpp"
#include "bridge/DwinCommands.hpp"

namespace bridge {
    DwinLogic::DwinLogic(const Settings &settings) : m_settings(settings) {}

    Message DwinLogic::sendAndReceive(const Message& request, MessageLayer& core) {
        std::cout << "[DwinLogic] sendAndReceive pushing request" << std::endl;

        std::promise<Message> promise;
        std::future<Message> future = promise.get_future();

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            // std::move обязателен, promise не копируется
            m_pendingRequests.push(std::move(promise));
        }

        // 3. Отправляем запрос в систему (MessageLayer -> Parser -> Transport)
        core.sendTo(UART_LAYER, request);

        // 4. Ждем ответа (блокируем поток main)
        // Устанавливаем таймаут 2 секунды. Если дисплей не ответит за это время, кидаем ошибку.
        if (future.wait_for(std::chrono::seconds(2)) == std::future_status::timeout) {

            // Если таймаут — нужно бы почистить очередь, но std::queue не дает удалить из середины.
            // В простом варианте мы просто кидаем исключение.
            // (В идеале нужно делать сложнее с ID запросов, но для теста пойдет)
            throw std::runtime_error("Timeout: Display did not respond in 2 seconds");
        }

        // 5. Возвращаем полученный ответ
        return future.get();
    }

    void DwinLogic::handle(const Message& message, MessageLayer& core) {
        std::lock_guard<std::mutex> lock(m_mutex);

        // Проверяем, ждет ли кто-то ответа
        if (!m_pendingRequests.empty()) {

            // Берем первый promise из очереди
            std::promise<Message>& promise = m_pendingRequests.front();

            // Передаем полученное сообщение в ожидающий поток (разблокируем future.wait)
            promise.set_value(message);

            // Удаляем выполненный запрос из очереди
            m_pendingRequests.pop();

        } else {
            // Здесь можно добавить логику обработки кнопок
            if (message.type == "vp_data") {
                handleDwinEvent(message, core);
            }

            std::cout << "[DwinLogic] Unsolicited message from Display (Type: "
                      << message.type << ")" << std::endl;

            // Пример: если нажата кнопка, отправить на HTTP (будущая реализация)
            // if (incomingMsg.type == "button_click") core.sendTo("HTTP", ...);
        }
    }

    // Обработка нажатий на дисплей.
    void DwinLogic::handleDwinEvent(const Message &message, MessageLayer &core) {
        // Проверка длины: VP(2) + Count(1) + Data(2 минимум) = 5 байт
        if (message.payload.size() < 5) return;

        uint16_t vp = (message.payload[0] << 8) | message.payload[1];

        // По литрам.
        if (vp == m_settings.dwin.vp_fuel_volume) {
            handleFuelVolume(message, core);
            handleAcceptOrder(core);

            uint8_t page_id = m_settings.dwin.page_fuel_progress;
            DwinCommands::sendPageToDwin(core, page_id);
            //change page
        }
        // Загрузка страницы текущего заказа. ParseEvents сам разберет обновление состояния заказа.
        // page_id = m_settings.dwin.page_fuel_progress;
        // sendPageToDwin(core, page_id);
    }

    void DwinLogic::handleFuelVolume(const Message &message, MessageLayer& core) {
        uint16_t volume = (message.payload[3] << 8) | message.payload[4];

        json json_object;
        json_object["price"] = {
            { "value", TRK_FUEL_PRICE },
            { "exponent", 0 }
        };

        json_object["type"] = FUEL_VOLUME_ORDER_TYPE;

        json_object["value"] = {
            { "value", volume },
            { "exponent", 0}
        };

        std::string json_string = json_object.dump();

        Message msg;
        msg.source = UART_LAYER;
        msg.type = CREATE_ORDER;
        msg.resource_id = TRK_ID;
        msg.payload.assign(json_string.begin(), json_string.end());

        core.sendTo(HTTP_LAYER, msg);
    }

    void DwinLogic::handleAcceptOrder(MessageLayer& core) {
        json json_object;
        json_object["command"] =  m_settings.APIDispenser.authorize;
        std::string json_string = json_object.dump();

        Message msg;
        msg.source = UART_LAYER;
        msg.type = SET_COMMAND;
        msg.resource_id = TRK_ID;
        msg.payload.assign(json_string.begin(), json_string.end());

        core.sendTo(HTTP_LAYER, msg);
    }
}
