#ifndef DWIN_EXMPL_DATABASE_HPP
#define DWIN_EXMPL_DATABASE_HPP

#include <sqlite3.h>
#include <string>
#include <stdexcept>
#include "bridge/core/Logger.hpp"

namespace bridge {

class Database {
public:
    explicit Database(const std::string& dbPath);
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    Database(Database&& other) noexcept;
    Database& operator=(Database&& other) noexcept;

    void execute(const std::string& sql);

    sqlite3_stmt* prepare(const std::string& sql);

    void finalize(sqlite3_stmt* stmt);

    void beginTransaction();

    void commit();

    void rollback();

    int64_t getLastInsertId() const;

    bool tableExists(const std::string& tableName);

private:
    sqlite3* db_;
    std::string dbPath_;

    void open();
    void close();
};

class DatabaseException : public std::runtime_error {
public:
    explicit DatabaseException(const std::string& message)
        : std::runtime_error(message) {}
};

} // namespace bridge

#endif // DWIN_EXMPL_DATABASE_HPP
