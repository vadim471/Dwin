#include <initp/database/cache.hpp>
#include <initp/database/sqlite.hpp>

namespace initp {
namespace database {
namespace cache {

manager::manager(const std::string& name):
    driver_(new database::sqlite(name))
{}

manager::~manager(void) {
    for (operation* object : this->operations_) {
        delete object;
    }
}

void manager::close(void) {
    this->driver_->close();
}

size_t manager::size(void) const {
    return this->operations_.size();
}

}}}
