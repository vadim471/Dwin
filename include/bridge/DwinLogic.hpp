//
// Created by vadim.tissen on 04.12.2025.
//

#pragma once
#include <future>

#include "ILogicHandler.hpp"
#include "MessageLayer.hpp"
#include "Settings.hpp"

namespace bridge {
    class DwinLogic : public ILogicHandler {
    public:
        // virtual ~DwinLogic() = default;
        DwinLogic(const Settings& settings);

        // ILogicHandler
        void handle(const Message& message, MessageLayer& core) override;
    private:

        void handleDwinEvent(const Message& message, MessageLayer& core);

        // Обработки нажатия на кнопку объёма заказа.
        static void handleFuelVolume(const Message &message, MessageLayer& core);

        // Пагинация. Отправляю запрос для повторого опроса состояний ТРК и отображения следующей "страницы".
        static void handlePaginationButton(const Message &message, MessageLayer& core);

        // Выбор ТРК. Запрос для обработки нажатия на иконку с ТРК.
        static void handleChosenTrk(const Message &message, MessageLayer& core);

        // Обработка нажатия на кнопку "Назад" или "Отмена".
        static void handleBasicTouchButton(const Message &message, MessageLayer& core);

        // Обработка нажатия на кнопку "Далее".
        static void handleNextButton(const Message &message, MessageLayer& core);

        // Обработка нажатия на ПинПад.
        static void handlePinpadButton(const Message &message, MessageLayer& core);

        Settings m_settings;
    };

    using DwinLogicPtr = std::shared_ptr<DwinLogic>;
}
