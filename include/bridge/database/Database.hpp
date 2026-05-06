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

    // Разрешить перемещение
    Database(Database&& other) noexcept;
    Database& operator=(Database&& other) noexcept;

    // Выполнить SQL запрос без возврата данных
    void execute(const std::string& sql);

    // Подготовить statement
    sqlite3_stmt* prepare(const std::string& sql);

    // Завершить statement
    void finalize(sqlite3_stmt* stmt);

    // Начать транзакцию
    void beginTransaction();

    // Зафиксировать транзакцию
    void commit();

    // Откатить транзакцию
    void rollback();

    // Получить последний ID вставленной записи
    int64_t getLastInsertId() const;

    // Проверить существование таблицы
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
