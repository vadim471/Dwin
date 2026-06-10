//
// Created by vadim.tissen on 10.12.2025.
//
#pragma once
#include <future>
#include <functional>
#include <mutex>
#include <queue>
#include <set>
#include <unordered_set>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>

#include "bridge/payment/PaymentData.hpp"
#include "bridge/json.hpp"
#include "ILogicHandler.hpp"
#include "bridge/core/Settings.hpp"
#include <itp/logger.hpp>

#include "bridge/bos/SalesData.hpp"


//HttpLogic.hpp
namespace bridge {
    using json = nlohmann::json;

    class PrimeLogic : public std::enable_shared_from_this<PrimeLogic>, public ILogicHandler {
    public:
        PrimeLogic(const Settings& settings, boost::asio::io_service& io_service);

        void startLoop(MessageLayer& core);
        void handle(const Message& msg, MessageLayer& core) override;

        static void getDevices(MessageLayer& core);
        static void getParameters(MessageLayer& core, const std::string& id);
        static void getProducts(MessageLayer& core);
        static void getDispenserStatus(MessageLayer &core, const std::string &trk_id);
        static void getTankers(MessageLayer& core);
        void startPolling(MessageLayer& core) const;

        // Инициализация ITP Logger
        void setItpLogger(itp::logger::ptr logger) { m_itp_logger = logger; }

        // Установка репозитория Bos для записи raw JSON
        // void setBosRepository(std::shared_ptr<BosRepository> repo) { m_bos_repo = repo; }


    private:
        void processEvent(const json& jArray, MessageLayer& core);

        // Для первой загрузки. Узнать количество ТРК и их статусы.
        void processDevices(const json& jArray, MessageLayer& core);

        void processParameters(const json& jArray, std::string url, MessageLayer& core);

        // Парсинг состояния одной ТРК.
        void processDispenserStatus(const json& jArray, MessageLayer& core, const std::string& url);

        // Для первой загрузки. Узнать виды топлива на заправке.
        void processProducts(const json& jArray);

        // Для первой загрузки. Узнать все резервуары на заправке.
        void processTankers(const json& jArray);

        // Хендлер обработки HTTP запросов.
        void processHandleHttp(const Message& message, MessageLayer& core);

        // Хендлер обработки различных ответов на HTTP запросы.
        void processVariableHttp(const json& jArray, MessageLayer& core, const std::string &url);

        // Обработка нажатий на дисплей.
        void processHandleUserTouch(const Message& message, MessageLayer& core);

        // Для начала пролива заказа нужно узнать id команды и ждать, пока ok = true
        void processTask(const json& jArray, MessageLayer& core) const;

        void processTaskId(const json& jArray, MessageLayer& core, std::string url);

        // Действия после нажатия на ТРК с неподнятым пистолетом.
        void processDispenserIdleAfterUserTouch(MessageLayer &core);

        // Действия после нажатия на ТРК с поднятым пистолетом.
        void processDispenserNozzleUpAfterUserTouch(MessageLayer &core, Product* product);

        // Действия после выбора ТРК с поднятым пистолетом.
        void processChosenDispenserNozzleUp(MessageLayer &core);

        // Дейстивия после завершения заказа выбранной ТРК.
        void processChosenDispenserComplete(MessageLayer &core);

        // Для завершения заказа. Каждый луп проверяется, есть ли в списке ТРК с
        // завершенным заказом, чтобы оновить последние значения и отправить актуальную информацию.
        void processOrderFinalization(const std::string& device_id, MessageLayer& core);

        // Печать чека Дебета
        void printDebetReceipt(MessageLayer& core);

        // Хендлер обработки статуса выбранной ТРК.
        void handleDispenserStatus(const json& jArray, MessageLayer& core);

        // Хендлер обработки статуса дисплея ТРК. Используется при проливе заказа
        // для изменения текущего состояния налива.
        void handleDispenserDisplay(const json& jArray, MessageLayer& core);

        // Хендлер обработки созданного заказа на ТРК.
        void handleDispenserOrderCreated(const json& jArray, MessageLayer& core);

        // Хендлер обработки изменения заказа на ТРК. ТРК выбрана.
        void handleDispenserOrderStatus(const json& jArray, MessageLayer& core, std::set<std::string>& completed_dispensers) ;

        // Хендлер обработки статуса невыбранной ТРК. Для страницы выбора ТРК.
        void handleRandomDispenserStatus(const json& event, MessageLayer& core);

        // Хендлер обработки статуса заказа невыбранной ТРК. Изменить состояние заказа в Map.
        void handleRandomDispenserOrderStatus(const json& event);

        // Хендлер обработки выбора количества ТРК.
        void handleAmountTRK(int amount);

        // Хендлер обработки выбора топлива для редактирования цены.
        void handleFuelTypeEditing(const Message& message, MessageLayer& core);

        // Хендлер обработки выбора сервисного меню.
        void handleServiceMenuButton(const Message& message, MessageLayer& core);

        // Хендлер обработки выбора состояния уровнемеров.
        void handleLevelGaugeButton(const Message& message, MessageLayer& core);

