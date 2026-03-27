#ifndef INITPLUS_DATABASE_CACHE_DETAIL_HPP_INCLUDED
#define INITPLUS_DATABASE_CACHE_DETAIL_HPP_INCLUDED

#include "cache.hpp"
#include "sqlite.hpp"
#include "sqlite_detail.hpp"
#include "../system/time.hpp"
#include "../system/debug.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <tuple>
#include <functional>
#include <type_traits>

namespace initp {
namespace database {
namespace cache {

class column {
public:
    enum value_type { integer, floating, decimal, boolean, string };
public:
    template<typename T>
    column(T value, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr):
        type_(integer),
        unsigned_(std::is_unsigned<T>::value) {
        data_.integer = int64_t(value);
    }
    template<typename T>
    column(T value, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr):
        type_(floating) {
        data_.floating = double(value);
    }
    template<typename T>
    column(T value, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr):
        type_(boolean),
        expand_(value ? 1 : 0)
    {}
    #ifdef INITPLUS_TOOLS_DECIMAL_HPP_INCLUDED
    template<typename T, bool Fixed, uint8_t Round>
    column(const tools::decimal<T, Fixed, Round>& value):
        type_(decimal),
        unsigned_(std::is_unsigned<T>::value) {
        data_.integer = int64_t(value.mantissa());
        expand_ = value.exponent();
    }
    #endif // INITPLUS_TOOLS_DECIMAL_HPP_INCLUDED
    column(const std::string& value):
        type_(string),
        string_(value)
    {}
    column(const char* value):
        type_(string),
        string_(value)
    {}
    virtual ~column(void) = default;
public:
    value_type type(void) const {
        return type_;
    }
    bool is_unsigned(void) const {
        return unsigned_;
    }
    std::string create_query(const std::string& name) const {
        std::stringstream query;
        query << name << " ";
        switch (this->type_) {
            case integer:
                query << "INTEGER NOT NULL";
                break;
            case floating:
                query << "REAL NOT NULL";
                break;
            case decimal:
                query << "INTEGER NOT NULL";
                query << ", " << name;
                query << "_exp TINYINT(3) NOT NULL";
                break;
            case boolean:
                query << "TINYINT(3) NOT NULL";
                break;
            case string:
                query << "VARCHAR(50) NOT NULL";
                break;
            default:
                query << "INTEGER NOT NULL";
                break;
        }
        return query.str();
    }
    std::string insert_name(const std::string& name) const {
        switch (this->type_) {
            case decimal:
                {
                    std::stringstream query;
                    query << name << ", " << name << "_exp";
                    return query.str();
                }
            default:
                return name;
        }
    }
    std::string insert_value(void) const {
        std::stringstream query;
        switch (this->type_) {
            case integer:
                if (this->unsigned_) {
                    query << (uint64_t)this->data_.integer;
                } else {
                    query << this->data_.integer;
                }
                break;
            case floating:
                query << this->data_.floating;
                break;
            case decimal:
                if (this->unsigned_) {
                    query << (uint64_t)this->data_.integer;
                } else {
                    query << this->data_.integer;
                }
                query << ", " << (int)this->expand_;
                break;
            case boolean:
                query << (int)((this->expand_ > 0) ? 1 : 0);
                break;
            case string:
                query << "'" << this->string_ << "'";
                break;
            default:
                query << "''";
                break;
        }
        return query.str();
    }
    std::string update_query(const std::string& name) const {
        std::stringstream query;
        query << name << " = ";
        switch (this->type_) {
            case integer:
                if (this->unsigned_) {
                    query << (uint64_t)this->data_.integer;
                } else {
                    query << this->data_.integer;
                }
                break;
            case floating:
                query << this->data_.floating;
                break;
            case decimal:
                if (this->unsigned_) {
                    query << (uint64_t)this->data_.integer;
                } else {
                    query << this->data_.integer;
                }
                query << ", " << name << "_exp = " << (int)this->expand_;
                break;
            case boolean:
                query << (int)((this->expand_ > 0) ? 1 : 0);
                break;
            case string:
                query << "'" << this->string_ << "'";
                break;
            default:
                query << "''";
                break;
        }
        return query.str();
    }
    std::string increment_query(const std::string& name) const {
        std::stringstream query;
        query << name << " = ";
        switch (this->type_) {
            case integer:
                query << name << " + 1";
                break;
            default:
                query << name;
                break;
        }
        return query.str();
    }
    template<typename T>
    static system::error_code select(const std::string& name, database::sqlite::statement& row, T& value, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr) {
        try {
            if (std::unique_ptr<SQLite::Column> column = std::move(row.getColumn(name.c_str()))) {
                value = (T)column->getInt64();
                return system::err::success;
            }
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::cache::column::select", "Failed to get column %s", name.c_str());
            return system::err::null_pointer;
        } catch (std::exception& e) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::cache::column::select", "%s", e.what());
            return system::err::internal_error;
        }
    }
    template<typename T>
    static system::error_code select(const std::string& name, database::sqlite::statement& row, T& value, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr) {
        try {
            if (std::unique_ptr<SQLite::Column> column = std::move(row.getColumn(name.c_str()))) {
                value = (T)column->getDouble();
                return system::err::success;
            }
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::cache::column::select", "Failed to get column %s", name.c_str());
            return system::err::null_pointer;
        } catch (std::exception& e) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::cache::column::select", "%s", e.what());
            return system::err::internal_error;
        }
    }
    template<typename T>
    static system::error_code select(const std::string& name, database::sqlite::statement& row, T& value, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr) {
        try {
            if (std::unique_ptr<SQLite::Column> column = std::move(row.getColumn(name.c_str()))) {
                value = column->getInt() != 0;
                return system::err::success;
            }
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::cache::column::select", "Failed to get column %s", name.c_str());
            return system::err::null_pointer;
        } catch (std::exception& e) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::cache::column::select", "%s", e.what());
            return system::err::internal_error;
        }
    }
    #ifdef INITPLUS_TOOLS_DECIMAL_HPP_INCLUDED
    template<typename T, bool Fixed, uint8_t Round>
    static system::error_code select(const std::string& name, database::sqlite::statement& row, tools::decimal<T, Fixed, Round>& value) {
        try {
            T mantissa = 0;
            if (std::unique_ptr<SQLite::Column> column = std::move(row.getColumn(name.c_str()))) {
                mantissa = (T)column->getInt64();
            } else {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::cache::column::select", "Failed to get column %s", name.c_str());
                return system::err::null_pointer;
            }
            std::string exp_name = name + std::string("_exp");
            uint8_t exponent = 0;
            if (std::unique_ptr<SQLite::Column> column = std::move(row.getColumn(exp_name.c_str()))) {
                exponent = (uint8_t)column->getInt();
            } else {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::cache::column::select", "Failed to get column %s", exp_name.c_str());
                return system::err::null_pointer;
            }
            value.set(mantissa, exponent);
            return system::err::success;
        } catch (std::exception& e) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::cache::column::select", "%s", e.what());
            return system::err::internal_error;
        }
    }
    #endif // INITPLUS_TOOLS_DECIMAL_HPP_INCLUDED
    static system::error_code select(const std::string& name, database::sqlite::statement& row, std::string& value) {
        try {
            if (std::unique_ptr<SQLite::Column> column = std::move(row.getColumn(name.c_str()))) {
                value = column->getString();
                return system::err::success;
            }
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::cache::column::select", "Failed to get column %s", name.c_str());
            return system::err::null_pointer;
        } catch (std::exception& e) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::database::cache::column::select", "%s", e.what());
            return system::err::internal_error;
        }
    }
