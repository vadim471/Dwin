#include "bridge/database/LevelGaugeRepository.hpp"
#include <sstream>
#include <ctime>
#include <iomanip>

namespace bridge {

    LevelGaugeRepository::LevelGaugeRepository(std::shared_ptr<Database> db)
        : db_(db) {
    }

    auto safeGetTextLG = [](sqlite3_stmt* stmt, int col) -> std::string {
        const unsigned char* text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char*>(text) : "";
    };

    void LevelGaugeRepository::createTable() {
        const char* sql = R"(
            CREATE TABLE IF NOT EXISTS level_gauge_records (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                date TEXT,
                timestamp TEXT,
                gauge_id TEXT,
                gauge_name TEXT,
                fuel_name TEXT,
                upper_level TEXT,
                lower_level TEXT,
                upper_volume TEXT,
                lower_volume TEXT,
                total_volume TEXT,
                temperature TEXT,
                weight TEXT,
                density TEXT,
                filling TEXT,
                sent_to_server INTEGER DEFAULT 0,
                created_at TEXT
            );
        )";

        db_->execute(sql);

        db_->execute("CREATE INDEX IF NOT EXISTS idx_lg_sent ON level_gauge_records(sent_to_server);");
        db_->execute("CREATE INDEX IF NOT EXISTS idx_lg_gauge_id ON level_gauge_records(gauge_id);");
        db_->execute("CREATE INDEX IF NOT EXISTS idx_lg_timestamp ON level_gauge_records(timestamp);");

