#ifndef INITPLUS_SYSTEM_TIME_H_INCLUDED
#define INITPLUS_SYSTEM_TIME_H_INCLUDED

/**
 * \file
 */

#include <stdint.h>

/**
 * \brief Тип представления времени в миллисекундах.
 */
typedef uint64_t sys_time_t;

/**
 * \brief Системные часы в миллисекундах.
 * \ingroup system_time
 * \return Время в миллисекундах.
 */
sys_time_t sys_clock_ms(void);

/**
 * \brief Отложить выполнение текущего потока на указанное время.
 * \ingroup system_time
 * \param[in] milliseconds Время в миллисекундах.
 */
void sys_sleep_for(sys_time_t milliseconds);

#endif // INITPLUS_SYSTEM_TIME_H_INCLUDED
