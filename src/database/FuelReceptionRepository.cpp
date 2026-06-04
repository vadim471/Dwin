#include "bridge/database/FuelReceptionRepository.hpp"
#include <sstream>

namespace bridge {

FuelReceptionRepository::FuelReceptionRepository(std::shared_ptr<Database> db)
    : db_(db) {
}

void FuelReceptionRepository::createTable() {
    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS fuel_reception_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            date TEXT NOT NULL,
            timestamp INTEGER NOT NULL,
            gauge_id TEXT NOT NULL,
            gauge_name TEXT,
            fuel_name TEXT,
            document_number TEXT,
            before_upper_level REAL NOT NULL,
            before_lower_level REAL NOT NULL,
            before_upper_volume REAL NOT NULL,
            before_lower_volume REAL NOT NULL,
            before_total_volume REAL NOT NULL,
            before_temperature REAL NOT NULL,
            before_weight REAL NOT NULL,
            before_density REAL NOT NULL,
            before_filling REAL NOT NULL,
            after_upper_level REAL NOT NULL,
            after_lower_level REAL NOT NULL,
            after_upper_volume REAL NOT NULL,
            after_lower_volume REAL NOT NULL,
            after_total_volume REAL NOT NULL,
            after_temperature REAL NOT NULL,
            after_weight REAL NOT NULL,
            after_density REAL NOT NULL,
            after_filling REAL NOT NULL,
            received_liters REAL NOT NULL,
            time_beginning TEXT,
            time_ending TEXT,
            sent_to_server INTEGER DEFAULT 0,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        );
        
        CREATE INDEX IF NOT EXISTS idx_fuel_reception_timestamp ON fuel_reception_records(timestamp);
        CREATE INDEX IF NOT EXISTS idx_fuel_reception_gauge_id ON fuel_reception_records(gauge_id);
        CREATE INDEX IF NOT EXISTS idx_fuel_reception_sent ON fuel_reception_records(sent_to_server);
    )";
    
    db_->execute(sql);
    LOG_SYSTEM_INFO << "FuelReceptionRepository: Table created/verified";
}

int64_t FuelReceptionRepository::insert(const FuelReceptionRecord& record) {
    std::string sql = R"(
        INSERT INTO fuel_reception_records (
            date, timestamp, gauge_id, gauge_name, fuel_name, document_number,
            before_upper_level, before_lower_level, before_upper_volume, before_lower_volume, before_total_volume,
            before_temperature, before_weight, before_density, before_filling,
            after_upper_level, after_lower_level, after_upper_volume, after_lower_volume, after_total_volume,
            after_temperature, after_weight, after_density, after_filling,
            received_liters, time_beginning, time_ending, sent_to_server
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";
    
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_text(stmt, 1, record.date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, record.timestamp);
    sqlite3_bind_text(stmt, 3, record.gauge_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, record.gauge_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, record.fuel_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, record.document_number.c_str(), -1, SQLITE_TRANSIENT);
    
    sqlite3_bind_double(stmt, 7, record.before_upper_level);
    sqlite3_bind_double(stmt, 8, record.before_lower_level);
    sqlite3_bind_double(stmt, 9, record.before_upper_volume);
    sqlite3_bind_double(stmt, 10, record.before_lower_volume);
    sqlite3_bind_double(stmt, 11, record.before_total_volume);
    sqlite3_bind_double(stmt, 12, record.before_temperature);
    sqlite3_bind_double(stmt, 13, record.before_weight);
    sqlite3_bind_double(stmt, 14, record.before_density);
    sqlite3_bind_double(stmt, 15, record.before_filling);
    
    sqlite3_bind_double(stmt, 16, record.after_upper_level);
    sqlite3_bind_double(stmt, 17, record.after_lower_level);
    sqlite3_bind_double(stmt, 18, record.after_upper_volume);
    sqlite3_bind_double(stmt, 19, record.after_lower_volume);
    sqlite3_bind_double(stmt, 20, record.after_total_volume);
    sqlite3_bind_double(stmt, 21, record.after_temperature);
    sqlite3_bind_double(stmt, 22, record.after_weight);
    sqlite3_bind_double(stmt, 23, record.after_density);
    sqlite3_bind_double(stmt, 24, record.after_filling);
    
    sqlite3_bind_double(stmt, 25, record.received_liters);
    sqlite3_bind_text(stmt, 26, record.time_beginning.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 27, record.time_ending.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 28, record.sent_to_server ? 1 : 0);
    
    int rc = sqlite3_step(stmt);
    db_->finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        throw DatabaseException("Failed to insert fuel reception record");
    }
    
    return db_->getLastInsertId();
}