        LOG_SYSTEM_INFO << "LevelGaugeRepository: Table created/verified";
    }

    int64_t LevelGaugeRepository::insert(const LevelGaugeData& record) {
        const char* sql = R"(
            INSERT INTO level_gauge_records (
                date, timestamp, gauge_id, gauge_name, fuel_name,
                upper_level, lower_level, upper_volume, lower_volume, total_volume,
                temperature, weight, density, filling, sent_to_server, created_at
            ) VALUES (
                ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?
            );
        )";

        sqlite3_stmt* stmt = db_->prepare(sql);

        int col = 1;
        sqlite3_bind_text(stmt, col++, record.date.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, record.timestamp.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, record.gauge_id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, record.gauge_name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, record.fuel_name.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, col++, record.upper_level.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, record.lower_level.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, record.upper_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, record.lower_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, record.total_volume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, record.temperature.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, record.weight.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, record.density.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, col++, record.filling.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_int(stmt, col++, record.sent_to_server ? 1 : 0);
        sqlite3_bind_text(stmt, col++, record.created_at.c_str(), -1, SQLITE_TRANSIENT);

        int rc = sqlite3_step(stmt);
        db_->finalize(stmt);

        if (rc != SQLITE_DONE) {
            LOG_SYSTEM_ERROR << "Failed to insert level gauge record";
            throw DatabaseException("Failed to insert level gauge record");
        }

        int64_t id = db_->getLastInsertId();
        LOG_SYSTEM_INFO << "Level Gauge record inserted with ID: " << id;

        return id;
    }

    LevelGaugeData LevelGaugeRepository::getById(int64_t id) {
        const char* sql = "SELECT * FROM level_gauge_records WHERE id = ?;";
        sqlite3_stmt* stmt = db_->prepare(sql);

        sqlite3_bind_int64(stmt, 1, id);

        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW) {
            db_->finalize(stmt);
            throw DatabaseException("Level gauge record not found");
        }

        LevelGaugeData data = mapFromStatement(stmt);
        db_->finalize(stmt);

        return data;
    }

    std::vector<LevelGaugeData> LevelGaugeRepository::getAll() {
        const char* sql = "SELECT * FROM level_gauge_records ORDER BY timestamp DESC;";
        sqlite3_stmt* stmt = db_->prepare(sql);

        std::vector<LevelGaugeData> results;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            results.push_back(mapFromStatement(stmt));
        }

        db_->finalize(stmt);
        return results;
    }

    std::vector<LevelGaugeData> LevelGaugeRepository::getByGaugeId(const std::string& gauge_id) {
        const char* sql = "SELECT * FROM level_gauge_records WHERE gauge_id = ? ORDER BY timestamp DESC;";
        sqlite3_stmt* stmt = db_->prepare(sql);

        sqlite3_bind_text(stmt, 1, gauge_id.c_str(), -1, SQLITE_TRANSIENT);

        std::vector<LevelGaugeData> results;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            results.push_back(mapFromStatement(stmt));
        }

        db_->finalize(stmt);
        return results;
    }

    std::vector<LevelGaugeData> LevelGaugeRepository::getUnsent() {
        const char* sql = "SELECT * FROM level_gauge_records WHERE sent_to_server = 0 ORDER BY id ASC;";
        sqlite3_stmt* stmt = db_->prepare(sql);

        std::vector<LevelGaugeData> results;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            results.push_back(mapFromStatement(stmt));
        }

        db_->finalize(stmt);
        return results;
    }

    void LevelGaugeRepository::markAsSent(int64_t id) {
        const char* sql = "UPDATE level_gauge_records SET sent_to_server = 1 WHERE id = ?;";
        sqlite3_stmt* stmt = db_->prepare(sql);

        sqlite3_bind_int64(stmt, 1, id);

        int rc = sqlite3_step(stmt);
        db_->finalize(stmt);

        if (rc != SQLITE_DONE) {
            LOG_SYSTEM_ERROR << "Failed to mark level gauge record as sent";
            throw DatabaseException("Failed to mark level gauge record as sent");
        }
        LOG_SYSTEM_INFO << "Level gauge record marked as sent: " << id;
    }

    void LevelGaugeRepository::deleteById(int64_t id) {
        const char* sql = "DELETE FROM level_gauge_records WHERE id = ?;";
        sqlite3_stmt* stmt = db_->prepare(sql);

        sqlite3_bind_int64(stmt, 1, id);

        int rc = sqlite3_step(stmt);
        db_->finalize(stmt);

        if (rc != SQLITE_DONE) {
            LOG_SYSTEM_ERROR << "Failed to delete level gauge record";
            throw DatabaseException("Failed to delete level gauge record");
        }

        LOG_SYSTEM_INFO << "Level gauge record deleted: " << id;
    }

    int64_t LevelGaugeRepository::count() {
        const char* sql = "SELECT COUNT(*) FROM level_gauge_records;";
        sqlite3_stmt* stmt = db_->prepare(sql);

        int64_t count = 0;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int64(stmt, 0);
        }

        db_->finalize(stmt);
        return count;
    }

    LevelGaugeData LevelGaugeRepository::mapFromStatement(sqlite3_stmt* stmt) {
        LevelGaugeData record;

        record.id = sqlite3_column_int64(stmt, 0);
        record.date = safeGetTextLG(stmt, 1);
        record.timestamp = sqlite3_column_int64(stmt, 2);
        record.gauge_id = safeGetTextLG(stmt, 3);
        record.gauge_name = safeGetTextLG(stmt, 4);
        record.fuel_name = safeGetTextLG(stmt, 5);

        record.upper_level = safeGetTextLG(stmt, 6);
        record.lower_level = safeGetTextLG(stmt, 7);
        record.upper_volume = safeGetTextLG(stmt, 8);
        record.lower_volume = safeGetTextLG(stmt, 9);
        record.total_volume = safeGetTextLG(stmt, 10);
        record.temperature = safeGetTextLG(stmt, 11);
        record.weight = safeGetTextLG(stmt, 12);
        record.density = safeGetTextLG(stmt, 13);
        record.filling = safeGetTextLG(stmt, 14);

        record.sent_to_server = sqlite3_column_int(stmt, 15) != 0;
        record.created_at = safeGetTextLG(stmt, 16);

        return record;
    }
} // namespace bridge
