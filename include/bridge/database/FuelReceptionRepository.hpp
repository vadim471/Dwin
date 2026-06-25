#ifndef DWIN_EXMPL_FUELRECEPTIONREPOSITORY_HPP
#define DWIN_EXMPL_FUELRECEPTIONREPOSITORY_HPP

#include <string>
#include <cstdint>
#include <memory>
#include <vector>

#include "Database.hpp"

namespace bridge {

// Структура для хранения данных о приеме топлива
struct FuelReceptionRecord {
    int64_t id = 0;
    std::string date;               // ISO 8601 формат
    std::string gauge_id;           // ID уровнемера
    std::string gauge_name;         // Название резервуара
    std::string fuel_name;          // Название топлива
    std::string document_number;    // Номер накладной
    std::string id_tso;
    
    // Показания до приема
    double before_upper_level = 0.0;
    double before_lower_level = 0.0;
    double before_upper_volume = 0.0;
    double before_lower_volume = 0.0;
    double before_total_volume = 0.0;
    double before_temperature = 0.0;
    double before_weight = 0.0;
    double before_density = 0.0;
    double before_filling = 0.0;
    
    // Показания после приема
    double after_upper_level = 0.0;
    double after_lower_level = 0.0;
    double after_upper_volume = 0.0;
    double after_lower_volume = 0.0;
    double after_total_volume = 0.0;
    double after_temperature = 0.0;
    double after_weight = 0.0;
    double after_density = 0.0;
    double after_filling = 0.0;
    
    double received_liters = 0.0;   // Фактически принято литров
    
    std::string time_beginning;     // Время начала приема
    std::string time_ending;        // Время окончания приема
    
    bool sent_to_server = false;    // Отправлено на сервер
    std::string created_at;         // Время создания записи
};

class FuelReceptionRepository {
public:
    explicit FuelReceptionRepository(std::shared_ptr<Database> db);

    void createTable();

    int64_t insert(const FuelReceptionRecord& record);

    FuelReceptionRecord getById(int64_t id);

    std::vector<FuelReceptionRecord> getAll();

    std::vector<FuelReceptionRecord> getByGaugeId(const std::string& gauge_id);

    std::vector<FuelReceptionRecord> getUnsent();

    void markAsSent(int64_t id);

    void deleteById(int64_t id);

    int64_t count();

private:
    std::shared_ptr<Database> db_;
    
    FuelReceptionRecord mapFromStatement(sqlite3_stmt* stmt);
};

} // namespace bridge

#endif // DWIN_EXMPL_FUELRECEPTIONREPOSITORY_HPP
