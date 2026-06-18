#ifndef DWIN_EXMPL_LEVELGAUGEREPOSITORY_HPP
#define DWIN_EXMPL_LEVELGAUGEREPOSITORY_HPP

#include <string>
#include <cstdint>
#include <memory>
#include <vector>

#include "Database.hpp"

namespace bridge {

// Структура для хранения показаний уровнемера
struct LevelGaugeData {
    int64_t id = 0;
    std::string date;            // ISO 8601 формат
    std::string timestamp;       // Unix timestamp в миллисекундах
    std::string gauge_id;        // ID уровнемера
    std::string gauge_name;      // Название резервуара
    std::string fuel_name;       // Название топлива
    std::string upper_level;     // Уровень основного поплавка (м)
    std::string lower_level;     // Уровень нижнего поплавка (м)
    std::string upper_volume;    // Объем основного продукта (м3)
    std::string lower_volume;    // Объем подтоварной жидкости (м3)
    std::string total_volume;    // Общий объем (м3)
    std::string temperature;     // Температура (°C)
    std::string weight;          // Масса (кг)
    std::string density;         // Плотность (кг/м3)
    std::string filling;         // Заполнение (%)
    bool sent_to_server = false; // Отправлено на сервер
    std::string created_at;      // Время создания записи
};

class LevelGaugeRepository {
public:
    explicit LevelGaugeRepository(std::shared_ptr<Database> db);

    void createTable();

    int64_t insert(const LevelGaugeData& record);

    LevelGaugeData getById(int64_t id);

    std::vector<LevelGaugeData> getAll();

    std::vector<LevelGaugeData> getByGaugeId(const std::string& gauge_id);

    std::vector<LevelGaugeData> getUnsent();

    void markAsSent(int64_t id);

    void deleteById(int64_t id);

    int64_t count();

private:
    std::shared_ptr<Database> db_;
    
    LevelGaugeData mapFromStatement(sqlite3_stmt* stmt);
};

} // namespace bridge

#endif // DWIN_EXMPL_LEVELGAUGEREPOSITORY_HPP
