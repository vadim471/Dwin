//
// Created by vadim.tissen on 09.12.2025.
//
#pragma once
#include <string>

#include "Settings.hpp"
#include "bridge/types.hpp"

namespace bridge {
    class utility {
    public:
        static std::string bytesToHex(const std::vector<uint8_t>& bytes);
        static std::string replaceIdInUrl(std::string url, const std::string& placeholder, std::string id);
        static std::vector<uint8_t> floatToBytes(float value);
        static std::vector<uint8_t> stringToBytes(const std::string& str, size_t fixedLength);
        static float parseFloatFromJson(const json& node);
        static std::string parseStringFromJson(const json& node);
        static int parseIntFromJson(const json& node);
        static bool parseBoolFromJson(const json& node);
    };
}
