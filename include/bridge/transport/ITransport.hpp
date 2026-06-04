//
// Created by vadim.tissen on 18.11.2025.
//

#pragma once

#include <functional>
#include <memory>
#include <string>

//ITransport.hpp
namespace bridge {
    struct RawData;

    // Интерфейс транспорта — абстрагирует UART, WS, HTTP, PIPE
    class ITransport {
    public:
        using ReceiveHandler = std::function<void(const RawData& raw_data)>; // При регистрации канала определяется функция

        virtual ~ITransport() = default;

        // Запустить транспорт (открыть порт/соединение, начать чтение)
        virtual void start() = 0;

        // Остановить транспорт (закрыть порт/соединение)
        virtual void stop() = 0;

        // Отправить сырые байты
        virtual void send(const RawData& raw_data) = 0;

        // Установить callback, который будет вызван при поступлении RawData.
        virtual void setReceiveHandler(ReceiveHandler handler) = 0;
    };

    using ITransportPtr = std::shared_ptr<ITransport>;
}