        // Хендлер обработки выбора редактирования цены топлива.
        void handleEditFuelButton(MessageLayer& core);

        // Хендлер создания заказа на пролив.
        void handleCreateOrder(MessageLayer& core, uint16_t volume, int exponent);

        // Хендлер обработки нажатий кнопки Basic Touch.
        void handleBasicTouch(MessageLayer& core);

        // Хендлер обработки выбора ТРК для использования на АЗС.
        void handleAcceptSelectedTRK();

        // Хендлер нажатия на ТРК выбора ТРК для использования на АЗС. Конфигурация настройки, какие ТРК использовать.
        void handleTrkSelectionToggle(MessageLayer& core, uint8_t slot_index);
        
        // Хендлер обработки ответа от Arkaim платежного процессинга
        void handlePaymentResponse(const Message& msg, MessageLayer& core);

        // Обработка нажатия на кнопку пагинации страницы выбора ТРК.
        void handlePaginationDispenser(int direction, MessageLayer& core);

        // Хендлер выбора ТРК.
        void handleChooseTRK(MessageLayer& core, int index);

        // Хендлер нажатия на Enter PinPad ввода цены топлива за литр.
        void handleEnterFuelPricePinPad(MessageLayer& core, std::string buffer);

        // Хендлер нажатия на Enter PinPad ввода объема заказа.
        void handleEnterFuelVolumeOrderPinPad(MessageLayer& core, std::string buffer);

        // Хендлер обработки распознания карты.
        void handleCardResolved(MessageLayer& core);

        // Хендлер обработки завершения заказа PAY_CONFIRM.
        void handlePayConfirmOrder(MessageLayer& core);

        // Хендлер обработки баланса карты.
        void handleGetBalanceCard(MessageLayer& core, const Message& message);

        // Хендлер обработки ввода неверного ПИН-кода.
        void handleIncorrectPinCode(MessageLayer& core, const std::string& order_id, const std::string& dispenser_id);

        // Хендлер обработки отмены транзакции сразу после оплаты
        void handleImmediatlyCancelTransaction(MessageLayer& core);

        // Хендлер обработки нажатия кнопки ENTER ввода ПИН кода.
        void handleEnterPinCodeButton(MessageLayer& core, const Message& message);

        // Хендлер обработки завершения приема топлива сервисного меню.
        void handleFinishReceptionFuel(MessageLayer& core);

        // Хендлер обработки нажатия создания нового заказа при проливе предыдущего.
        void handleCreateNewOrder(MessageLayer& core);

        // Хендлер обработки отмены ввода пин-кода после создания заказа.
        void handleCancelEnterPin(MessageLayer& core);

        // Хендлер обработки начала заполнения танкера
        void processReceptionTanker(MessageLayer& core);

        // Отрисовка страницы выбора ТРК для использования на АЗС. Настройка конфигурации, какие ТРК использовать.
        void renderTrkSelectionPage(MessageLayer& core);

        // Заполнение TRK ID на всех страницах интерфейса.
        void setTRKIdOnDisplay(MessageLayer& core, const std::string& trk_id) const;

        // Заполнение типа топлива на всех страницах интерфейса (Картинки).
        void setProductIdOnDisplay(MessageLayer& core, int value) const;

        // Заполнение типа топлива на всех страницах интерфейса (Текст).
        void setProductTextOnDisplay(MessageLayer& core, const std::string& text) const;

        // Вывод текущего времени в Footer.
        void setFooterDateTime(MessageLayer& core);

        // Вывод текущей цены топлива выбранного вида топлива на всех страницах.
        void setFuelTypePriceOnDisplay(MessageLayer& core, const std::string &value) const;

        // Заполнение значений параметров уровнемера.
        void setLevelGaugeParametersOnDisplay(MessageLayer& core, const LevelGauge& level_gauge, const std::string &type);
        void setReceptionLevelGaugeParametersOnDisplay(MessageLayer& core, const LevelGauge& level_gauge);

        static void sendTaskToHttp(MessageLayer& core, std::string id);

        // Обработка нажатия на кнопку пагинации страницы уровнемеров.
        void changeLevelGauge(int direction, MessageLayer& core, const std::string &type);

        // Обработка нажатия на кнопку пагинации страницы редактирования цены топлива.
        void changeFuelType(int direction, MessageLayer& core);

        // Отображение иконки ТРК страницы выбора ТРК по статусу.
        void renderDispenser(MessageLayer& core);

        // Создание заказа и отправка его на сервер.
        void createOrder(MessageLayer& core, int value, int exponent, int price, int price_exponent) const;

        // Запрос на выполнение платежа по карте CL2_CMD_PAY_TRANSACTION.
        void createPayment(MessageLayer& core,
                           const std::string& order_id,
                           const std::string& product_id,
                           uint32_t int_price_per_fuel,
                           uint8_t dec_price_per_fuel,
                           uint32_t int_value_fuel,
                           uint8_t dec_value_fuel,
                           uint32_t int_order_price,
                           uint8_t dec_order_price);

        // Команда на начало пролива ТРК.
        void authorizeOnServer(MessageLayer& core);

