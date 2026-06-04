#include "bridge/database/LevelGaugeRepository.hpp"
#include <sstream>
#include <ctime>
#include <iomanip>

namespace bridge {

LevelGaugeRepository::LevelGaugeRepository(std::shared_ptr<Database> db)
    : db_(db) {
}

void LevelGaugeRepository::createTable() {
    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS level_gauge_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            date TEXT NOT NULL,
            timestamp INTEGER NOT NULL,
            gauge_id TEXT NOT NULL,
            gauge_name TEXT,
            fuel_name TEXT,
            upper_level REAL NOT NULL,
            lower_level REAL NOT NULL,
            upper_volume REAL NOT NULL,
            lower_volume REAL NOT NULL,
            total_volume REAL NOT NULL,
            temperature REAL NOT NULL,
            weight REAL NOT NULL,
            density REAL NOT NULL,
            filling REAL NOT NULL,
            sent_to_server INTEGER DEFAULT 0,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        );
        
        CREATE INDEX IF NOT EXISTS idx_level_gauge_timestamp ON level_gauge_records(timestamp);
        CREATE INDEX IF NOT EXISTS idx_level_gauge_gauge_id ON level_gauge_records(gauge_id);
        CREATE INDEX IF NOT EXISTS idx_level_gauge_sent ON level_gauge_records(sent_to_server);
    )";
    
    db_->execute(sql);
    LOG_SYSTEM_INFO << "LevelGaugeRepository: Table created/verified";
}

int64_t LevelGaugeRepository::insert(const LevelGaugeRecord& record) {
    std::string sql = R"(
        INSERT INTO level_gauge_records (
            date, timestamp, gauge_id, gauge_name, fuel_name,
            upper_level, lower_level, upper_volume, lower_volume, total_volume,
            temperature, weight, density, filling, sent_to_server
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";
    
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_text(stmt, 1, record.date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, record.timestamp);
    sqlite3_bind_text(stmt, 3, record.gauge_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, record.gauge_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, record.fuel_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 6, record.upper_level);
    sqlite3_bind_double(stmt, 7, record.lower_level);
    sqlite3_bind_double(stmt, 8, record.upper_volume);
    sqlite3_bind_double(stmt, 9, record.lower_volume);
    sqlite3_bind_double(stmt, 10, record.total_volume);
    sqlite3_bind_double(stmt, 11, record.temperature);
    sqlite3_bind_double(stmt, 12, record.weight);
    sqlite3_bind_double(stmt, 13, record.density);
    sqlite3_bind_double(stmt, 14, record.filling);
    sqlite3_bind_int(stmt, 15, record.sent_to_server ? 1 : 0);
    
    int rc = sqlite3_step(stmt);
    db_->finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        throw DatabaseException("Failed to insert level gauge record");
    }
    
    return db_->getLastInsertId();
}

LevelGaugeRecord LevelGaugeRepository::getById(int64_t id) {
    std::string sql = "SELECT * FROM level_gauge_records WHERE id = ?";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int64(stmt, 1, id);
    
    LevelGaugeRecord record;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        record = mapFromStatement(stmt);
    }
    
    db_->finalize(stmt);
    return record;
}

std::vector<LevelGaugeRecord> LevelGaugeRepository::getAll() {
    std::string sql = "SELECT * FROM level_gauge_records ORDER BY timestamp DESC";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    std::vector<LevelGaugeRecord> records;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        records.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return records;
}

std::vector<LevelGaugeRecord> LevelGaugeRepository::getByGaugeId(const std::string& gauge_id) {
    std::string sql = "SELECT * FROM level_gauge_records WHERE gauge_id = ? ORDER BY timestamp DESC";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_text(stmt, 1, gauge_id.c_str(), -1, SQLITE_TRANSIENT);
    
    std::vector<LevelGaugeRecord> records;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        records.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return records;
}

std::vector<LevelGaugeRecord> LevelGaugeRepository::getUnsent() {
    std::string sql = "SELECT * FROM level_gauge_records WHERE sent_to_server = 0 ORDER BY timestamp ASC";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    std::vector<LevelGaugeRecord> records;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        records.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return records;
}

void LevelGaugeRepository::markAsSent(int64_t id) {
    std::string sql = "UPDATE level_gauge_records SET sent_to_server = 1 WHERE id = ?";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int64(stmt, 1, id);
    
    int rc = sqlite3_step(stmt);
    db_->finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        throw DatabaseException("Failed to mark level gauge record as sent");
    }
}

void LevelGaugeRepository::deleteById(int64_t id) {
    std::string sql = "DELETE FROM level_gauge_records WHERE id = ?";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int64(stmt, 1, id);
    
    int rc = sqlite3_step(stmt);
    db_->finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        throw DatabaseException("Failed to delete level gauge record");
    }
}

int64_t LevelGaugeRepository::count() {
    std::string sql = "SELECT COUNT(*) FROM level_gauge_records";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    int64_t count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int64(stmt, 0);
    }
    
    db_->finalize(stmt);
    return count;
}

LevelGaugeRecord LevelGaugeRepository::mapFromStatement(sqlite3_stmt* stmt) {
    LevelGaugeRecord record;
    
    record.id = sqlite3_column_int64(stmt, 0);
    record.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    record.timestamp = sqlite3_column_int64(stmt, 2);
    record.gauge_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    
    const char* gauge_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    record.gauge_name = gauge_name ? gauge_name : "";
    
    const char* fuel_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    record.fuel_name = fuel_name ? fuel_name : "";
    
    record.upper_level = sqlite3_column_double(stmt, 6);
    record.lower_level = sqlite3_column_double(stmt, 7);
    record.upper_volume = sqlite3_column_double(stmt, 8);
    record.lower_volume = sqlite3_column_double(stmt, 9);
    record.total_volume = sqlite3_column_double(stmt, 10);
    record.temperature = sqlite3_column_double(stmt, 11);
    record.weight = sqlite3_column_double(stmt, 12);
    record.density = sqlite3_column_double(stmt, 13);
    record.filling = sqlite3_column_double(stmt, 14);
    record.sent_to_server = sqlite3_column_int(stmt, 15) != 0;
    
    const char* created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 16));
    record.created_at = created_at ? created_at : "";
    
    return record;
}

} // namespace bridge
