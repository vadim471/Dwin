//
// Created by vadim.tissen on 09.12.2025.
//


#include "bridge/core/utility.hpp"

#include <iomanip>
#include <ios>
#include <string>
#include <regex>
#include <ctime>
#include "bridge/core/types.hpp"
#include <algorithm>
#include <iconv.h>

#include "bridge/payment/CardData.hpp"

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
        if (status == DISPENSER_ERROR) {
            return s.dwin.icon_dispenser_error;
        }
        if (status == DISPENSER_HALTED) {
            return s.dwin.icon_dispenser_halted;
        }
        if (status == DISPENSER_OFFLINE) {
            return s.dwin.icon_dispenser_offline;
        }
        if (status == DISPENSER_LOCKED) {
            return s.dwin.icon_dispenser_locked;;
        }
        if (status == DISPENSER_FUELLING) {
            return s.dwin.icon_dispenser_fueling;
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

    std::string utility::getLevelGaugeIdByProductId(const std::vector<Tanker>& tankers, const std::string& product_id) {
        for (const auto& tanker : tankers) {

            if (tanker.product_id == product_id) {
                if (!tanker.level_gauge_ids.empty()) {
                    return tanker.level_gauge_ids.front();
                }
            }
        }
        return "";
    }

    std::string utility::getTimeStringForBos(uint64_t time) {
        time_t raw_time = static_cast<time_t>(time);

        struct tm* dt = std::gmtime(&raw_time);
        char buffer[20];

        std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", dt);

        return std::string(buffer);
    }

    std::string utility::formatDoubleWithComma(double value, int precision) {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(precision) << value;
        std::string formatted = stream.str();
        std::replace(formatted.begin(), formatted.end(), '.', ',');

        return formatted;
    }

    std::string utility::formatLimitType(const std::string& type) {
        if (type == "суточного") return "сут.";
        if (type == "месячного") return "мес.";
        if (type == "недельного") return "нед.";
        if (type == "годового") return "год.";
        return type;
    }

    ParsedReceipt utility::parseTerminalBalanceReceipt(const std::string& receipt_text) {
        ParsedReceipt data;
        std::smatch match;

        std::regex cardRegex("№:\\s*([0-9 ]+)");
        if (std::regex_search(receipt_text, match, cardRegex)) {
            std::string cardNum = match[1].str();

            if (!cardNum.empty()) {
                cardNum.erase(cardNum.find_last_not_of(" ") + 1);
            }

            data.card_number = cardNum;
        }

        std::regex walletRegex("Кошелек\\s+([^\\s]+)");
        if (std::regex_search(receipt_text, match, walletRegex)) {
            data.wallet_type = match[1].str();
        }

        // Если баланс скрыт - парсить нечего.
        if (receipt_text.find(HIDDEN_CARD_BALANCE_OPTIMA_PC) != std::string::npos) {
            data.hidden_balance = true;
        } else {
            data.hidden_balance = false;
            std::regex balanceRegex("баланс:\\s*([\\d\\s\\xC2\\xA0,]+)");
            if (std::regex_search(receipt_text, match, balanceRegex)) {
                std::string rawBalance = match[1].str();

                rawBalance.erase(std::remove(rawBalance.begin(), rawBalance.end(), ' '), rawBalance.end());

                size_t pos;
                while ((pos = rawBalance.find("\xC2\xA0")) != std::string::npos) {
                    rawBalance.erase(pos, 2);
                }

                std::replace(rawBalance.begin(), rawBalance.end(), ',', '.');
                data.balance_before = rawBalance;
            }

            std::regex limitRegex("Остаток\\s+(суточного|месячного|недельного|годового)\\s+лимита\\s*\\.+\\s*([\\d,]+)\\s*([^\\s\\.]+)");

            auto words_begin = std::sregex_iterator(receipt_text.begin(), receipt_text.end(), limitRegex);
            auto words_end = std::sregex_iterator();

            std::stringstream ss;
            bool found_any_limit = false;

            for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
                std::smatch match = *i;
                std::string limitType = match[1].str();
                std::string valueStr = match[2].str();
                std::string unitStr = match[3].str();

                std::replace(valueStr.begin(), valueStr.end(), ',', '.');
                double val = 0.0;
                try { val = std::stod(valueStr); } catch (...) {}

                if (found_any_limit) ss << "\n";

                ss << "Ост. " << formatLimitType(limitType) << " лимита: "
                   << std::fixed << std::setprecision(2) << val << unitStr;

                found_any_limit = true;
            }

            if (found_any_limit) {
                data.limit_info = ss.str();
            }
        }

        return data;
    }


    std::string utility::getWalletSymbol(const std::string& wallet_type) {
        if (wallet_type == "Литровый") {
            return "л.";
        }
        if (wallet_type == "Денежный") {
            return "р.";
        }
        return ""; // По умолчанию или если тип неизвестен
    }

    std::string utility::convertUtf8ToCp1251(const std::string& utf8_str) {
        iconv_t cd = iconv_open("CP1251", "UTF-8");
        if (cd == (iconv_t)-1) {
            return utf8_str; // Если кодировка не поддерживается ОС, возвращаем как есть
        }

        size_t in_bytes_left = utf8_str.length();
        char* in_buf = const_cast<char*>(utf8_str.data());

        // CP1251 занимает максимум 1 байт на символ, поэтому буфера размера UTF-8 хватит с запасом
        std::vector<char> out_buffer(in_bytes_left + 1, 0);
        size_t out_bytes_left = out_buffer.size() - 1;
        char* out_buf = out_buffer.data();

        if (iconv(cd, &in_buf, &in_bytes_left, &out_buf, &out_bytes_left) == (size_t)-1) {
            iconv_close(cd);
            return utf8_str; // При ошибке конвертации возвращаем оригинал
        }

        iconv_close(cd);
        return std::string(out_buffer.data());
    }

    //"             OОО ТОС\n                КАЗС-1\n               г. Мурманск\n\n
    //Номер карты:  000038\n
    //Операция:     ДЕБЕТ\n
    //Продукт:      ДТ\n
    //Кол-во, лит.: 290.000\n
    //Кошелек:      ЛИТРОВЫЙ\n
    //-------------- Баланс, лит. ------------\n
    //до операции        |  после операции\n
    //+984855.16         |  +984565.16\n
    //----------------------------------------\n
    //Баланс:             984565.16\n
    //Лимит операции:     0.00 лит.\n
    //Ост. сут. лимита:   10000.00 лит.\n
    //Ост. мес. лимита:   0.00 лит.\n\n
    //Цена за литр : 1.00 руб.\n
    //Номер чека   : 313760\n
    //2026-05-05 13:21:19 \n \n \n \n \n \n \n \n \n \n\n-------------------------------\n"
    std::string utility::getFormattedReceiptForBos(const std::string& address,
        const std::string& card_number,
        const std::string& operation,
        const std::string& product_name,
        double volume_liters,
        double price_per_liter,
        uint64_t transaction_id,
        uint64_t receipt_time) {

        std::stringstream receipt;

        receipt << "             " << address;
        receipt << "Номер карты: " << card_number << "\n";
        receipt << "Операция: " << operation << "\n";

        receipt << "Продукт: " << product_name << "\n";
        receipt << "Количество литров: " << std::fixed << std::setprecision(2) << volume_liters << " л.\n";
        receipt << "Цена за литр: " << std::fixed << std::setprecision(2) << price_per_liter << " руб.\n";
        receipt << "Номер транзакции: " << transaction_id << "\n\n";
        receipt << "             " << getTimeStringForBos(receipt_time) << "\n \n \n \n \n \n \n \n \n \n\n-------------------------------\n";

        return receipt.str();
    }

    const std::string utility::getCurrentTimeString() {
        std::time_t now = std::time(nullptr);
        std::tm* local_time = std::localtime(&now);

        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y.%m.%d %H:%M:%S", local_time);

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

            if (!j.contains("gas.station") || !j["gas.station"].is_object()) {
                j["gas.station"] = json::object();
            }
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

    std::string utility::formatDecimalValue(uint32_t value, uint8_t decimal) {
        if (decimal == 0) {
            return std::to_string(value);
        }

        std::string str = std::to_string(value);
        
        // Добавляем нули спереди если нужно
        while (str.length() <= decimal) {
            str = "0" + str;
        }

        // Вставляем точку
        size_t pos = str.length() - decimal;
        str.insert(pos, ".");

        return str;
    }

    double utility::calculateFuelLiters(const std::string& before,
                           const std::string& after)
    {
        double beforeValue = std::stod(before);
        double afterValue  = std::stod(after);

        // 1 м3 = 1000 литров
        return (afterValue - beforeValue) * 1000.0;
    }

    //std::string utility::createReceiptFromBank()

    // std::string utility::createReceiptFromTransaction(
    //     uint64_t transaction_id,
    //     const std::string& product_id,
    //     uint32_t price_value, uint8_t price_decimal,
    //     uint32_t volume_value, uint8_t volume_decimal,
    //     uint32_t amount_value, uint8_t amount_decimal,
    //     uint32_t fact_volume_value, uint8_t fact_volume_decimal,
    //     uint32_t fact_amount_value, uint8_t fact_amount_decimal,
    //     bool not_complete,
    //     uint64_t transaction_time
    // ) {
    //     std::stringstream receipt;
    //
    //     receipt << "========================================\n";
    //     receipt << "           ЧЕК ТРАНЗАКЦИИ\n";
    //     receipt << "========================================\n\n";
    //
    //     time_t time = static_cast<time_t>(transaction_time / 1000);
    //     char time_buf[64];
    //     strftime(time_buf, sizeof(time_buf), "%d.%m.%Y %H:%M:%S", localtime(&time));
    //     receipt << "Дата: " << time_buf << "\n";
    //     receipt << "ID транзакции: " << transaction_id << "\n\n";
    //
    //     receipt << "Товар: " << product_id << "\n";
    //     receipt << "Цена за литр: " << formatDecimalValue(price_value, price_decimal) << " руб.\n\n";
    //
    //     if (not_complete) {
    //         receipt << "--- Запрошено ---\n";
    //         receipt << "Объем: " << formatDecimalValue(volume_value, volume_decimal) << " л.\n";
    //         receipt << "Сумма: " << formatDecimalValue(amount_value, amount_decimal) << " руб.\n\n";
    //     }
    //
    //     receipt << "--- Отпущено ---\n";
    //     receipt << "Объем: " << formatDecimalValue(fact_volume_value, fact_volume_decimal) << " л.\n";
    //     receipt << "Сумма: " << formatDecimalValue(fact_amount_value, fact_amount_decimal) << " руб.\n\n";
    //
    //     receipt << "========================================\n";
    //     receipt << "        Спасибо за покупку!\n";
    //     receipt << "========================================\n";
    //
    //     return receipt.str();
    // }
    std::string utility::calculateRevertFuelString(uint32_t volume_value_begin, uint8_t volume_decimal_begin,
                                          uint32_t volume_value_end, uint8_t volume_decimal_end) {

        double ordered = static_cast<double>(volume_value_begin) / std::pow(10, volume_decimal_begin);
        double dispensed = static_cast<double>(volume_value_end) / std::pow(10, volume_decimal_end);

        double revert = ordered - dispensed;

        if (revert < 0.0) {
            revert = 0.0;
        }

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << revert;
        std::string result = oss.str();

        std::replace(result.begin(), result.end(), '.', ',');

        return result;
    }

    std::string utility::createSecondReceiptFromLevelGauge(
        const std::string& product_name,
        const Tanker& tanker,
        const LevelGauge& level_gauge_current,
        const LevelGauge& level_gauge_initial,
        const std::string& document_number,
        const std::string& time_beginning,
        const std::string& time_ending
        ) {
        std::stringstream receipt;


        receipt << "      " << getCurrentTimeString() << "\n";
        receipt << "========================================\n\n";
        receipt << "      " << TITLE_ENDING_RECEPTION_FUEL << "\n\n";
        receipt << "========================================\n\n";

        if (!product_name.empty()) {
            receipt << "Топливо" << "\n" << product_name << "\n";
        } else if (!tanker.product_id.empty()) {
            receipt << "Топливо ID" << "\n" << tanker.product_id << "\n";
        }

        receipt << "Уровнемер" << "\n" << level_gauge_current.id << "\n";

        receipt << "Номер накладной" << "\n" << document_number << "\n";

        receipt << "Фактически принято" << "\n" << calculateFuelLiters(level_gauge_initial.total_volume, level_gauge_current.total_volume) << "\n";

        receipt << "Начало приема топлива" << "\n" << time_beginning << "\n";

        receipt << "Окончание приема топлива" << "\n" << time_ending << "\n";

        receipt << "========================================\n\n";
        receipt << TITLE_ENDING_BEFORE_RECEPTION_FUEL << "\n";
        receipt << "========================================\n\n";

        receipt << "Уровень основного поплавка" << "\n" << level_gauge_initial.upper_level << " (м)\n";

        receipt << "Уровень нижнего поплавка: " << "\n" << level_gauge_initial.lower_level << " (м)\n";

        receipt << "Объем основного продукта" << "\n" << level_gauge_initial.upper_volume << " (м3)\n";

        receipt << "Объем подтоварной жидкости" << "\n" << level_gauge_initial.lower_volume << " (м3)\n";

        receipt << "Общий объем" << "\n" << level_gauge_initial.total_volume << " (м3)\n";

        receipt << "Средняя температура" << "\n" << 0 << " (С)\n";

        receipt << "Масса" << "\n" << level_gauge_initial.weight << " (кг)\n";

        receipt << "Плотность" << "\n" << level_gauge_initial.density << " (кг/м3)\n";

        receipt << "Заполнение" << "\n" << level_gauge_initial.filling << " (%)\n";

        receipt << "========================================\n\n";
        receipt << TITLE_ENDING_AFTER_RECEPTION_FUEL << "\n";
        receipt << "========================================\n\n";

        receipt << "Уровень основного поплавка" << "\n" << level_gauge_current.upper_level << " (м)\n";

        receipt << "Уровень нижнего поплавка: " << "\n" << level_gauge_current.lower_level << " (м)\n";

        receipt << "Объем основного продукта" << "\n" << level_gauge_current.upper_volume << " (м3)\n";

        receipt << "Объем подтоварной жидкости" << "\n" << level_gauge_current.lower_volume << " (м3)\n";

        receipt << "Общий объем" << "\n" << level_gauge_current.total_volume << " (м3)\n";

        receipt << "Средняя температура" << "\n" << 0 << " (С)\n";

        receipt << "Масса" << "\n" << level_gauge_current.weight << " (кг)\n";

        receipt << "Плотность" << "\n" << level_gauge_current.density << " (кг/м3)\n";

        receipt << "Заполнение" << "\n" << level_gauge_current.filling << " (%)\n";

        receipt << "========================================\n";

        return receipt.str();
    }

    std::string utility::createFirstReceiptFromLevelGauge(
        const LevelGauge& level_gauge,
        const Tanker& tanker,
        const std::string& product_name,
        const std::string& time
    ) {
        std::stringstream receipt;

        receipt << "      " << time << "\n";
        receipt << "========================================\n\n";
        receipt << "      " << TITLE_BEGINNING_RECEPTION_FUEL << "\n\n";
        receipt << "========================================\n\n";


        if (!product_name.empty()) {
            receipt << "Топливо" << "\n" << product_name << "\n";
        } else if (!tanker.product_id.empty()) {
            receipt << "Топливо ID" << "\n" << tanker.product_id << "\n";
        }

        receipt << "Уровнемер" << "\n" << level_gauge.id << "\n";

        receipt << "Уровень основного поплавка" << "\n" << level_gauge.upper_level << " (м)\n";

        receipt << "Уровень нижнего поплавка: " << "\n" << level_gauge.lower_level << " (м)\n";

        receipt << "Объем основного продукта" << "\n" << level_gauge.upper_volume << " (м3)\n";

        receipt << "Объем подтоварной жидкости" << "\n" << level_gauge.lower_volume << " (м3)\n";

        receipt << "Общий объем" << "\n" << level_gauge.total_volume << " (м3)\n";

        receipt << "Средняя температура" << "\n" << 0 << " (С)\n";

        receipt << "Масса" << "\n" << level_gauge.weight << " (кг)\n";

        receipt << "Плотность" << "\n" << level_gauge.density << " (кг/м3)\n";

        receipt << "Заполнение" << "\n" << level_gauge.filling << " (%)\n";

        receipt << "========================================\n";

        return receipt.str();
    }

    std::string utility::createDebitReceipt(
        const std::string& address,
        const std::string& card_number,
        const std::string& product_name,
        const std::string& wallet_type,
        double refund_volume,
        double price_per_liter,
        uint64_t transaction_id,

        std::string* balance_before,
        std::string* balance_after,
        std::string* limit_info
    ) {
        std::stringstream receipt;

        receipt << address << "\n\n";

        receipt << "Номер карты: " << card_number << "\n";
        receipt << "Операция: ВОЗВРАТ\n\n";

        receipt << "Продукт: " << product_name << "\n";
        receipt << "Кол-во, лит: " << std::fixed << std::setprecision(2) << refund_volume << "\n";
        receipt << "Кошелек: " << wallet_type << "\n";

        if (balance_after) {
            receipt << "--------------" << "Баланс" << "--------------" << "\n";
            receipt << "до операции | после операции" << "\n";

            receipt << (balance_before ? *balance_before : "N/A") << " | " << *balance_after << "\n\n";

            receipt << "--------------" << "----" << "--------------" << "\n";
            receipt << "Баланс: " << *balance_after << "\n";

            if (limit_info && !limit_info->empty()) {
                receipt << *limit_info << "\n";
            }
        }

        receipt << "Цена за литр: " << std::fixed << std::setprecision(2) << price_per_liter << " руб.\n";
        receipt << "Номер транзакции: " << transaction_id << "\n\n";

        time_t now = std::time(nullptr);
        char time_buf[64];
        strftime(time_buf, sizeof(time_buf), "%d.%m.%Y %H:%M:%S", localtime(&now));
        receipt << time_buf << "\n\n";


        return receipt.str();
    }

    std::string utility::createRefundReceipt(
        const std::string& address,
        const std::string& card_number,
        const std::string& product_name,
        std::string& wallet_type,
        double refund_volume,

        std::string* balance_before,
        std::string* balance_after,
        std::string* limit_info
    ) {
        std::stringstream receipt;


        receipt << address << "\n\n";

        receipt << "Номер карты: " << card_number << "\n";
        receipt << "Операция: ВОЗВРАТ\n\n";

        receipt << "Продукт: " << product_name << "\n";
        receipt << "Кол-во, лит: " << std::fixed << std::setprecision(2) << refund_volume << "\n";
        receipt << "Кошелек: " << wallet_type << "\n";

        if (balance_after) {
            receipt << "--------------" << "Баланс" << "--------------" << "\n";
            receipt << "до операции | после операции" << "\n";

            receipt << (balance_before ? *balance_before : "N/A") << " | " << *balance_after << "\n\n";

            receipt << "--------------" << "----" << "--------------" << "\n";
            receipt << "Баланс: " << *balance_after << "\n";

            if (limit_info && !limit_info->empty()) {
                receipt << *limit_info << "\n";
            }
        }

        time_t now = std::time(nullptr);
        char time_buf[64];
        strftime(time_buf, sizeof(time_buf), "%d.%m.%Y %H:%M:%S", localtime(&now));
        receipt << time_buf << "\n\n";


        return receipt.str();
    }

    std::pair<std::string, std::string> utility::splitByFirstSpace(const std::string& str) {
        size_t spacePos = str.find(' ');

        if (spacePos != std::string::npos) {
            std::string first_part = str.substr(0, spacePos);
            std::string second_part = str.substr(spacePos + 1);

            second_part.erase(std::remove(second_part.begin(), second_part.end(), ' '), second_part.end());

            return std::make_pair(first_part, second_part);
        }

        return std::make_pair(str, "");
    }

    double utility::stringToDouble(std::string s) {
        std::replace(s.begin(), s.end(), ',', '.');
        return std::stod(s);
    }
}

