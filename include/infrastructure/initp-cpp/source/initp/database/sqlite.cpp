#include <initp/database/sqlite.hpp>
#include <initp/system/debug.hpp>

#ifndef SYSTEM_DISABLE_SQL_LONG_REQUESTS
#include <initp/system/time.hpp>
#endif // SYSTEM_DISABLE_SQL_LONG_REQUESTS

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>

#include <boost/make_unique.hpp>
#include <sstream>

namespace initp {
namespace database {

    // Statement class

sqlite::statement::statement(SQLite::Database& aDatabase, const std::string& aQuery):
    sql_(boost::make_unique<SQLite::Statement>(aDatabase, aQuery))
{}

sqlite::statement::statement(SQLite::Database& aDatabase, const char* aQuery):
    sql_(boost::make_unique<SQLite::Statement>(aDatabase, aQuery))
{}

sqlite::statement::~statement(void) {}

bool sqlite::statement::executeStep(void) {
    try {
        return this->sql_->executeStep();
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::statement::executeStep", "Catch error %s", e.what());
        return false;
    }
}

std::unique_ptr<SQLite::Column> sqlite::statement::getColumn(const int aIndex) {
    try {
        return boost::make_unique<SQLite::Column>(this->sql_->getColumn(aIndex));
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::statement::getColumn", "Catch error %s", e.what());
        return nullptr;
    }
}

std::unique_ptr<SQLite::Column> sqlite::statement::getColumn(const char* apName) {
    try {
        return boost::make_unique<SQLite::Column>(this->sql_->getColumn(apName));
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::statement::getColumn", "Catch error %s", e.what());
        return nullptr;
    }
}

bool sqlite::statement::isColumnNull(const int aIndex) const {
    try {
        return this->sql_->isColumnNull(aIndex);
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::statement::isColumnNull", "Catch error %s", e.what());
        return true;
    }
}

bool sqlite::statement::isColumnNull(const char* apName) const {
    try {
        return this->sql_->isColumnNull(apName);
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::statement::isColumnNull", "Catch error %s", e.what());
        return true;
    }
}

    // Construction

sqlite::sqlite(const std::string& path):
    path_(path) {}

sqlite::~sqlite(void) {
    this->instance_.reset();
}

    // Public methods

system::error_code sqlite::open(void) {
    if (this->is_open()) {
        return system::err::success;
    }
    this->table_ = std::string();
    try {
        this->instance_ = boost::make_unique<SQLite::Database>(this->path_, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    } catch (std::exception&) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::open", "Unable to open database file %s", this->path_.c_str());
        return system::err::connection_failed;
    }
    return system::err::success;
}

system::error_code sqlite::close(void) {
    if (!this->is_open()) {
        return system::err::success;
    }
    this->table_ = std::string();
    try {
        this->instance_.reset();
    } catch (std::exception&) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::open", "Unable to close database file %s", this->path_.c_str());
        return system::err::generic_error;
    }
    return system::err::success;
}

system::error_code sqlite::set_table(const std::string& table) {

    if (!this->is_open()) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "initp::database::sqlite::set_table", "Database not connected");
        return system::err::invalid_state;
    }

    if (this->is_table_set(table)) {
        return system::err::success;
    }

    try {
        if (!this->instance_->tableExists(table)) {
            sys_debug_print(SYSTEM_LEVEL_WARNING, "initp::database::sqlite::set_table", "Table '%s' not exists", table.c_str());
            return system::err::generic_error;
        }
        this->table_ = table;
    } catch (std::exception&) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::set_table", "Unable to get or create table");
        return system::err::generic_error;
    }

    return system::err::success;
}

system::error_code sqlite::set_or_create_table(const std::string& table, const std::vector<std::string>& queries) {

    if (!this->is_open()) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "initp::database::sqlite::set_or_create_table", "Database not connected");
        return system::err::invalid_state;
    }

    if (this->is_table_set(table)) {
        return system::err::success;
    }

    try {
        if (!this->instance_->tableExists(table)) {
            for (const std::string& query : queries) {
                this->instance_->exec(query);
            }
        }
        this->table_ = table;
    } catch (std::exception&) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::set_or_create_table", "Unable to get or create table");
        return system::err::generic_error;
    }

    return system::err::success;
}

