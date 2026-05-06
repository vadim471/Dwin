#include "bridge/MetrologicalRecord.hpp"
#include "bridge/Logger.hpp"

namespace bridge {

MetrologicalRecordRepository::MetrologicalRecordRepository(std::shared_ptr<Database> db)
    : db_(db) {
}

void MetrologicalRecordRepository::createTable() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS metrological_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            date TEXT NOT NULL,
            density TEXT,
            filling TEXT,
            fuel_name TEXT,
            id_tso TEXT,
            lower_level TEXT,
            lower_volume TEXT,
            name_pmp TEXT,
            temperature TEXT,
            total_volume TEXT,
            upper_level TEXT,
            upper_volume TEXT,
            weight TEXT,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
    )";
    
    db_->execute(sql);
    
    // Создать индексы для быстрого поиска
    db_->execute("CREATE INDEX IF NOT EXISTS idx_id_tso ON metrological_records(id_tso);");
    db_->execute("CREATE INDEX IF NOT EXISTS idx_fuel_name ON metrological_records(fuel_name);");
    db_->execute("CREATE INDEX IF NOT EXISTS idx_name_pmp ON metrological_records(name_pmp);");
    db_->execute("CREATE INDEX IF NOT EXISTS idx_date ON metrological_records(date);");
    db_->execute("CREATE INDEX IF NOT EXISTS idx_created_at ON metrological_records(created_at);");
    
    LOG_SYSTEM_INFO << "Metrological records table created/verified";
}

int64_t MetrologicalRecordRepository::insert(const MetrologicalRecordData& record) {
    const char* sql = R"(
        INSERT INTO metrological_records (
            date, density, filling, fuel_name, id_tso,
            lower_level, lower_volume, name_pmp, temperature,
            total_volume, upper_level, upper_volume, weight
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
    )";
    
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_text(stmt, 1, record.date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, record.density.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, record.filling.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, record.fuelName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, record.idTso.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, record.lowerLevel.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, record.lowerVolume.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, record.namePmp.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, record.temperature.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 10, record.totalVolume.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, record.upperLevel.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 12, record.upperVolume.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 13, record.weight.c_str(), -1, SQLITE_TRANSIENT);
    
    int rc = sqlite3_step(stmt);
    db_->finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        LOG_SYSTEM_ERROR << "Failed to insert metrological record";
        throw DatabaseException("Failed to insert metrological record");
    }
    
    int64_t id = db_->getLastInsertId();
    LOG_SYSTEM_INFO << "Metrological record inserted with ID: " << id;
    
    return id;
}

MetrologicalRecordData MetrologicalRecordRepository::mapFromStatement(sqlite3_stmt* stmt) {
    MetrologicalRecordData data;
    
    data.id = sqlite3_column_int64(stmt, 0);
    
    const unsigned char* text;
    
    text = sqlite3_column_text(stmt, 1);
    if (text) data.date = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 2);
    if (text) data.density = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 3);
    if (text) data.filling = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 4);
    if (text) data.fuelName = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 5);
    if (text) data.idTso = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 6);
    if (text) data.lowerLevel = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 7);
    if (text) data.lowerVolume = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 8);
    if (text) data.namePmp = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 9);
    if (text) data.temperature = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 10);
    if (text) data.totalVolume = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 11);
    if (text) data.upperLevel = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 12);
    if (text) data.upperVolume = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 13);
    if (text) data.weight = reinterpret_cast<const char*>(text);
    
    text = sqlite3_column_text(stmt, 14);
    if (text) data.createdAt = reinterpret_cast<const char*>(text);
    
    return data;
}

MetrologicalRecordData MetrologicalRecordRepository::getById(int64_t id) {
    const char* sql = "SELECT * FROM metrological_records WHERE id = ?;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int64(stmt, 1, id);
    
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        db_->finalize(stmt);
        throw DatabaseException("Metrological record not found");
    }
    
    MetrologicalRecordData data = mapFromStatement(stmt);
    db_->finalize(stmt);
    
    return data;
}

std::vector<MetrologicalRecordData> MetrologicalRecordRepository::getAll() {
    const char* sql = "SELECT * FROM metrological_records ORDER BY created_at DESC;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    std::vector<MetrologicalRecordData> results;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return results;
}

std::vector<MetrologicalRecordData> MetrologicalRecordRepository::getByIdTso(const std::string& idTso) {
    const char* sql = "SELECT * FROM metrological_records WHERE id_tso = ? ORDER BY created_at DESC;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_text(stmt, 1, idTso.c_str(), -1, SQLITE_TRANSIENT);
    
    std::vector<MetrologicalRecordData> results;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return results;
}

std::vector<MetrologicalRecordData> MetrologicalRecordRepository::getByFuelName(const std::string& fuelName) {
    const char* sql = "SELECT * FROM metrological_records WHERE fuel_name = ? ORDER BY created_at DESC;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_text(stmt, 1, fuelName.c_str(), -1, SQLITE_TRANSIENT);
    
    std::vector<MetrologicalRecordData> results;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return results;
}

std::vector<MetrologicalRecordData> MetrologicalRecordRepository::getByNamePmp(const std::string& namePmp) {
    const char* sql = "SELECT * FROM metrological_records WHERE name_pmp = ? ORDER BY created_at DESC;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_text(stmt, 1, namePmp.c_str(), -1, SQLITE_TRANSIENT);
    
    std::vector<MetrologicalRecordData> results;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return results;
}

std::vector<MetrologicalRecordData> MetrologicalRecordRepository::getByDateRange(
    const std::string& startDate, const std::string& endDate) {
    const char* sql = "SELECT * FROM metrological_records WHERE date >= ? AND date <= ? ORDER BY date DESC;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_text(stmt, 1, startDate.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, endDate.c_str(), -1, SQLITE_TRANSIENT);
    
    std::vector<MetrologicalRecordData> results;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return results;
}

void MetrologicalRecordRepository::update(const MetrologicalRecordData& record) {
    const char* sql = R"(
        UPDATE metrological_records SET
            date = ?, density = ?, filling = ?, fuel_name = ?, id_tso = ?,
            lower_level = ?, lower_volume = ?, name_pmp = ?, temperature = ?,
            total_volume = ?, upper_level = ?, upper_volume = ?, weight = ?
        WHERE id = ?;
    )";
    
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_text(stmt, 1, record.date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, record.density.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, record.filling.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, record.fuelName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, record.idTso.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, record.lowerLevel.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, record.lowerVolume.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, record.namePmp.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, record.temperature.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 10, record.totalVolume.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, record.upperLevel.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 12, record.upperVolume.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 13, record.weight.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 14, record.id);
    
    int rc = sqlite3_step(stmt);
    db_->finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        LOG_SYSTEM_ERROR << "Failed to update metrological record";
        throw DatabaseException("Failed to update metrological record");
    }
    
    LOG_SYSTEM_INFO << "Metrological record updated: " << record.id;
}

void MetrologicalRecordRepository::deleteById(int64_t id) {
    const char* sql = "DELETE FROM metrological_records WHERE id = ?;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int64(stmt, 1, id);
    
    int rc = sqlite3_step(stmt);
    db_->finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        LOG_SYSTEM_ERROR << "Failed to delete metrological record";
        throw DatabaseException("Failed to delete metrological record");
    }
    
    LOG_SYSTEM_INFO << "Metrological record deleted: " << id;
}

int64_t MetrologicalRecordRepository::count() {
    const char* sql = "SELECT COUNT(*) FROM metrological_records;";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    int64_t count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int64(stmt, 0);
    }
    
    db_->finalize(stmt);
    return count;
}

} // namespace bridge
