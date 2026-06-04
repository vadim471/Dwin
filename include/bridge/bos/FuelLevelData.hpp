//
// Created by vadim.tissen on 13.05.2026.
//

#pragma once
#include "bridge/json.hpp"
#include "bridge/core/types.hpp"

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
}
