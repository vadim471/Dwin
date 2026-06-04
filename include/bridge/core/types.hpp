//
// Created by vadim.tissen on 18.11.2025.
//

#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <chrono>
#include "constant.hpp"

//types.hpp
namespace bridge {
    using Bytes = std::vector<uint8_t>;

    struct RawData {
        Bytes data;
    };

    enum class ErrorSeverity {
        WARNING,
        CRITICAL,
        FATAL
    };

    enum class ErrorSource {
        HTTP,
        UART,
        NETWORK,
        SYSTEM
    };

    struct ErrorEvent {
        ErrorSeverity severity;
        ErrorSource source;
        int code;
        std::string description;
        std::string context;
    };

    struct Message {
        uint64_t id = 0;
        std::string type; //button_click, update
        std::string source; //ws, http, uart
        std::string url; //http url response
        Bytes payload; //deserialized data
        std::string resource_id; // id replace in URL
        int status_code;
        std::chrono::system_clock::time_point timestamp;
    };

    struct HttpResponseData {
        std::string url;
        int status_code;
        std::string body;
        std::string reason;
    };

    struct Order {
        std::string amount;
        std::string volume;
        std::string price;
        std::string order_id;
        std::string product_id;
    };

    struct DatabaseOrder {
        std::string id = "";
        uint64_t time_begin = 0;
        uint64_t time_end = 0;
        std::string status = "";
        bool complete = false;
        bool closed = false;
        std::string dispenser_id = "";
        std::string product_id = "";
        std::string product_rating = "";
        std::string order_type = ""; // По объему / полный бак / сумма
        double order_price = 0.0; // Цена на ТРК (литр топлива)
        double order_value = 0.0; // Выбранная сумма или объем
        double order_amount = 0.0; // Сумма на ТРК
        double order_volume = 0.0; // Объем на ТРК
        double amount = 0.0; // Текущая сумма
        double volume = 0.0; // Текущий объем
        uint32_t fact_amount_raw = 0;
        uint8_t fact_amount_exp = 0;
        uint32_t fact_volume_raw = 0;
        uint8_t fact_volume_exp = 0;
        bool round_to_order = false; // Округлить до суммы заказа

        DatabaseOrder() = default;

        DatabaseOrder(const std::string& id) : id(id) {}
    };

    struct HttpRequestTask {
        std::string method;
        std::string uri;
        std::string body;
    };

    struct Product {
        std::string id;
        std::string rating;
        std::string title;
        std::string price;

        Product(const std::string& id) : id(id) {}
    };

    struct Dispenser {
        std::string id;
        std::string status;
        std::string prev_status;
        Order order; // Фактический заказ. Текущий налитый в m_orders HttpLogic.
        std::string product_id;
        std::chrono::steady_clock::time_point m_last_volume_change_time; // Время последней информации о проливе.
        bool is_fuelling_paused; // Отжат ли курок.
        Dispenser(const std::string &_id)
            : id(_id),
              status(DISPENSER_IDLE),
              m_last_volume_change_time(std::chrono::steady_clock::now()),
              is_fuelling_paused(false) {}
    };

    struct LevelGauge {
        std::string id;
        std::string upper_volume; // Объём основного продукта (м³).
        std::string lower_volume; // Объём подтоварной жидкости (м³).
        std::string total_volume; // Общий объём (м³).
        std::string upper_level; // Уровень основного поплавка (м).
        std::string lower_level; // Уровень нижнего поплавка (м).
        std::string filling; // Заполнение (%).
        std::string density; // Плотность (кг/м³).
        std::string weight; // Масса (кг).

        LevelGauge(const std::string& _id) : id(_id) {}
        LevelGauge(){}
    };

    struct Tanker {
        std::string id;
        std::string product_id;
        std::string title;
        bool active;
        bool filled;
        std::string lock;
        std::string minimal;
        std::vector<std::string> level_gauge_ids;

        Tanker(const std::string& _id) : id(_id) {}
    };

    struct ApiRoute {
        std::string method; //GET, POST, PUT
        std::string url; //"api/v3/device"
    };

    enum class GaugeTaskType {
        BEFORE_FUELLING, //ORD_AUTHORIZED
        AFTER_FUELLING // ORD_INTERRUPTED / ORD_DELIVERED
    };

    // Структура для заполнения в последствии информации об уровнемерах в другую структуру.
    struct PendingGaugeTask {
        std::string order_id;
        std::string dispenser_id;
        GaugeTaskType task_type;
    };

    // Структура для извлечения полезной нагрузки из чека баланса optima-pc.
    struct ParsedReceipt {
        std::string card_number;
        std::string wallet_type; // "Денежный", "Литровый" и т.д.
        std::string balance_before; // Очищенный баланс: "940934.423"
        std::string balance_after;
        std::string limit_info;  // Отформатированный лимит: "Ост. сут. лимита: 10.00л"
    };
}
