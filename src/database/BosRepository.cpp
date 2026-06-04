#include "bridge/database/BosRepository.hpp"
#include <stdexcept>

namespace bridge {
    BosRepository::BosRepository(std::shared_ptr<Database> db) : db_(db) {}

    void BosRepository::createTable() {
        std::string sql = R"(
         CREATE TABLE IF NOT EXISTS bos (
             id INTEGER PRIMARY KEY AUTOINCREMENT,
             type INTEGER NOT NULL,
             data TEXT NOT NULL,
             created_at TEXT DEFAULT CURRENT_TIMESTAMP
         );
         CREATE INDEX IF NOT EXISTS idx_bos_type ON bos(type);
     )";
        db_->execute(sql);
        LOG_SYSTEM_INFO << "BosRepository: Table created/verified";
    }

    int64_t BosRepository::insert(int type, const std::string& payload) {
        std::string sql = "INSERT INTO bos (type, data) VALUES (?, ?)";
        sqlite3_stmt* stmt = db_->prepare(sql);
        sqlite3_bind_int(stmt, 1, type);
        sqlite3_bind_text(stmt, 2, payload.c_str(), -1, SQLITE_TRANSIENT);

        int rc = sqlite3_step(stmt);
        db_->finalize(stmt);

        if (rc != SQLITE_DONE) {
            throw DatabaseException("Failed to insert bos record");
        }

        return db_->getLastInsertId();
    }
}