        void onTimerExpired(const boost::system::error_code& ec, MessageLayer& core);
        void scheduleNextTick(MessageLayer& core);

        // Таймаут: проверка и запуск отложенных страниц.
        void checkTimers(MessageLayer& core);
        void startPageTimer(int timeout_seconds, uint16_t page_id, std::function<void()> on_expire = nullptr);
        void clearTimers();

        // Стартовая страница выбора ТРК по конфигурации (mono/duo/multi).
        uint16_t getStartPage() const;

        // Рендеринг ТРК на дисплее по конфигурации.
        void renderMonoDispenser(MessageLayer& core);
        void renderDuoDispenser(MessageLayer& core);
        void renderMultiDispenser(MessageLayer& core);

        // Заполнение страницы с редактированием типов топлива 1 вариант со строками.
        void fillPageEditingFuelType(std::vector<Product>& products, MessageLayer& core);

        // Заполнение страницы с редактированием цены типов топлива 2 вариант.
        void fillPageEditingFuelTypeSecondVariable(std::vector<Product>& products, MessageLayer& core);

        // Отправка числа с плавающей запятой. Принимает строку вида "12,34" - отправляет на 1 адрес "12", на второй адрес "34".
        void sendFloatToDwin(uint16_t int_vp, uint16_t dec_vp, MessageLayer& core, int int_length, int dec_length, std::string value);

        // Обработчик ошибок HTTP Layer.
        void processHandleHttpError(const Message& message, MessageLayer &core);

        // Диспетчер ошибок
        void dispatchError(const ErrorEvent& error, MessageLayer& core);

        // Проверка задержки заказа.
        void checkFuellingProcess(MessageLayer& core);

        boost::asio::deadline_timer m_timer;
        boost::asio::deadline_timer m_ui_timer;
        std::atomic<bool> m_waitingResponse;

        std::mutex m_mutex;
        std::queue<std::promise<Message>> m_pendingRequest;

        long long m_lastId = 0;
        Settings m_settings;

        std::vector<Product> m_products;
        std::vector<Dispenser> m_dispensers;
        std::vector<LevelGauge> m_level_gauge;
        std::vector<Tanker> m_tankers;

        std::vector<std::string> m_edit_page_row_to_fuel_id; // Карта для связи строки - id топлива. Для страницы редактирования цены топлива.
        std::string m_current_fuel_id_editing;

        int m_amount_trk; // Значение конфигурации ТРК. Моно / Дуо / Мульти ТРК.

        std::string m_current_dispenser_id; // Реальная id ТРК.

        std::string m_current_tanker_id; // Реальная id выбранного уровнемера для наполнения.

        std::vector<std::string> m_selected_dispensers; // ID выбранных ТРК в правильном порядке

        std::unordered_map<std::string, DatabaseOrder> m_orders; // Мапа, хранящая созданные заказы на ААЗС. Dispenser_id - DatabaseOrder. Order хранится ЗАКАЗА, не фактический.

        std::unordered_map<std::string, SalesData> m_pending_info_sales; // Мапа для хранения промежуточного состояния SalesData. Пока запрос пройдет все стадии, информация будет наполняться.

        std::unordered_map<std::string, std::queue<PendingGaugeTask>> m_pending_level_gauge_orders; // Мапа, хранящая заказы, ждущие информацию об уровнемере

        // Для пагинации ТРК.
        int m_current_dispenser_index = 0;

        // Для пагинации уровнемеров.
        int m_current_level_gauge_index = 0;

        bool m_reception_active = false;
        std::string m_reception_level_gauge_id;
        std::string m_reception_document_number;
        LevelGauge m_reception_initial_state;
        std::string m_reception_start_time;

        // Для пагинации типов топлива при выборе для редактирования.
        int m_current_fuel_type_index = 0;

        // PNG id, указывающее на количество ТРК АЗС.
        int m_amount_trk_png = 1;

        // Флаг, указывающий, нажали ли на создание нового заказа во время пролива. Чтобы после завершения предыдущего не перелистывало на страницу завершения заказа.
        //bool m_new_order_created = false;

        // Id созданного заказа на Prime.
        int m_current_order_task = 0;

        bool m_dwin_button_next_for_start_fuel = false;
        bool m_is_server_ready_for_start_fuel = false;
        std::unordered_set<std::string> m_payment_requested_order_ids;

        // Для отправки времени в Footer каждую секунду.
        std::string m_last_time_str;

        // Универсальный таймер для отложенного показа страниц
        struct PageTimer {
            std::chrono::steady_clock::time_point start;
            int timeout_seconds = 0;
            uint16_t page_id = 0;
            std::function<void()> on_expire;
        };
        std::vector<PageTimer> m_page_timers;

        std::chrono::steady_clock::time_point m_last_fueling_sound;

        // ITP Logger для удаленного логирования
        itp::logger::ptr m_itp_logger;

        ParsedReceipt m_parsed_card_balance;
        bool m_waiting_balance_response; // флаг ожидания баланса карты для вывода его на экран дисплея.
    };

    using HttpLogicPtr = std::shared_ptr<PrimeLogic>;
}