FuelReceptionRecord FuelReceptionRepository::getById(int64_t id) {
    std::string sql = "SELECT * FROM fuel_reception_records WHERE id = ?";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int64(stmt, 1, id);
    
    FuelReceptionRecord record;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        record = mapFromStatement(stmt);
    }
    
    db_->finalize(stmt);
    return record;
}

std::vector<FuelReceptionRecord> FuelReceptionRepository::getAll() {
    std::string sql = "SELECT * FROM fuel_reception_records ORDER BY timestamp DESC";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    std::vector<FuelReceptionRecord> records;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        records.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return records;
}

std::vector<FuelReceptionRecord> FuelReceptionRepository::getByGaugeId(const std::string& gauge_id) {
    std::string sql = "SELECT * FROM fuel_reception_records WHERE gauge_id = ? ORDER BY timestamp DESC";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_text(stmt, 1, gauge_id.c_str(), -1, SQLITE_TRANSIENT);
    
    std::vector<FuelReceptionRecord> records;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        records.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return records;
}

std::vector<FuelReceptionRecord> FuelReceptionRepository::getUnsent() {
    std::string sql = "SELECT * FROM fuel_reception_records WHERE sent_to_server = 0 ORDER BY timestamp ASC";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    std::vector<FuelReceptionRecord> records;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        records.push_back(mapFromStatement(stmt));
    }
    
    db_->finalize(stmt);
    return records;
}

void FuelReceptionRepository::markAsSent(int64_t id) {
    std::string sql = "UPDATE fuel_reception_records SET sent_to_server = 1 WHERE id = ?";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int64(stmt, 1, id);
    
    int rc = sqlite3_step(stmt);
    db_->finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        throw DatabaseException("Failed to mark fuel reception record as sent");
    }
}

void FuelReceptionRepository::deleteById(int64_t id) {
    std::string sql = "DELETE FROM fuel_reception_records WHERE id = ?";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    sqlite3_bind_int64(stmt, 1, id);
    
    int rc = sqlite3_step(stmt);
    db_->finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        throw DatabaseException("Failed to delete fuel reception record");
    }
}

int64_t FuelReceptionRepository::count() {
    std::string sql = "SELECT COUNT(*) FROM fuel_reception_records";
    sqlite3_stmt* stmt = db_->prepare(sql);
    
    int64_t count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int64(stmt, 0);
    }
    
    db_->finalize(stmt);
    return count;
}

FuelReceptionRecord FuelReceptionRepository::mapFromStatement(sqlite3_stmt* stmt) {
    FuelReceptionRecord record;
    
    record.id = sqlite3_column_int64(stmt, 0);
    record.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    record.timestamp = sqlite3_column_int64(stmt, 2);
    record.gauge_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    
    const char* gauge_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    record.gauge_name = gauge_name ? gauge_name : "";
    
    const char* fuel_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    record.fuel_name = fuel_name ? fuel_name : "";
    
    const char* document_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    record.document_number = document_number ? document_number : "";
    
    record.before_upper_level = sqlite3_column_double(stmt, 7);
    record.before_lower_level = sqlite3_column_double(stmt, 8);
    record.before_upper_volume = sqlite3_column_double(stmt, 9);
    record.before_lower_volume = sqlite3_column_double(stmt, 10);
    record.before_total_volume = sqlite3_column_double(stmt, 11);
    record.before_temperature = sqlite3_column_double(stmt, 12);
    record.before_weight = sqlite3_column_double(stmt, 13);
    record.before_density = sqlite3_column_double(stmt, 14);
    record.before_filling = sqlite3_column_double(stmt, 15);
    
    record.after_upper_level = sqlite3_column_double(stmt, 16);
    record.after_lower_level = sqlite3_column_double(stmt, 17);
    record.after_upper_volume = sqlite3_column_double(stmt, 18);
    record.after_lower_volume = sqlite3_column_double(stmt, 19);
    record.after_total_volume = sqlite3_column_double(stmt, 20);
    record.after_temperature = sqlite3_column_double(stmt, 21);
    record.after_weight = sqlite3_column_double(stmt, 22);
    record.after_density = sqlite3_column_double(stmt, 23);
    record.after_filling = sqlite3_column_double(stmt, 24);
    
    record.received_liters = sqlite3_column_double(stmt, 25);
    
    const char* time_beginning = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 26));
    record.time_beginning = time_beginning ? time_beginning : "";
    
    const char* time_ending = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 27));
    record.time_ending = time_ending ? time_ending : "";
    
    record.sent_to_server = sqlite3_column_int(stmt, 28) != 0;
    
    const char* created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 29));
    record.created_at = created_at ? created_at : "";
    
    return record;
}

} // namespace bridge
