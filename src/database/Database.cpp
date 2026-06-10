#include "bridge/database/Database.hpp"
#include <sqlite3.h>

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
            close(); // Закрываем текущее соединение, если оно есть
            db_ = other.db_;
            dbPath_ = std::move(other.dbPath_);
            other.db_ = nullptr;
        }
        return *this;
    }

    void Database::open() {
        if (sqlite3_open(dbPath_.c_str(), &db_) != SQLITE_OK) {
            std::string errorMsg = "Failed to open database: ";
            if (db_) {
                errorMsg += sqlite3_errmsg(db_);
                sqlite3_close(db_);
                db_ = nullptr;
            }
            throw DatabaseException(errorMsg);
        }
    }

    void Database::close() {
        if (db_) {
            sqlite3_close(db_);
            db_ = nullptr;
        }
    }

    void Database::execute(const std::string& sql) {
        char* errMsg = nullptr;
        if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::string error = "SQL execute error: ";
            if (errMsg) {
                error += errMsg;
                sqlite3_free(errMsg); // Освобождаем память, выделенную SQLite
            }
            throw DatabaseException(error);
        }
    }

    sqlite3_stmt* Database::prepare(const std::string& sql) {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::string error = "SQL prepare error: ";
            if (db_) {
                error += sqlite3_errmsg(db_);
            }
            throw DatabaseException(error);
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
        if (!db_) return 0;
        return sqlite3_last_insert_rowid(db_);
    }

    bool Database::tableExists(const std::string& tableName) {
        std::string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name=?;";
        sqlite3_stmt* stmt = prepare(sql);

        if (sqlite3_bind_text(stmt, 1, tableName.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
            finalize(stmt);
            throw DatabaseException("Failed to bind table name in tableExists");
        }

        bool exists = false;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            exists = true;
        }

        finalize(stmt);
        return exists;
    }
}