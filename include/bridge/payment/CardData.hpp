//
// Created by vadim.tissen on 16.06.2026.
//

#pragma once

#include "bridge/json.hpp"
#include "bridge/core/types.hpp"

#include <string>

namespace bridge {
    struct ParsedReceipt {
        std::string card_number;
        std::string wallet_type; // "Денежный", "Литровый" и т.д.
        std::string balance_before; // Очищенный баланс: "940934.423"
        std::string balance_after;
        bool hidden_balance;
        std::string limit_info;  // Отформатированный лимит: "Ост. сут. лимита: 10.00л"
    };

    inline Bytes serializeCardInfo(const ParsedReceipt& request) {
        nlohmann::json json = {
            {"card_number", request.card_number},
            {"wallet_type", request.wallet_type},
            {"balance_before", request.balance_before},
            {"balance_after", request.balance_after},
            {"limit_info", request.limit_info},
            {"balance_hidden", request.hidden_balance},
        };

        const auto body = json.dump();
        return Bytes(body.begin(), body.end());
    }

    inline bool deserializeCardInfo(const Bytes& payload, ParsedReceipt& request) {
        try {
            const std::string body(payload.begin(), payload.end());
            const auto json = nlohmann::json::parse(body);

            request.hidden_balance = json.value("balance_hidden", false);
            request.card_number = json.value("card_number", std::string());
            request.wallet_type = json.value("wallet_type", std::string());
            request.balance_before = json.value("balance_before", std::string());
            request.balance_after = json.value("balance_after", std::string());
            request.limit_info = json.value("limit_info", std::string());

            return true;
        } catch (...) {
            return false;
        }
    }
}