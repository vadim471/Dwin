#ifndef DWIN_EXMPL_METROLOGICAL_RECORD_HPP
#define DWIN_EXMPL_METROLOGICAL_RECORD_HPP

#include <string>
#include <cstdint>
#include <memory>
#include "Database.hpp"

namespace bridge {

struct MetrologicalRecordData {
    int64_t id = 0;
    std::string date;
    std::string density;
    std::string filling;
    std::string fuelName;
    std::string idTso;
    std::string lowerLevel;
    std::string lowerVolume;
    std::string namePmp;
    std::string temperature;
    std::string totalVolume;
    std::string upperLevel;
    std::string upperVolume;
    std::string weight;

    std::string createdAt;
};

class MetrologicalRecordRepository {
public:
    explicit MetrologicalRecordRepository(std::shared_ptr<Database> db);

    void createTable();

    int64_t insert(const MetrologicalRecordData& record);

    MetrologicalRecordData getById(int64_t id);

    std::vector<MetrologicalRecordData> getAll();

    std::vector<MetrologicalRecordData> getByIdTso(const std::string& idTso);

    std::vector<MetrologicalRecordData> getByFuelName(const std::string& fuelName);

    std::vector<MetrologicalRecordData> getByNamePmp(const std::string& namePmp);

    std::vector<MetrologicalRecordData> getByDateRange(const std::string& startDate, const std::string& endDate);

    void update(const MetrologicalRecordData& record);

    void deleteById(int64_t id);
    
    int64_t count();

private:
    std::shared_ptr<Database> db_;
    
    MetrologicalRecordData mapFromStatement(sqlite3_stmt* stmt);
};

} // namespace bridge

#endif // DWIN_EXMPL_METROLOGICAL_RECORD_HPP
