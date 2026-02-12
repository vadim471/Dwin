//
// Created by vadim.tissen on 10.12.2025.
//
#pragma once
#include <future>
#include <mutex>
#include <queue>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include "bridge/json.hpp"
#include "bridge/MessageLayer.hpp"
#include "ILogicHandler.hpp"
#include "types.hpp"
#include "Settings.hpp"

//HttpLogic.hpp
namespace bridge {
    using json = nlohmann::json;


    enum class DispenserFlowState {
        IDLE,
        SHOWING_FUEL_TYPE,
        WAITING_USER_INPUT
    };

    class HttpLogic : public std::enable_shared_from_this<HttpLogic>, public ILogicHandler {
    public:
        HttpLogic(const Settings& settings, boost::asio::io_service& io_service);

        void startLoop(MessageLayer& core);
        void handle(const Message& msg, MessageLayer& core) override;

        static void getDevices(MessageLayer& core);
        static void getParameters(MessageLayer& core, const std::string& id);
        static void getProducts(MessageLayer& core);
        static void getDispenserStatus(MessageLayer &core, const std::string &trk_id);
        void startPolling(MessageLayer& core) const;

    private:
        void processEvent(const json& jArray, MessageLayer& core);

        // Для первой загрузки. Узнать количество ТРК и их статусы.
        void processDevices(const json& jArray, MessageLayer& core);

        static void processParameters(const json& jArray, MessageLayer& core);

        // Парсинг состояния одной ТРК.
        void processDispenserStatus(const json& jArray, MessageLayer& core, const std::string& url);

        // Для первой загрузки. Узнать виды топлива на заправке.
        void processProducts(const json& jArray);

        // Для отправки COMMAND на сервер.
        void processHandleHttp(const json& jArray, MessageLayer& core, const std::string& url);

        // Обработка нажатий на дисплей.
        void processHandleUserTouch(const Message& message, MessageLayer& core);

        // Для начала пролива заказа нужно узнать id команды и ждать, пока ok = true
        void processTask(const json& jArray, MessageLayer& core);
        void processTaskId(const json& jArray, MessageLayer& core);

        // Хендлер обработки статуса выбранной ТРК.
        void handleDispenserStatus(const json& jArray, MessageLayer& core);

        // Хендлер обработки статуса дисплея ТРК. Используется при проливе заказа
        // для изменения текущего состояния налива.
        void handleDispenserDisplay(const json& jArray, MessageLayer& core);

        // Хендлер обработки созданного заказа на ТРК.
        void handleDispenserOrderCreated(MessageLayer& core);

        // Хендлер обработки изменения заказа на ТРК.
        void handleDispenserOrderChanged(const json& jArray, MessageLayer& core) const;

        // Хендлеры обработки статуса не выбранной ТРК. Для страницы выбора ТРК.
        void handleRandomDispenserStatus(const json& event, MessageLayer& core);

        // Заполнение TRK ID на всех страницах интерфейса.
        void setTRKIdOnDisplay(MessageLayer& core, std::string trk_id) const;

        // Заполнение типа топлива на всех страницах интерфейса (Картинки).
        void setProductIdOnDisplay(MessageLayer& core, int value) const;

        // Заполнение типа топлива на всех страницах интерфейса (Текст).
        void setProductTextOnDisplay(MessageLayer& core, const std::string& text) const;

        // Вывод текущей суммы заказа на всех страницах интерфейса.
        void setCurrentOrderAmountOnDisplay(MessageLayer& core, const std::string &value) const;

        // Вывод текущего объема заказа на всех страницах интерфейса
        void setCurrentOrderVolumeOnDisplay(MessageLayer& core, const std::string &value) const;

        // Вывод текущего времени в Footer.
        void setFooterDateTime(MessageLayer& core);

        // Вывод текущей цены топлива выбранного вида топлива на всех страницах.
        void setFuelTypePriceOnDisplay(MessageLayer& core, const std::string &value) const;

        static void sendTaskToHttp(MessageLayer& core, std::string id);

        // Обработка нажатия на кнопку пагинации страницы выбора ТРК.
        void changeDispenser(int direction, MessageLayer& core);

        // Отображение иконки ТРК страницы выбора ТРК по статусу.
        void renderDispenser(MessageLayer& core);

        // Создание заказа и отправка его на сервер.
        void createOrder(MessageLayer& core, int value, int exponent);

        // Команда на начало пролива ТРК.
        void authorizeOnServer(MessageLayer& core);

        // Обработка нажатий на пинпад дисплея.
        void handlePinpadButton(const Message& message, MessageLayer& core);

        void onTimerExpired(const boost::system::error_code& ec, MessageLayer& core);
        void scheduleNextTick(MessageLayer& core);

        // Таймаут на снятие пистолета выбранной ТРК.
        void checkIdleTimeout(MessageLayer& core);

        boost::asio::deadline_timer m_timer;
        boost::asio::deadline_timer m_ui_timer;
        std::atomic<bool> m_waitingResponse;
        DispenserFlowState m_flowState = DispenserFlowState::IDLE;

        std::mutex m_mutex;
        std::queue<std::promise<Message>> m_pendingRequest;

        long long m_lastId = 0;
        Settings m_settings;
        //Order m_current_order;

        std::vector<Product> m_products;
        std::vector<Dispenser> m_dispensers;

        // Для пагинации.
        int m_current_dispenser_index = 0;

        // Реальная id ТРК.
        std::string m_current_dispenser_id;

        // Строка, содержащая значение, введенное с пинпада.
        std::string m_pinpad_buffer;

        bool m_dwin_button_next_for_start_fuel = false;
        bool m_is_server_ready_for_start_fuel = false;

        // Для отправки времени в Footer каждую секунду.
        std::string m_last_time_str;

        std::chrono::steady_clock::time_point m_idle_timer_start;
        bool m_is_idle_timer_running = false;
    };

    using HttpLogicPtr = std::shared_ptr<HttpLogic>;
}