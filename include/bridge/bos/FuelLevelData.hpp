//
// Created by vadim.tissen on 13.05.2026.
//

#pragma once
#include "bridge/json.hpp"
#include "bridge/core/types.hpp"
#include "bridge/core/utility.hpp"
#include "bridge/database/LevelGaugeRepository.hpp"

namespace bridge {
    struct FuelLevelData {
        std::string date;
        uint64_t id_tso;
        uint64_t time_stamp;
        std::string name_pmp;
        std::string fuel_name;
        double upper_level;
        double lower_level;
        double upper_volume;
        double lower_volume;
        double total_volume;
        double temperature;
        double weight;
        double density;
        double filling;
    };
    inline Bytes serializeFuelLevel(const FuelLevelData& data) {
        nlohmann::json j = {
            {"Date", data.date},
            {"IdTso", data.id_tso},
            {"TimeStamp", data.time_stamp},
            {"NamePmp", data.name_pmp},
            {"FuelName", data.fuel_name},
            {"UpperLevel", data.upper_level},
            {"LowerLevel", data.lower_level},
            {"UpperVolume", data.upper_volume},
            {"LowerVolume", data.lower_volume},
            {"TotalVolume", data.total_volume},
            {"Temperature", data.temperature},
            {"Weight", data.weight},
            {"Density", data.density},
            {"Filling", data.filling}
        };

        std::string s = j.dump();
        return Bytes(s.begin(), s.end());
    };

    inline void parseLevelGaugeRecordFromJson(const nlohmann::json &j, LevelGaugeData &lg_data) {
        lg_data.date = j.value("Date", "");
        lg_data.timestamp = j.value("TimeStamp", "");
        lg_data.gauge_id = j.value("NamePmp", "");
        lg_data.gauge_name = j.value("NamePmp", "");
        lg_data.fuel_name = j.value("FuelName", "");
        lg_data.upper_level = j.value("UpperLevel", "");
        lg_data.lower_level = j.value("LowerLevel", "");
        lg_data.upper_volume = j.value("UpperVolume", "");
        lg_data.lower_volume = j.value("LowerVolume", "");
        lg_data.total_volume = j.value("TotalVolume", "");
        lg_data.temperature = j.value("Temperature", "");
        lg_data.weight = j.value("Weight", "");
        lg_data.density = j.value("Density", "");
        lg_data.filling = j.value("Filling", "");

        lg_data.sent_to_server = false;
        lg_data.created_at = utility::getCurrentTimeString();
    }
}
