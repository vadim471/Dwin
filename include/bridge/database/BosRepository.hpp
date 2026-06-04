#ifndef DWIN_EXMPL_BOSREPOSITORY_HPP
#define DWIN_EXMPL_BOSREPOSITORY_HPP

#include <string>
#include <memory>

#include "Database.hpp"


namespace bridge {

    class BosRepository {
    public:
        explicit BosRepository(std::shared_ptr<Database> db);
        void createTable();
        int64_t insert(int type, const std::string& payload);

    private:
        std::shared_ptr<Database> db_;
    };

} // namespace bridge

#endif // DWIN_EXMPL_BOSREPOSITORY_HPP