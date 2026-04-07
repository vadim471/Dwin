//
// Created by vadim.tissen on 09.12.2025.
//


#include "bridge/utility.hpp"

#include <iomanip>
#include <ios>
#include <string>
#include "bridge/types.hpp"
#include <algorithm>

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

    double utility::parseDoubleFromJson(const std::string& value_str, int exponent) {
        try {
            if (value_str.empty()) {
                return 0.0;
            }

            double base_value = std::stod(value_str);

            return base_value * std::pow(10.0, -exponent);

        } catch (const std::invalid_argument& e) {
            std::cerr << "[Utility] Invalid argument converting to double: " << value_str << std::endl;
            return 0.0;
        } catch (const std::out_of_range& e) {
            std::cerr << "[Utility] Out of range converting to double: " << value_str << std::endl;
            return 0.0;
        }
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

    int utility::ceilStringToInt(std::string value_str) {
        if (value_str.empty()) return 0;

        std::replace(value_str.begin(), value_str.end(), ',', '.');

        try {
            double value = std::stod(value_str);

            return static_cast<int>(std::ceil(value));

        } catch (const std::invalid_argument& e) {
            return 0;
        } catch (const std::out_of_range& e) {
            return 0;
        }
    }

    std::tuple<int, int> utility::formatFloatStringToInt(std::string value_str) {
        int value = 0;
        int exponent = 0;

        size_t dot_position = value_str.find(",");
        if (dot_position != std::string::npos) {
            exponent = value_str.length() - dot_position - 1;

            std::string string_value = value_str;
            string_value.erase(dot_position, 1);
            try {
                value = std::stol(string_value);
            } catch (std::exception &e) {
                value = 0;
            }
        } else {
            try {
                value = std::stol(value_str);
            } catch (std::exception &e) {
                value = 0;
            }
        }
        return std::make_tuple(value, exponent);;
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

        if (valueStr.empty() || valueStr == "0") return "0,00";
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
        if (result.empty()) {
            return s;
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

    int utility::getIconForGasStation(int amount, const Settings &s) {
        if (amount == 1) return s.dwin.page_choose_trk_config_uno_trk;
        if (amount == 2) return s.dwin.page_choose_trk_config_double_trk;
        return s.dwin.page_choose_trk;
    }

    Product* utility::getProductById(std::vector<Product>& products, const std::string& id) {
        auto it = std::find_if(products.begin(), products.end(),
            [&id](const Product& d) {
                return d.id == id;
            });

        if (it != products.end()) {
            return &(*it);
        }
        return nullptr;
    }

    LevelGauge* utility::getLevelGaugeById(std::vector<LevelGauge>& gauges, const std::string& id) {
        auto it = std::find_if(gauges.begin(), gauges.end(),
            [&id](const LevelGauge& d) {
                return d.id == id;
            });

        if (it != gauges.end()) {
            return &(*it);
        }
        return nullptr;
    }

    const Tanker* utility::getTankerByLevelGaugeId(const std::vector<Tanker>& tankers, const std::string& gauge_id) {
        for (const auto& tanker : tankers) {
            auto it = std::find(tanker.level_gauge_ids.begin(), tanker.level_gauge_ids.end(), gauge_id);

            if (it != tanker.level_gauge_ids.end()) {
                return &tanker;
            }
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

    std::pair<std::string, std::string> utility::splitFloatString(const std::string& floatStr, size_t lenFrac) {
        std::string intPart = "0";
        std::string fracPart = "";

        if (floatStr.empty()) {
            return std::make_pair(intPart, std::string(lenFrac, '0'));
        }

        size_t dotPos = floatStr.find(',');
        if (dotPos == std::string::npos) {
            dotPos = floatStr.find('.');
        }

        if (dotPos != std::string::npos) {
            intPart = floatStr.substr(0, dotPos);
            fracPart = floatStr.substr(dotPos + 1);
        } else {
            intPart = floatStr;
        }

        if (intPart.empty()) intPart = "0";
        if (intPart == "-")  intPart = "-0";

        if (fracPart.length() > lenFrac) {
            fracPart = fracPart.substr(0, lenFrac);
        } else if (fracPart.length() < lenFrac) {
            fracPart.append(lenFrac - fracPart.length(), '0');
        }

        return std::make_pair(intPart, fracPart);
    }

    bool utility::checkAllFuelPrice(std::vector<Product>& products) {
        if (products.empty()) {
            return false;
        }

        for (const auto& product : products) {
            if (product.price == "0" || product.price.empty()) {
                return false;
            }
        }
        return true;
    }

    void utility::saveTRKAmountToConfig(const std::string& configPath, int amount) {
        try {
            json j;
            std::ifstream inFile(configPath);
            if (inFile.is_open()) {
                inFile >> j;
                inFile.close();
            }

            if (!j.contains("gas.station")) {
                j["gas.station"] = json::object();
            }

            j["gas.station"]["amount_trk"] = amount;

            std::ofstream outFile(configPath);
            if (!outFile.is_open()) {
                std::cerr << "[Config] CRITICAL ERROR: Could not open file for writing: " << configPath << std::endl;
                return;
            }
            outFile << j.dump(4);

            std::cout << "[Config] Saved amount_trk: " << amount << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "[Config] Error saving TRK amount to json: " << e.what() << std::endl;
        }
    }

    void utility::saveFuelPriceToConfig(const std::string& configPath, const std::string& fuel_id, const std::string& price_str) {
        try {
            json j;
            std::ifstream inFile(configPath);

            if (inFile.is_open()) {
                inFile >> j;
                inFile.close();
            } else {
                j = json::object();
            }

            if (!j.contains("gas.station") || !j["gas.station"].is_object()) {
                j["gas.station"] = json::object();
            }

            if (!j["gas.station"].contains("fuel_price") || !j["gas.station"]["fuel_price"].is_object()) {
                j["gas.station"]["fuel_price"] = json::object();
            }

            j["gas.station"]["fuel_price"][fuel_id] = price_str;

            std::ofstream outFile(configPath);
            if (!outFile.is_open()) {
                std::cerr << "[Config] CRITICAL ERROR: Could not open file for writing: " << configPath << std::endl;
                return;
            }
            outFile << j.dump(4);
            std::cout << "[Config] Saved price for " << fuel_id << " -> " << price_str << std::endl;

        } catch (const json::exception& e) {
            std::cerr << "[Config] JSON Error while saving fuel price: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[Config] General Error while saving fuel price: " << e.what() << std::endl;
        }
    }

    int utility::getPaginationIndex(int current_index, int size, int direction) {
        int index = current_index + direction;
        if (index >= size) {
            index = 0;
        } else if (index < 0) {
            index = size - 1;
        }
        return index;
    }

    void utility::saveUsedTRKsToConfig(const std::string& configPath, const std::vector<std::string>& selected_trks) {
        try {
            json j;
            std::ifstream inFile(configPath);

            if (inFile.is_open()) {
                inFile >> j;
                inFile.close();
            } else {
                j = json::object();
            }

            // Проверяем наличие базовой структуры
            if (!j.contains("gas.station") || !j["gas.station"].is_object()) {
                j["gas.station"] = json::object();
            }

            // Магия nlohmann::json: он сам превратит вектор С++ в массив JSON ["trk2", "trk1"]
            j["gas.station"]["used_trks"] = selected_trks;

            std::ofstream outFile(configPath);
            if (!outFile.is_open()) {
                std::cerr << "[Config] CRITICAL ERROR: Could not open file for writing: " << configPath << std::endl;
                return;
            }

            outFile << j.dump(4);

            std::cout << "[Config] Saved " << selected_trks.size() << " used TRKs to config." << std::endl;

        } catch (const json::exception& e) {
            std::cerr << "[Config] JSON Error while saving used TRKs: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[Config] General Error while saving used TRKs: " << e.what() << std::endl;
        }
    }

    std::string utility::primeIdToProcessingId(const std::string& prime_product_id,
                                                const std::map<std::string, std::string>& prime_standalone,
                                                const std::map<std::string, std::string>& processing_standalone) {
        for (const auto& pair : prime_standalone) {
            if (pair.second == prime_product_id) {
                auto it = processing_standalone.find(pair.first);
                if (it != processing_standalone.end()) {
                    return it->second;
                }
            }
        }
        return prime_product_id;
    }
}

