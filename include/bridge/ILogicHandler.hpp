//
// Created by vadim.tissen on 02.12.2025.
//

#pragma once

// ILogicHandler.hpp
#include <memory>
#include "types.hpp"

namespace bridge {

    // Класс, чтобы обработчик мог отправлять ответы.
    class MessageLayer;

    class ILogicHandler {
    public:
        virtual ~ILogicHandler() = default;

        // Метод обработки. Принимает сообщение и ссылку на ядро, чтобы отправить ответ.
        virtual void handle(const Message& msg, MessageLayer& core) = 0;
    };

    using ILogicHandlerPtr = std::shared_ptr<ILogicHandler>;
}