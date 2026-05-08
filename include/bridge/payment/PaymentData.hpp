#pragma once

#include "bridge/json.hpp"
#include "bridge/core/types.hpp"

#include <string>

namespace bridge {

struct PaymentRequestData {
    std::string product_id;
    uint32_t price_value = 0;
    uint8_t price_decimal = 0;
    uint32_t volume_value = 0;
    uint8_t volume_decimal = 0;
    uint32_t amount_value = 0;
    uint8_t amount_decimal = 0;

    //std::string dispenser_id = ""; // Необходимо для повторного ввода ПИН-кода. в HttpLogic я достаю из мапы заказов последний по этому айди, чтобы не запрашивать по новой данные заказа.

    // Confirm-specific fields (from reference: payment.cpp)
    uint8_t not_complete = 0;       // 0 = full delivery, 1 = partial
    uint32_t fact_volume_value = 0;
    uint8_t fact_volume_decimal = 0;
    uint32_t fact_amount_value = 0;
    uint8_t fact_amount_decimal = 0;
};

inline Bytes serializePaymentRequest(const PaymentRequestData& request) {
    nlohmann::json json = {
        {"product_id", request.product_id},
        {"price_value", request.price_value},
        {"price_decimal", request.price_decimal},
        {"volume_value", request.volume_value},
        {"volume_decimal", request.volume_decimal},
        {"amount_value", request.amount_value},
        {"amount_decimal", request.amount_decimal},
        {"not_complete", request.not_complete},
        {"fact_volume_value", request.fact_volume_value},
        {"fact_volume_decimal", request.fact_volume_decimal},
        {"fact_amount_value", request.fact_amount_value},
        {"fact_amount_decimal", request.fact_amount_decimal},
    };

    const auto body = json.dump();
    return Bytes(body.begin(), body.end());
}

inline bool deserializePaymentRequest(const Bytes& payload, PaymentRequestData& request) {
    try {
        const std::string body(payload.begin(), payload.end());
        const auto json = nlohmann::json::parse(body);

        request.product_id = json.value("product_id", std::string());
        request.price_value = json.value("price_value", uint32_t(0));
        request.price_decimal = json.value("price_decimal", uint8_t(0));
        request.volume_value = json.value("volume_value", uint32_t(0));
        request.volume_decimal = json.value("volume_decimal", uint8_t(0));
        request.amount_value = json.value("amount_value", uint32_t(0));
        request.amount_decimal = json.value("amount_decimal", uint8_t(0));
        request.not_complete = json.value("not_complete", uint8_t(0));
        request.fact_volume_value = json.value("fact_volume_value", uint32_t(0));
        request.fact_volume_decimal = json.value("fact_volume_decimal", uint8_t(0));
        request.fact_amount_value = json.value("fact_amount_value", uint32_t(0));
        request.fact_amount_decimal = json.value("fact_amount_decimal", uint8_t(0));

        return true;
    } catch (...) {
        return false;
    }
}

struct ReceiptData {
    std::string address;
    std::string card_number;
    std::string product_name;
    double volume_liters = 0.0;
    double price_per_liter = 0.0;
    uint64_t transaction_id = 0;
    double ordered_volume_liters = 0.0;  // Для чека возврата
    bool is_refund = false;
};

inline Bytes serializeReceiptData(const ReceiptData& receipt) {
    nlohmann::json json = {
        {"address", receipt.address},
        {"card_number", receipt.card_number},
        {"product_name", receipt.product_name},
        {"volume_liters", receipt.volume_liters},
        {"price_per_liter", receipt.price_per_liter},
        {"transaction_id", receipt.transaction_id},
        {"ordered_volume_liters", receipt.ordered_volume_liters},
        {"is_refund", receipt.is_refund}
    };

    const auto body = json.dump();
    return Bytes(body.begin(), body.end());
}

inline bool deserializeReceiptData(const Bytes& payload, ReceiptData& receipt) {
    try {
        const std::string body(payload.begin(), payload.end());
        const auto json = nlohmann::json::parse(body);

        receipt.address = json.value("address", std::string());
        receipt.card_number = json.value("card_number", std::string());
        receipt.product_name = json.value("product_name", std::string());
        receipt.volume_liters = json.value("volume_liters", 0.0);
        receipt.price_per_liter = json.value("price_per_liter", 0.0);
        receipt.transaction_id = json.value("transaction_id", uint64_t(0));
        receipt.ordered_volume_liters = json.value("ordered_volume_liters", 0.0);
        receipt.is_refund = json.value("is_refund", false);

        return true;
    } catch (...) {
        return false;
    }
}

} // namespace bridge
