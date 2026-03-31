#ifndef INITPLUS_DATABASE_SQLITE_HPP_INCLUDED
#define INITPLUS_DATABASE_SQLITE_HPP_INCLUDED

#include <initp/system/error_code.hpp>

#include <memory>
#include <vector>

namespace SQLite {

class Database;
class Statement;
class Column;

}

namespace initp {
namespace database {

class sqlite:
    std::enable_shared_from_this<sqlite> {

private: // Private types
    typedef sqlite self_type;

public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::unique_ptr<self_type> uptr;

public: // Statement class
    class statement {
        typedef statement self_type;
    public:
        typedef std::unique_ptr<self_type> uptr;
    public:
        statement(SQLite::Database& aDatabase, const std::string& aQuery);
        statement(SQLite::Database& aDatabase, const char* aQuery);
        statement(const self_type&) = delete;
        virtual ~statement(void);
    public:
        bool executeStep(void);
        std::unique_ptr<SQLite::Column> getColumn(const int aIndex);
        std::unique_ptr<SQLite::Column> getColumn(const char* apName);
        bool isColumnNull(const int aIndex) const;
        bool isColumnNull(const char* apName) const;
    private:
        std::unique_ptr<SQLite::Statement> sql_;
    };

public: // Construction
    sqlite(const std::string&);
    sqlite(const self_type&) = delete;
    virtual ~sqlite(void);

public: // Public methods
    system::error_code open(void);
    system::error_code close(void);
    system::error_code set_table(const std::string&);
    system::error_code set_or_create_table(const std::string&, const std::vector<std::string>&);
    bool is_open(void) const;
    bool is_exist(const std::string&) const;
    bool is_table_set(const std::string&) const;

public: // Query methods
    system::error_code execute(const std::string&);
    system::error_code upsert(const std::string&, const std::string&, const std::string&, const std::string&);
    statement::uptr select(const std::string&);

private: // Private fields
    std::string path_;
    std::string table_;
    std::unique_ptr<SQLite::Database> instance_;
};

}}

#endif // INITPLUS_DATABASE_SQLITE_HPP_INCLUDED
