//
// Created by vadim.tissen on 09.12.2025.
//
#pragma once
#include <string>
#include <map>


#include "Settings.hpp"
#include "bridge/core/types.hpp"

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

        // Конвертирует prime product_id в processing product_id.
        static std::string primeIdToProcessingId(const std::string& prime_product_id,
                                                  const std::map<std::string, std::string>& prime_standalone,
                                                  const std::map<std::string, std::string>& processing_standalone);

        // Форматирует значение с decimal в строку с точкой
        static std::string formatDecimalValue(uint32_t value, uint8_t decimal);

        // Создает чек для печати из данных транзакции
        static std::string createReceiptFromTransaction(
            uint64_t transaction_id,
            const std::string& product_id,
            uint32_t price_value, uint8_t price_decimal,
            uint32_t volume_value, uint8_t volume_decimal,
            uint32_t amount_value, uint8_t amount_decimal,
            uint32_t fact_volume_value, uint8_t fact_volume_decimal,
            uint32_t fact_amount_value, uint8_t fact_amount_decimal,
            bool not_complete,
            uint64_t transaction_time
        );

        static std::string createFirstReceiptFromLevelGauge(
            const LevelGauge& level_gauge,
            const Tanker& tanker,
            const std::string& product_name,
            const std::string& time
        );

        static std::string createSecondReceiptFromLevelGauge(
            const std::string& product_name,
            const Tanker& tanker,
            const LevelGauge& level_gauge_current,
            const LevelGauge& level_gauge_initial,
            const std::string& document_number,
            const std::string& time_beginning,
            const std::string& time_ending
            );
    };
}
