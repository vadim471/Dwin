#ifndef INITPLUS_DATABASE_CACHE_HPP_INCLUDED
#define INITPLUS_DATABASE_CACHE_HPP_INCLUDED

#include "../system/error_code.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>

namespace initp {
namespace database {

class sqlite;

namespace cache {

class operation {
public:
    operation(void) {}
    virtual ~operation(void) = default;
public:
    virtual bool empty(void) const = 0;
    virtual system::error_code execute(void) = 0;
};

class manager {

private: // Private types
    typedef manager self_type;

public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::unique_ptr<self_type> uptr;

public: // Construction
    manager(const std::string& name);
    manager(const self_type&) = delete;
    virtual ~manager(void);

public: // Public methods

    template<typename T, typename std::enable_if<std::is_base_of<operation, T>::value>::type* = nullptr>
    T& emplace(const std::string& name, bool use_time, uint64_t limit);

    void close(void);

    size_t size(void) const;

private: // Private fields
    std::unique_ptr<database::sqlite> driver_;
    std::vector<operation*> operations_;
};

}}}

#endif // INITPLUS_DATABASE_CACHE_HPP_INCLUDED
