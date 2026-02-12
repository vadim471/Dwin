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
        static std::string replaceIdInUrl(std::string url, const std::string& placeholder, const std::string &id);
        static std::vector<uint8_t> floatToBytes(float value);
        static std::vector<uint8_t> stringToBytes(const std::string& str, size_t fixedLength);
        static float parseFloatFromJson(const json& node);
        static std::string parseStringFromJson(const json& node);
        static int parseIntFromJson(const json& node);
        static bool parseBoolFromJson(const json& node);

        // Возвращает icon id dwin соответствующее статусу.
        static int getIconForStatus(const std::string& status, const Settings& s);

        // Возвращает id из url.
        static std::string getIdFromUrl(const std::string& url, const std::string& marker);

        // Возвращает Dispenser из std::vector по string id.
        static Dispenser* getDispenserById(std::vector<Dispenser>& dispensers, const std::string& id);
        static std::string getFormattedStringFromJson(std::string valueStr, int exponent);

        // Извлекает первое встреченное число из строки. Минус не переваривает.
        static std::string extractFirstInt(const std::string &str);
        static int getIconForProduct(const std::string& product, const Settings& s);

        // Возвращает Product из std::vector по string id.
        static const Product* getProductById(const std::vector<Product>& products, const std::string& id);

        // Возвращает std::string для вывода времени в Footer.
        static const std::string getCurrentTimeString();
    };
}
