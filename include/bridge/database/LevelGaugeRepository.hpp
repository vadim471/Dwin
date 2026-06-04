#ifndef DWIN_EXMPL_LEVELGAUGEREPOSITORY_HPP
#define DWIN_EXMPL_LEVELGAUGEREPOSITORY_HPP

#include <string>
#include <cstdint>
#include <memory>
#include <vector>

#include "Database.hpp"

namespace bridge {

// Структура для хранения показаний уровнемера
struct LevelGaugeRecord {
    int64_t id = 0;
    std::string date;           // ISO 8601 формат
    int64_t timestamp = 0;      // Unix timestamp в миллисекундах
    std::string gauge_id;       // ID уровнемера
    std::string gauge_name;     // Название резервуара
    std::string fuel_name;      // Название топлива
    double upper_level = 0.0;   // Уровень основного поплавка (м)
    double lower_level = 0.0;   // Уровень нижнего поплавка (м)
    double upper_volume = 0.0;  // Объем основного продукта (м3)
    double lower_volume = 0.0;  // Объем подтоварной жидкости (м3)
    double total_volume = 0.0;  // Общий объем (м3)
    double temperature = 0.0;   // Температура (°C)
    double weight = 0.0;        // Масса (кг)
    double density = 0.0;       // Плотность (кг/м3)
    double filling = 0.0;       // Заполнение (%)
    bool sent_to_server = false; // Отправлено на сервер
    std::string created_at;     // Время создания записи
};

class LevelGaugeRepository {
public:
    explicit LevelGaugeRepository(std::shared_ptr<Database> db);

    void createTable();

    int64_t insert(const LevelGaugeRecord& record);

    LevelGaugeRecord getById(int64_t id);

    std::vector<LevelGaugeRecord> getAll();

    std::vector<LevelGaugeRecord> getByGaugeId(const std::string& gauge_id);

    std::vector<LevelGaugeRecord> getUnsent();

    void markAsSent(int64_t id);

    void deleteById(int64_t id);

    int64_t count();

private:
    std::shared_ptr<Database> db_;
    
    LevelGaugeRecord mapFromStatement(sqlite3_stmt* stmt);
};

} // namespace bridge

#endif // DWIN_EXMPL_LEVELGAUGEREPOSITORY_HPP
