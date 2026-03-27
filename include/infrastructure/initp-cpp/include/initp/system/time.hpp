#ifndef INITPLUS_SYSTEM_TIME_HPP_INCLUDED
#define INITPLUS_SYSTEM_TIME_HPP_INCLUDED

#include <chrono>
#include <thread>

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
    static initp::system::time_t milliseconds(void) {
        return (
            std::chrono::system_clock::now().time_since_epoch() /
            std::chrono::milliseconds(1)
        );
    }
    /**
     * \brief ќтложить выполение текущего потока на указанное врем€.
     * \param[in] value ¬рем€ в миллисекундах.
     */
    static void sleep_for(system::time_t value) {
        return std::this_thread::sleep_for(std::chrono::milliseconds(value));
    }
};

}}

#endif // INITPLUS_SYSTEM_TIME_HPP_INCLUDED
