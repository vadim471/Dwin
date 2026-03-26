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
        static double parseDoubleFromJson(const std::string& value_str, int exponent);

        // Возвращает icon id dwin соответствующее статусу.
        static int getIconForStatus(const std::string& status, const Settings& s);

        // Возвращает icon id dwin, соответсующее количеству ТРК на АЗС.
        static int getIconForGasStation(int amount, const Settings& s);

        // Возвращает id из url.
        static std::string getIdFromUrl(const std::string& url, const std::string& marker);

        // Возвращает Dispenser из std::vector по string id.
        static Dispenser* getDispenserById(std::vector<Dispenser>& dispensers, const std::string& id);
        static std::string getFormattedStringFromJson(std::string valueStr, int exponent);

        // Извлекает первое встреченное число из строки. Минус не переваривает.
        static std::string extractFirstInt(const std::string &str);

        // Получить id иконки dwin через product->rating.
        static int getIconForProduct(const std::string& product, const Settings& s);

        // Возвращает Product из std::vector по string id.
        static Product* getProductById(std::vector<Product>& products, const std::string& id);

        // Возвращает LevelGauge из std::vector по string id.
        static LevelGauge *getLevelGaugeById(std::vector<LevelGauge> &gauges, const std::string &id);

        // Возвращает Tanker из std::vector по id привязанного LevelGauge.
        static const Tanker* getTankerByLevelGaugeId(const std::vector<Tanker>& tankers, const std::string& gauge_id);

        // Возвращает std::string для вывода времени в Footer.
        static const std::string getCurrentTimeString();

        // Возвращает целую и дробную часть строки, разделенной по "," или ".".
        static std::pair<std::string, std::string> splitFloatString(const std::string& floatStr, size_t lenFrac);

        // Возвращает информацию, заполнена ли цена у всех видов топлива.
        static bool checkAllFuelPrice(std::vector<Product> &products);

        // Запись количества ТРК в файл конфигурации.
        static void saveTRKAmountToConfig(const std::string& configPath, int amount);

        static void saveFuelPriceToConfig(const std::string& configPath, const std::string& fuel_id, const std::string& price_str);

        // Превращает строку вида "63,6" в int, округление вверх.
        static int ceilStringToInt(std::string value_str);

        // Возваращет из строки вида "63.6" int 636, int 1.
        static std::tuple<int, int> formatFloatStringToInt(std::string value_str);

        // Возвращает индекс следующего элемента после нажатия кнопок пагинаци.
        static int getPaginationIndex(int current_index, int size, int direction);

        // Заполняет информацию об используемых ТРК в файл конфигурации.
        static void saveUsedTRKsToConfig(const std::string& configPath, const std::vector<std::string>& selected_trks);
    };
}
