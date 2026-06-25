//
// Created by vadim.tissen on 13.05.2026.
//

#pragma once
#include <string>

#include "bridge/json.hpp"
#include "bridge/core/types.hpp"
#include "bridge/core/utility.hpp"
#include "bridge/database/TransactionRepository.hpp"

namespace bridge {
    enum TypeSale {
        Full = 1,
        Partial = 2,
        Revert = 3,
        Failure = 4
    };

    struct SalesData {
        std::string date;
        std::string processing;
        std::string fact_amount;
        std::string fact_price;
        std::string fact_quantity;
        std::string rqs_amount;
        std::string rqs_price;
        std::string rqs_quantity;
        std::string transaction_id;
        std::string card_number;
        std::string fuel_name;
        std::string trk_name;
        std::string type_sale;
        std::string id_tso;

        std::string after_density;
        std::string after_filling;
        std::string after_lower_level;
        std::string after_lower_volume;
        std::string after_temperature;
        std::string after_total_volume;
        std::string after_upper_level;
        std::string after_upper_volume;
        std::string after_weight;

        std::string before_density;
        std::string before_filling;
        std::string before_lower_level;
        std::string before_lower_volume;
        std::string before_temperature;
        std::string before_total_volume;
        std::string before_upper_level;
        std::string before_upper_volume;
        std::string before_weight;

        std::string discount;
        std::string dispenser_volume_begin;
        std::string dispenser_volume_end;
        std::string dsc;
        std::string receipt;
    };

    inline void parseTransactionFromJson(const nlohmann::json &j, TransactionData &t_data) {
        t_data.date = j.value("Date", "");
        t_data.processing = j.value("Processing", "");
        t_data.fact_amount = j.value("FactAmount", "");
        t_data.fact_price = j.value("FactPrice", "");
        t_data.fact_quantity = j.value("FactQuantity", "");
        t_data.rqs_amount = j.value("RqsAmount", "");
        t_data.rqs_price = j.value("RqsPrice", "");
        t_data.rqs_quantity = j.value("RqsQuantity", "");
        t_data.transaction_id = j.value("TransactionId", "");
        t_data.card_number = j.value("CardNumber", "");
        t_data.fuel_name = j.value("FuelName", "");
        t_data.trk_name = j.value("TrkName", "");
        t_data.type_sale = j.value("TypeSale", "");
        t_data.id_tso = j.value("IdTso", "");

        t_data.after_density = j.value("AfterDensity", "");
        t_data.after_filling = j.value("AfterFilling", "");
        t_data.after_lower_level = j.value("AfterLowerLevel", "");
        t_data.after_lower_volume = j.value("AfterLowerVolume", "");
        t_data.after_temperature = j.value("AfterTemperature", "");
        t_data.after_total_volume = j.value("AfterTotalVolume", "");
        t_data.after_upper_level = j.value("AfterUpperLevel", "");
        t_data.after_upper_volume = j.value("AfterUpperVolume", "");
        t_data.after_weight = j.value("AfterWeight", "");

        t_data.before_density = j.value("BeforeDensity", "");
        t_data.before_filling = j.value("BeforeFilling", "");
        t_data.before_lower_level = j.value("BeforeLowerLevel", "");
        t_data.before_lower_volume = j.value("BeforeLowerVolume", "");
        t_data.before_temperature = j.value("BeforeTemperature", "");
        t_data.before_total_volume = j.value("BeforeTotalVolume", "");
        t_data.before_upper_level = j.value("BeforeUpperLevel", "");
        t_data.before_upper_volume = j.value("BeforeUpperVolume", "");
        t_data.before_weight = j.value("BeforeWeight", "");

        t_data.discount = j.value("Discount", "");
        t_data.dispenser_volume_begin = j.value("DispenserVolumeBegin", "");
        t_data.dispenser_volume_end = j.value("DispenserVolumeEnd", "");
        t_data.dsc = j.value("Dsc", "");
        t_data.receipt = j.value("Receipt", "");

        t_data.sent_to_server = false;
        t_data.created_at = utility::getCurrentTimeString();
    }

    inline Bytes serializeSalesData(const SalesData &sales_data) {
        nlohmann::json j = {
            {"Date", sales_data.date},
            {"Processing", sales_data.processing},
            {"FactAmount", sales_data.fact_amount},
            {"FactPrice", sales_data.fact_price},
            {"FactQuantity", sales_data.fact_quantity},
            {"RqsAmount", sales_data.rqs_amount},
            {"RqsPrice", sales_data.rqs_price},
            {"RqsQuantity", sales_data.rqs_quantity},
            {"TransactionId", sales_data.transaction_id},
            {"CardNumber", sales_data.card_number},
            {"FuelName", sales_data.fuel_name},
            {"TrkName", sales_data.trk_name},
            {"TypeSale", sales_data.type_sale},
            {"IdTso", sales_data.id_tso},

            {"AfterDensity", sales_data.after_density},
            {"AfterFilling", sales_data.after_filling},
            {"AfterLowerLevel", sales_data.after_lower_level},
            {"AfterLowerVolume", sales_data.after_lower_volume},
            {"AfterTemperature", sales_data.after_temperature},
            {"AfterTotalVolume", sales_data.after_total_volume},
            {"AfterUpperLevel", sales_data.after_upper_level},
            {"AfterUpperVolume", sales_data.after_upper_volume},
            {"AfterWeight", sales_data.after_weight},

            {"BeforeDensity", sales_data.before_density},
            {"BeforeFilling", sales_data.before_filling},
            {"BeforeLowerLevel", sales_data.before_lower_level},
            {"BeforeLowerVolume", sales_data.before_lower_volume},
            {"BeforeTemperature", sales_data.before_temperature},
            {"BeforeTotalVolume", sales_data.before_total_volume},
            {"BeforeUpperLevel", sales_data.before_upper_level},
            {"BeforeUpperVolume", sales_data.before_upper_volume},
            {"BeforeWeight", sales_data.before_weight},

            {"Discount", sales_data.discount},
            {"DispenserVolumeBegin", sales_data.dispenser_volume_begin},
            {"DispenserVolumeEnd", sales_data.dispenser_volume_end},
            {"Dsc", sales_data.dsc},
            {"Receipt", sales_data.receipt}
        };

        const auto body = j.dump();
        return Bytes(body.begin(), body.end());
    }
}