private:
    value_type type_;
    union {
        int64_t integer;
        double floating;
    } data_;
    bool unsigned_;
    uint8_t expand_;
    std::string string_;
};

template<std::size_t N, typename... Args>
struct detail {
static system::error_code select(const std::function<std::string(uint8_t)>& get_name, database::sqlite::statement& row, std::tuple<Args&...> args) {
    system::error_code result = column(std::get<N - 1>(args)).select(get_name(N + 1), row, std::get<N - 1>(args));
    if (result) return result;
    return detail<N - 1, Args...>::select(get_name, row, args);
}
};

template<typename... Args>
struct detail<1, Args...> {
static system::error_code select(const std::function<std::string(uint8_t)>& get_name, database::sqlite::statement& row, std::tuple<Args&...> args) {
    return column(std::get<0>(args)).select(get_name(2), row, std::get<0>(args));
}
};

template<typename IdType, typename... Args>
class table:
    public virtual operation {

private:
    typedef table<IdType, Args...> self_type;

public:
    typedef std::shared_ptr<self_type> ptr;
    typedef std::unique_ptr<self_type> uptr;

public: // Construction
    table(database::sqlite& driver, const std::string& name, bool use_time, uint64_t limit):
        operation(),
        driver_(driver),
        name_(name),
        use_time_(use_time),
        limit_(limit)
    {}
    table(const self_type&) = delete;
    table(const self_type&& object):
        operation(),
        driver_(object.driver_),
        name_(std::move(object.name_)),
        args_(std::move(object.args_)),
        use_time_(object.use_time_),
        limit_(object.limit_)
    {}
    virtual ~table(void) = default;

public: // Public methods

    void set_names(const std::vector<std::string>& args) {
        this->args_.clear();
        this->args_.insert(this->args_.begin(), args.cbegin(), args.cend());
    }

    virtual bool empty(void) const {

        if (!this->driver_.is_open()) {
            if (this->driver_.open()) {
                return true;
            }
        }
        if (!this->driver_.is_table_set(this->name_)) {
            if (!this->driver_.is_exist(this->name_)) {
                if (this->driver_.set_or_create_table(this->name_, this->create_queries())) {
                    return true;
                }
            } else {
                if (this->driver_.set_table(this->name_)) {
                    return true;
                }
            }
        }

        database::sqlite::statement::uptr result = this->driver_.select(this->select_by_expiry_query());
        if (result && result->executeStep()) {
            return false;
        } else return true;
    }

    virtual system::error_code execute(void) {
        return system::err::success;
    }

    system::error_code select(const IdType& id, Args&... args) {

        if (!this->driver_.is_open()) {
            if (system::error_code result = this->driver_.open()) {
                return result;
            }
        }
        if (!this->driver_.is_table_set(this->name_)) {
            if (!this->driver_.is_exist(this->name_)) {
                if (system::error_code result = this->driver_.set_or_create_table(this->name_, this->create_queries())) {
                    return result;
                }
            } else {
                if (system::error_code result = this->driver_.set_table(this->name_)) {
                    return result;
                }
            }
        }

        database::sqlite::statement::uptr result = this->driver_.select(this->select_query(id));
        if (result && result->executeStep()) {
            system::error_code ec = detail<sizeof...(Args), Args...>::select(
                std::bind(&self_type::column_name, this, std::placeholders::_1),
                *result.get(),
                std::forward_as_tuple(args...)
            );
            return ec;
        }
        return system::err::null_pointer;
    }

    system::error_code select_by_expiry(IdType& id, Args&... args) {

        if (!this->driver_.is_open()) {
            if (system::error_code result = this->driver_.open()) {
                return result;
            }
        }
        if (!this->driver_.is_table_set(this->name_)) {
            if (!this->driver_.is_exist(this->name_)) {
                if (system::error_code result = this->driver_.set_or_create_table(this->name_, this->create_queries())) {
                    return result;
                }
            } else {
                if (system::error_code result = this->driver_.set_table(this->name_)) {
                    return result;
                }
            }
        }

        database::sqlite::statement::uptr result = this->driver_.select(this->select_by_expiry_query());
        if (result && result->executeStep()) {
            system::error_code ec = column(id).select(this->column_name(0), *result.get(), id);
            if (!ec) ec = detail<sizeof...(Args), Args...>::select(
                std::bind(&self_type::column_name, this, std::placeholders::_1),
                *result.get(),
                std::forward_as_tuple(args...)
            );
            return ec;
        }
        return system::err::null_pointer;
    }

    system::error_code success(const IdType& id) {
        return this->remove(id);
    }

    system::error_code failed(const IdType& id) {

        if (!this->driver_.is_open()) {
            if (system::error_code result = this->driver_.open()) {
                return result;
            }
        }
        if (!this->driver_.is_table_set(this->name_)) {
            if (!this->driver_.is_exist(this->name_)) {
                if (system::error_code result = this->driver_.set_or_create_table(this->name_, this->create_queries())) {
                    return result;
                }
            } else {
                if (system::error_code result = this->driver_.set_table(this->name_)) {
                    return result;
                }
            }
        }

        database::sqlite::statement::uptr result = this->driver_.select(this->select_query(id));
        if (result && result->executeStep()) {
            uint64_t expiry(0);
            system::error_code ec = column::select(this->column_name(1), *result.get(), expiry);
            if (ec) return ec;
            if (this->use_time_) {
                uint64_t time = system::time::milliseconds() / 1000L;
                if (expiry + this->limit_ <= time) {
                    return this->remove(id);
                }
            } else if (this->limit_ > 0) {
                if (expiry >= this->limit_) {
                    return this->remove(id);
                } else {
                    return this->driver_.execute(this->increment_query(id));
                }
            }
        }
        return system::err::success;
    }

    system::error_code insert(const IdType& id, const Args&... args) {

        if (!this->driver_.is_open()) {
            if (system::error_code result = this->driver_.open()) {
                return result;
            }
        }

        uint64_t expiry = use_time_ ? (system::time::milliseconds() / 1000L) : 0;
        std::vector<column> list = { id, expiry, args... };

        if (!this->driver_.is_table_set(this->name_)) {
            if (!this->driver_.is_exist(this->name_)) {
                if (system::error_code result = this->driver_.set_or_create_table(this->name_, this->create_queries(list))) {
                    return result;
                }
            } else {
                if (system::error_code result = this->driver_.set_table(this->name_)) {
                    return result;
                }
            }
        }

        return this->driver_.execute(this->insert_query(list));
    }

    system::error_code insert_or_update(const IdType& id, const Args&... args) {

        if (!this->driver_.is_open()) {
            if (system::error_code result = this->driver_.open()) {
                return result;
            }
        }

        uint64_t expiry = use_time_ ? (system::time::milliseconds() / 1000L) : 0;
        std::vector<column> list = { id, expiry, args... };

        if (!this->driver_.is_table_set(this->name_)) {
            if (!this->driver_.is_exist(this->name_)) {
                if (system::error_code result = this->driver_.set_or_create_table(this->name_, this->create_queries(list))) {
                    return result;
                }
            } else {
                if (system::error_code result = this->driver_.set_table(this->name_)) {
                    return result;
                }
            }
        }

        database::sqlite::statement::uptr result = this->driver_.select(this->select_query(id));
        if (result && result->executeStep()) {
            return this->driver_.execute(this->update_query(list));
        } else return this->driver_.execute(this->insert_query(list));
    }

    system::error_code remove(const IdType& id) {

        if (!this->driver_.is_open()) {
            if (system::error_code result = this->driver_.open()) {
                return result;
            }
        }

        if (!this->driver_.is_table_set(this->name_)) {
            if (!this->driver_.is_exist(this->name_)) {
                if (system::error_code result = this->driver_.set_or_create_table(this->name_, this->create_queries())) {
                    return result;
                }
            } else {
                if (system::error_code result = this->driver_.set_table(this->name_)) {
                    return result;
                }
            }
        }

        return this->driver_.execute(this->delete_query(id));
    }

private: // Queries
    std::vector<std::string> create_queries(const std::vector<column>& list = { IdType(), uint64_t(), Args()... }) const {

        std::vector<std::string> queries;
        std::stringstream query;

        query << "DROP INDEX IF EXISTS " << this->name_ << "_idx1;";
        queries.push_back(query.str());

        query.str("");
        query << "CREATE TABLE " << this->name_ << "(";
        for (size_t i = 0, size = list.size(); i < size; ++i) {
            if (i > 0) query << ", ";
            query << list[i].create_query(this->column_name(i));
            if (i == 0) query << " PRIMARY KEY";
        }
        query << ");";
        queries.push_back(query.str());

        query.str("");
        query << "CREATE INDEX " << this->name_ << "_idx1 ON " << this->name_ << " (" << this->column_name(1) << ");";
        queries.push_back(query.str());

        return queries;
    }
    std::string insert_query(const std::vector<column>& list) const {

        std::stringstream query;

        query << "INSERT INTO " << this->name_ << "(";
        for (size_t i = 0, size = list.size(); i < size; ++i) {
            if (i > 0) query << ", ";
            query << list[i].insert_name(this->column_name(i));
        }

        query << ") VALUES (";
        for (size_t i = 0, size = list.size(); i < size; ++i) {
            if (i > 0) query << ", ";
            query << list[i].insert_value();
        }

        query << ");";
        return query.str();
    }
    std::string update_query(const std::vector<column>& list) const {

        std::stringstream query;

        query << "UPDATE " << this->name_ << " SET ";
        for (size_t i = 1, size = list.size(); i < size; ++i) {
            if (i > 1) query << ", ";
            query << list[i].update_query(this->column_name(i));
        }

        query << " WHERE " << this->column_name(0) << " = " << list[0].insert_value() << ";";
        return query.str();
    }
    std::string delete_query(const IdType& id) const {
        std::stringstream query;
        query << "DELETE FROM " << this->name_ << " WHERE " << this->column_name(0) << " = " << column(id).insert_value() << ";";
        return query.str();
    }
    std::string delete_query(const std::vector<column>& list) const {
        std::stringstream query;
        query << "DELETE FROM " << this->name_ << " WHERE " << this->column_name(0) << " = " << list[0].insert_value() << ";";
        return query.str();
    }
    std::string select_query(const IdType& id) const {
        std::stringstream query;
        query << "SELECT * FROM " << this->name_ << " WHERE " << this->column_name(0) << " = " << column(id).insert_value() << ";";
        return query.str();
    }
    std::string select_by_expiry_query(void) const {
        std::stringstream query;
        query << "SELECT * FROM " << this->name_ << " ORDER BY " << this->column_name(1) << " ASC LIMIT 1;";
        return query.str();
    }
    std::string increment_query(const IdType& id) const {
        std::stringstream query;
        query << "UPDATE " << this->name_ << " SET ";
        query << column(uint64_t()).increment_query(this->column_name(1));
        query << " WHERE " << this->column_name(0) << " = " << column(id).insert_value() << ";";
        return query.str();
    }
    std::string increment_query(const std::vector<column>& list) const {
        std::stringstream query;
        query << "UPDATE " << this->name_ << " SET ";
        query << list[1].increment_query(this->column_name(1));
        query << " WHERE " << this->column_name(0) << " = " << list[0].insert_value() << ";";
        return query.str();
    }
    std::string column_name(uint8_t index) const {
        if (index == 0) {
            return std::string("id");
        } else if (index == 1) {
            return std::string("expiry");
        } else if (size_t(index - 2) < this->args_.size()) {
            return this->args_[index - 2];
        } else {
            std::stringstream stream;
            stream << "arg" << (int)(index - 1);
            return stream.str();
        }
    }

private: // Private fields
    database::sqlite& driver_;
    std::string name_;
    std::vector<std::string> args_;
    bool use_time_;
    uint64_t limit_;
};

template<typename T, typename std::enable_if<std::is_base_of<operation, T>::value>::type*>
T& manager::emplace(const std::string& name, bool use_time, uint64_t limit) {
    T* object = new T(*this->driver_.get(), name, use_time, limit);
    this->operations_.push_back(object);
    return *object;
}

}}}

#endif // INITPLUS_DATABASE_CACHE_DETAIL_HPP_INCLUDED
