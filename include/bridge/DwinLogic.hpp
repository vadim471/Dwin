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

        // Пагинация. Отправляю запрос для повторого опроса состояний ТРК и отображения следующей "страницы".
        static void handlePaginationButton(const Message &message, MessageLayer& core, const std::string message_type);

        // Обработка нажатия с отправкой key code.
        static void handleTouchWithKeyCode(const Message &message, MessageLayer& core);

        // Обработка нажатия с отправкой только VP адреса
        static void handleTouchWithoutKeyCode(const Message &message, MessageLayer& core);

        Settings m_settings;

        // Хендлер создания заказа на пролив.
        void handleCreateOrder(const Message &message, MessageLayer& core);

        // Хендлер выбора ТРК при конфигурации мульти ТРК.
        void handleTouchMultiTRK(MessageLayer& core);

        // Хендлер выбора ТРК при конфигурации 1/2 ТРК. Отправляет индекс выбранной ТРК.
        void handleTouchTRK(const Message &message, MessageLayer& core);

        // Хендлер обработки нажатия на кнопки BaseTouch.
        void handleBasicTouch(const Message &message, MessageLayer& core);

        // Хендлер обработки нажатий на пинпад.
        void handleTouchPinPad(const Message &message, MessageLayer& core);

        // Хендлер обработки нажатия сервисного меню
        void handleServiceMenu(const Message &message, MessageLayer& core);

        // Хендлер обработки нажатия выбора количества ТРК.
        void handleAmountTRK(const Message &message, MessageLayer& core);

        // Хендлер выбора топлива для редактирования цены.
        void handleFuelTypeEditing(MessageLayer& core);

        // Хендлер выбора ТРК страницы выбора используемых ТРК.
        void handleSelectedTRK(const Message &message, MessageLayer& core);

        // Строка, содержащая значение, введенное с пинпада.
        std::string d_pinpad_buffer;
    };

    using DwinLogicPtr = std::shared_ptr<DwinLogic>;
}
