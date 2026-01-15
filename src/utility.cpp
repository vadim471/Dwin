//
// Created by vadim.tissen on 09.12.2025.
//


#include "bridge/utility.hpp"

#include <iomanip>
#include <ios>
#include <string>
#include "bridge/types.hpp"

namespace bridge {
    std::string utility::bytesToHex(const std::vector<uint8_t>& bytes) {
            std::stringstream ss;
            ss << std::hex << std::setfill('0');
            for (const auto& byte : bytes) {
                ss << std::setw(2) << static_cast<int>(byte) << " ";
            }
            return ss.str();
        }

    std::string utility::replaceIdInUrl(std::string url, const std::string& placeholder, std::string id) {
            size_t pos = url.find(placeholder);
            if (pos != std::string::npos) {
                url.replace(pos, placeholder.length(), id);
            }
            return url;
        }

    std::vector<uint8_t> utility::floatToBytes(float value) {
        std::vector<uint8_t> data(4);
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);

        data[0] = bytes[3];
        data[1] = bytes[2];
        data[2] = bytes[1];
        data[3] = bytes[0];

        return data;
    }

    std::vector<uint8_t> utility::stringToBytes(const std::string &str, size_t fixedLength) {
        std::vector<uint8_t> data(fixedLength, 0x00);
        size_t copyLen = std::min(str.size(), fixedLength);
        std::copy(str.begin(), str.begin() + copyLen, data.begin());
        return data;
    }

    float utility::parseFloatFromJson(const json& node) {
        if (node.is_object()) {
            float value = std::stof(node["value"].get<std::string>());

            if (node.count("exponent")) {
                int exponent = node["exponent"].get<int>();
                return value / pow(10, exponent);
            }
            return value;
        }
        return 0.0f;
    }

    std::string utility::parseStringFromJson(const json& value) {
        if (value.is_string()) {
            return value.get<std::string>();
        }
        if (value.is_number()) {
            return std::to_string(value.get<int>());
        }
        return "";
    }

    int utility::parseIntFromJson(const json& value) {
        if (value.is_number()) {
            return value.get<int>();
        }

        return 0;
    }

    bool utility::parseBoolFromJson(const json& value) {
        if (value.is_boolean()) {
            return value.get<bool>();
        }

        return false;
    }
}
