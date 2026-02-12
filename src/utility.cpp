//
// Created by vadim.tissen on 09.12.2025.
//


#include "bridge/utility.hpp"

#include <iomanip>
#include <ios>
#include <string>
#include "bridge/types.hpp"

namespace bridge {
    std::string utility::bytesToHex(const std::vector<uint8_t> &bytes) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (const auto &byte: bytes) {
            ss << std::setw(2) << static_cast<int>(byte) << " ";
        }
        return ss.str();
    }

    std::string utility::replaceIdInUrl(std::string url, const std::string &placeholder, const std::string &id) {
        size_t pos = url.find(placeholder);
        if (pos != std::string::npos) {
            url.replace(pos, placeholder.length(), id);
        }
        return url;
    }

    std::vector<uint8_t> utility::floatToBytes(float value) {
        std::vector<uint8_t> data(4);
        const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&value);

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

    float utility::parseFloatFromJson(const json &node) {
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

    std::string utility::parseStringFromJson(const json &value) {
        if (value.is_string()) {
            return value.get<std::string>();
        }
        if (value.is_number()) {
            return std::to_string(value.get<int>());
        }
        return "";
    }

    int utility::parseIntFromJson(const json &value) {
        if (value.is_number()) {
            return value.get<int>();
        }

        return 0;
    }

    bool utility::parseBoolFromJson(const json &value) {
        if (value.is_boolean()) {
            return value.get<bool>();
        }

        return false;
    }

    int utility::getIconForStatus(const std::string &status, const Settings &s) {
        if (status == DISPENSER_IDLE) {
            return s.dwin.icon_dispenser_idle;
        }
        if (status == DISPENSER_NOZZLE_UP) {
            return s.dwin.icon_dispenser_nozzle_up;
        }
        if (status == DISPENSER_COMPLETE) {
            return s.dwin.icon_dispenser_order_end;
        }
        return s.dwin.icon_dispenser_idle;
    }

    std::string utility::getIdFromUrl(const std::string &url, const std::string &marker) {
        size_t startPos = url.find(marker);
        if (startPos == std::string::npos) {
            return "";
        }

        startPos += marker.length();
        size_t endPos = url.find('/', startPos);

        if (endPos == std::string::npos) {
            return url.substr(startPos);
        }

        return url.substr(startPos, endPos - startPos);
    }



    Dispenser* utility::getDispenserById(std::vector<Dispenser>& dispensers, const std::string& id) {
        auto it = std::find_if(dispensers.begin(), dispensers.end(),
            [&id](const Dispenser& d) {
                return d.id == id;
            });

        if (it != dispensers.end()) {
            return &(*it);
        }
        return nullptr;
    }

    std::string utility::getFormattedStringFromJson(std::string valueStr, int exponent) {

        if (valueStr.empty()) return "0,00";
        if (exponent <= 0) return valueStr;

        if (valueStr.length() <= static_cast<size_t>(exponent)) {

            size_t zerosNeeded = exponent - valueStr.length() + 1;
            valueStr.insert(0, zerosNeeded, '0');
        }

        size_t insertPos = valueStr.length() - exponent;
        valueStr.insert(insertPos, ",");

        return valueStr;
    }

    std::string utility::extractFirstInt(const std::string &s) {
        std::string result;
        bool found = false;

        for (char c : s) {
            if (std::isdigit(static_cast<unsigned char>(c))) {
                found = true;
                result += c;
            } else if (found) {
                break;
            }
        }
        return result;
    }

    int utility::getIconForProduct(const std::string &product, const Settings &s) {
        if (product == RATING_PR_92) {
            return s.dwin.icon_fuel_type_92;
        }
        if (product == RATING_PR_95) {
            return s.dwin.icon_fuel_type_95;
        }
        if (product == RATING_PR_98) {
            return s.dwin.icon_fuel_type_98;
        }
        if (product == RATING_PR_100) {
            return s.dwin.icon_fuel_type_100;
        }
        if (product == RATING_PR_DIESEL) {
            return s.dwin.icon_fuel_type_dt;
        }
        return s.dwin.icon_fuel_type_dt;
    }

    const Product* utility::getProductById(const std::vector<Product>& products, const std::string& id) {
        auto it = std::find_if(products.begin(), products.end(),
            [&id](const Product& d) {
                return d.id == id;
            });

        if (it != products.end()) {
            return &(*it);
        }
        return nullptr;
    }

    const std::string utility::getCurrentTimeString() {
        std::time_t now = std::time(nullptr);
        std::tm* local_time = std::localtime(&now);

        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M:%S", local_time);

        return std::string(buffer);
    }
}
