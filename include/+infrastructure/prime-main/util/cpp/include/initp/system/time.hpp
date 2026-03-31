#ifndef INITPLUS_SYSTEM_TIME_HPP_INCLUDED
#define INITPLUS_SYSTEM_TIME_HPP_INCLUDED

#include <chrono>

namespace initp {
namespace system {

/**
 * \brief “ип, определ€ющий врем€ в миллисекундах.
 */
typedef std::chrono::milliseconds::rep time_t;

/**
 * \brief ‘ункции дл€ работы со временем.
 */
struct time {
    /**
     * \brief ¬рем€ с начала эпохи UNIX в миллисекундах (GMT).
     */
    static initp::system::time_t now(void) {
        return (
            std::chrono::system_clock::now().time_since_epoch() /
            std::chrono::milliseconds(1)
        );
    }
};

}}

#endif // INITPLUS_SYSTEM_TIME_HPP_INCLUDED
