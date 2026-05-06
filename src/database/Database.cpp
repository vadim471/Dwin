#include "bridge/database/Database.hpp"
#include <sstream>

namespace bridge {

Database::Database(const std::string& dbPath)
    : db_(nullptr), dbPath_(dbPath) {
    open();
}

Database::~Database() {
    close();
}

Database::Database(Database&& other) noexcept
    : db_(other.db_), dbPath_(std::move(other.dbPath_)) {
    other.db_ = nullptr;
}

Database& Database::operator=(Database&& other) noexcept {
    if (this != &other) {
        close();
        db_ = other.db_;
        dbPath_ = std::move(other.dbPath_);
        other.db_ = nullptr;
    }
    return *this;
}

void Database::open() {
    int rc = sqlite3_open(dbPath_.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::string error = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        db_ = nullptr;
        LOG_SYSTEM_ERROR << "Failed to open database: " << error;
        throw DatabaseException("Failed to open database: " + error);
    }
    
    // Включить поддержку внешних ключей
    execute("PRAGMA foreign_keys = ON;");
    
    LOG_SYSTEM_INFO << "Database opened successfully: " << dbPath_;
}

void Database::close() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
        LOG_SYSTEM_INFO << "Database closed: " << dbPath_;
    }
}

void Database::execute(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::string error = errMsg ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
        LOG_SYSTEM_ERROR << "SQL execution failed: " << error << " | SQL: " << sql;
        throw DatabaseException("SQL execution failed: " + error);
    }
}

sqlite3_stmt* Database::prepare(const std::string& sql) {
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::string error = sqlite3_errmsg(db_);
        LOG_SYSTEM_ERROR << "Failed to prepare statement: " << error << " | SQL: " << sql;
        throw DatabaseException("Failed to prepare statement: " + error);
    }
    
    return stmt;
}

void Database::finalize(sqlite3_stmt* stmt) {
    if (stmt) {
        sqlite3_finalize(stmt);
    }
}

void Database::beginTransaction() {
    execute("BEGIN TRANSACTION;");
}

void Database::commit() {
    execute("COMMIT;");
}

void Database::rollback() {
    execute("ROLLBACK;");
}

int64_t Database::getLastInsertId() const {
    return sqlite3_last_insert_rowid(db_);
}

bool Database::tableExists(const std::string& tableName) {
    std::string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name=?;";
    sqlite3_stmt* stmt = nullptr;
    
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, tableName.c_str(), -1, SQLITE_TRANSIENT);
    
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    
    return exists;
}

} // namespace bridge
