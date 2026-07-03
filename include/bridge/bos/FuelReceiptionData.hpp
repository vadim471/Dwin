//
// Created by vadim.tissen on 25.06.2026.
//

#pragma once
#include <string>

#include "bridge/json.hpp"
#include "bridge/core/types.hpp"
#include "bridge/core/utility.hpp"
#include "bridge/database/FuelReceptionRepository.hpp"

namespace bridge {
    struct FuelReceiptData {
        std::string id_tso;
        std::string date;

        std::string document_number;
        std::string level_gauge_id;
        std::string quantity;
        std::string fuel_name;

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
    };

    inline void parseFuelReceiptFromJson(const nlohmann::json &j, FuelReceptionRecord &t_data) {
        t_data.after_density = utility::stringToDouble(j.value("AfterDensity", ""));
        t_data.after_filling = utility::stringToDouble(j.value("AfterFilling", ""));
        t_data.after_lower_level = utility::stringToDouble(j.value("AfterLowerLevel", ""));
        t_data.after_lower_volume = utility::stringToDouble(j.value("AfterLowerVolume", ""));
        t_data.after_temperature = utility::stringToDouble(j.value("AfterTemperature", ""));
        t_data.after_total_volume = utility::stringToDouble(j.value("AfterTotalVolume", ""));
        t_data.after_upper_level = utility::stringToDouble(j.value("AfterUpperLevel", ""));
        t_data.after_upper_volume = utility::stringToDouble(j.value("AfterUpperVolume", ""));
        t_data.after_weight = utility::stringToDouble(j.value("AfterWeight", ""));

        t_data.before_density = utility::stringToDouble(j.value("BeforeDensity", ""));
        t_data.before_filling = utility::stringToDouble(j.value("BeforeFilling", ""));
        t_data.before_lower_level = utility::stringToDouble(j.value("BeforeLowerLevel", ""));
        t_data.before_lower_volume = utility::stringToDouble(j.value("BeforeLowerVolume", ""));
        t_data.before_temperature = utility::stringToDouble(j.value("BeforeTemperature", ""));
        t_data.before_total_volume = utility::stringToDouble(j.value("BeforeTotalVolume", ""));
        t_data.before_upper_level = utility::stringToDouble(j.value("BeforeUpperLevel", ""));
        t_data.before_upper_volume = utility::stringToDouble(j.value("BeforeUpperVolume", ""));
        t_data.before_weight = utility::stringToDouble(j.value("BeforeWeight", ""));

        t_data.id_tso = j.value("IdTso", "");
        t_data.date = j.value("Date", "");
        t_data.gauge_id = j.value("NamePmp", "");
        t_data.fuel_name = j.value("FuelName", "");
        t_data.document_number = j.value("NumDocuments", "");
        t_data.received_liters = utility::stringToDouble(j.value("Quantity", ""));


        t_data.sent_to_server = false;
        t_data.created_at = utility::getCurrentTimeString();
    }

    inline Bytes serializeFuelReceiptData(const FuelReceiptData &t_data) {
        nlohmann::json j = {
            {"AfterDensity", t_data.after_density},
            {"AfterFilling", t_data.after_filling},
            {"AfterLowerLevel", t_data.after_lower_level},
            {"AfterLowerVolume", t_data.after_lower_volume},
            {"AfterTemperature", t_data.after_temperature},
            {"AfterTotalVolume", t_data.after_total_volume},
            {"AfterUpperLevel", t_data.after_upper_level},
            {"AfterUpperVolume", t_data.after_upper_volume},
            {"AfterWeight", t_data.after_weight},

            {"BeforeDensity", t_data.before_density},
            {"BeforeFilling", t_data.before_filling},
            {"BeforeLowerLevel", t_data.before_lower_level},
            {"BeforeLowerVolume", t_data.before_lower_volume},
            {"BeforeTemperature", t_data.before_temperature},
            {"BeforeTotalVolume", t_data.before_total_volume},
            {"BeforeUpperLevel", t_data.before_upper_level},
            {"BeforeUpperVolume", t_data.before_upper_volume},
            {"BeforeWeight", t_data.before_weight},

            {"Date", t_data.date},
            {"IdTso", t_data.id_tso},
            {"NumDocuments", t_data.document_number},
            {"Quantity", t_data.quantity},
            {"NamePmp", t_data.level_gauge_id},
            {"FuelName", t_data.fuel_name},
        };

        const auto body = j.dump();
        return Bytes(body.begin(), body.end());
    }

    inline FuelReceiptData createFuelReceiptData(const LevelGauge *init_gauge, const LevelGauge *current_gauge, std::string name_pmp,
        std::string doc_num, std::string fuel_name) {
        FuelReceiptData data;

        data.after_density = current_gauge->density;
        data.after_filling = current_gauge->filling;
        data.after_lower_level = current_gauge->lower_level;
        data.after_lower_volume = current_gauge->lower_volume;
        data.after_temperature = "12";
        data.after_total_volume = current_gauge->total_volume;
        data.after_upper_level = current_gauge->upper_level;
        data.after_upper_volume = current_gauge->upper_volume;
        data.after_weight = current_gauge->weight;

        data.before_density = init_gauge->density;
        data.before_filling = init_gauge->filling;
        data.before_lower_level = init_gauge->lower_level;
        data.before_lower_volume = init_gauge->lower_volume;
        data.before_temperature = "12";
        data.before_total_volume = init_gauge->total_volume;
        data.before_upper_level = init_gauge->upper_level;
        data.before_upper_volume = init_gauge->upper_volume;
        data.before_weight = init_gauge->weight;

        data.document_number = doc_num;
        data.level_gauge_id = name_pmp;
        data.fuel_name = fuel_name;

        data.quantity = utility::calculateFuelLiters(init_gauge->total_volume, current_gauge->total_volume);
        return data;
    }
}
