//
// Created by vadim.tissen on 04.12.2025.
//

#pragma once

#include "ILogicHandler.hpp"
#include "bridge/core/Settings.hpp"
#include "bridge/core/types.hpp"
#include <itp/logger.hpp>

namespace bridge {
    class DwinLogic : public ILogicHandler {
    public:
        // virtual ~DwinLogic() = default;
        DwinLogic(const Settings& settings);

        // ILogicHandler
        void handle(const Message& message, MessageLayer& core) override;

        // Инициализация ITP Logger
        void setItpLogger(itp::logger::ptr logger) { m_itp_logger = logger; }

    private:

        // Вывод текущей суммы заказа на всех страницах интерфейса.
        void setCurrentOrderAmountOnDisplay(MessageLayer& core, const std::string &value) const;

        // Вывод текущего объема заказа на всех страницах интерфейса.
        void setCurrentOrderVolumeOnDisplay(MessageLayer& core, const std::string &value) const;

        // Заполнение текущего объема пролива.
        void setCurrentFuellingVolume(MessageLayer& core, const std::string &value);

        // Заполнение текущей суммы пролива.
        void setCurrentFuellingAmount(MessageLayer& core, const std::string &value);

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

        // Хендлер обработки отмены заказа.
        void handleCancelTransaction(const Message &message, MessageLayer& core);

        // Хендлер обработки нажатия на кнопку получения баланса карты.
        void handleGetCardBalance(MessageLayer& core);

        // Хендлер обработки нажатия на завершение приема топлива
        void handleFinishReceptionFuel(MessageLayer& core);

        // Хендлер обработки ответа на запрос баланса карты.
        void handleGetBalanceResponse(const Message &message, MessageLayer& core) const;

        // Хендлер обработки нажатия на кнопку выбора резервуара для приема топлива.
        void handleTouchReceptionTanker(MessageLayer& core);

        // Хендлер очистки всех инпутов пин-пада.
        void clearAllInputPinPad(MessageLayer& core) const;

        // Отправка ID терминала и версии на все страницы (один раз при инициализации)
        void sendStandaloneIdAndVersion(MessageLayer& core);

        // Функция очистки инпутов полей объема и стоимости.
        void clearInputAmountAndVolumeText(MessageLayer& core);

        void setStandaloneIdAndVersion(MessageLayer& core) const;

        // Функция отправки тысяч, сотен, десятков на дисплей DWIN.
        void sendTriPartFloatToDwin(uint16_t thousands_vp, uint16_t units_vp, uint16_t kopecks_vp,
                                        MessageLayer &core,
                                        int thousands_length, int units_length, int kopecks_length,
                                        const std::string& value);

        // Строка, содержащая значение, введенное с пинпада.
        std::string d_pinpad_buffer;

        // ITP Logger для удаленного логирования
        itp::logger::ptr m_itp_logger;
    };

    using DwinLogicPtr = std::shared_ptr<DwinLogic>;
}