bool sqlite::is_open(void) const {
    return (!this->instance_) ? false : true;
}

bool sqlite::is_exist(const std::string& table) const {
    if (!this->is_open()) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "initp::database::sqlite::is_exist", "Database not connected");
        return false;
    }
    try {
        if (!this->instance_->tableExists(table)) {
            return false;
        } else return true;
    } catch (std::exception&) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::is_exist", "Unable to get or create table");
        return false;
    }
}

bool sqlite::is_table_set(const std::string& table) const {
    return this->table_.compare(table) == 0;
}

    // Query methods

system::error_code sqlite::execute(const std::string& query) {

    if (!this->is_open()) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "initp::database::sqlite::execute", "Database not connected");
        return system::err::invalid_state;
    }

    #ifndef SYSTEM_DISABLE_SQL_LONG_REQUESTS
    system::time_t time = system::time::milliseconds();
    #endif // SYSTEM_DISABLE_SQL_LONG_REQUESTS

    try {
        this->instance_->exec(query);
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::execute", "Unable to execute SQL query; %s", e.what());
        sys_debug_print(SYSTEM_LEVEL_DEBUG, "initp::database::sqlite::execute", "Query: %s", query.c_str());
        return system::err::generic_error;
    }

    #ifndef SYSTEM_DISABLE_SQL_LONG_REQUESTS
    time = system::time::milliseconds() - time;
    if (time > 300) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "initp::database::sqlite::execute", "SQL query execution took %d ms", (int)time);
        sys_debug_print(SYSTEM_LEVEL_DEBUG, "initp::database::sqlite::execute", "Query: %s", query.c_str());
    }
    #endif // SYSTEM_DISABLE_SQL_LONG_REQUESTS

    return system::err::success;
}

system::error_code sqlite::upsert(const std::string& key, const std::string& value, const std::string& insert_query, const std::string& update_query) {

    if (!this->is_open()) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "initp::database::sqlite::upsert", "Database not connected");
        return system::err::invalid_state;
    }

    bool exist = false;
    std::stringstream s;
    s << "SELECT " << key << " FROM " << this->table_ << " WHERE " << key << " = '" << value << "'";
    std::string query = s.str();

    try {
        SQLite::Statement sql(*this->instance_.get(), query);
        exist = sql.executeStep();
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::upsert", "Unable to execute SQL query; %s", e.what());
        sys_debug_print(SYSTEM_LEVEL_DEBUG, "initp::database::sqlite::upsert", "Query: %s", query.c_str());
        return system::err::generic_error;
    }

    #ifndef SYSTEM_DISABLE_SQL_LONG_REQUESTS
    system::time_t time = system::time::milliseconds();
    #endif // SYSTEM_DISABLE_SQL_LONG_REQUESTS

    try {
        this->instance_->exec(exist ? update_query : insert_query);
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::upsert", "Unable to execute SQL query; %s", e.what());
        sys_debug_print(SYSTEM_LEVEL_DEBUG, "initp::database::sqlite::upsert", "Query: %s", query.c_str());
        return system::err::generic_error;
    }

    #ifndef SYSTEM_DISABLE_SQL_LONG_REQUESTS
    time = system::time::milliseconds() - time;
    if (time > 300) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "initp::database::sqlite::upsert", "SQL query execution took %d ms", (int)time);
        sys_debug_print(SYSTEM_LEVEL_DEBUG, "initp::database::sqlite::upsert", "Query: %s", query.c_str());
    }
    #endif // SYSTEM_DISABLE_SQL_LONG_REQUESTS

    return system::err::success;
}

sqlite::statement::uptr sqlite::select(const std::string& query) {

    if (!this->is_open()) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "initp::database::sqlite::select", "Database not connected");
        return nullptr;
    }

    try {
        return boost::make_unique<statement>(*this->instance_.get(), query);
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::sqlite::select", "Catch error %s", e.what());
        return nullptr;
    }
}

}}
