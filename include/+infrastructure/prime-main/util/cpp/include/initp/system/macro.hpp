#ifndef INITPLUS_SYSTEM_MACRO_HPP_INCLUDED
#define INITPLUS_SYSTEM_MACRO_HPP_INCLUDED

#ifdef MIN
#undef MIN
#endif // MIN
/**
 * \brief Выбор минимального из двух операндов.
 */
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#ifdef MAX
#undef MAX
#endif // MAX
/**
 * \brief Выбор максимального из двух операндов.
 */
#define MAX(a, b) ((a) >= (b) ? (a) : (b))

#ifdef BIND
#undef BIND
#endif // BIND
/**
 * \brief Привязка метода класса, в качестве объекта используется \c shared_from_this().
 *
 * Данный макрос использует специфичный синтаксис \b GCC, совместимость с другими компиляторами не гарантирована.
 */
#define BIND(f, ...) std::bind(&self_type::f, this->shared_from_this(), ##__VA_ARGS__)

#ifdef RBIND
#undef RBIND
#endif // RBIND
/**
 * \brief Привязка метода класса, в качестве объекта используется \c this.
 *
 * Данный макрос использует специфичный синтаксис \b GCC, совместимость с другими компиляторами не гарантирована.
 */
#define RBIND(f, ...) std::bind(&self_type::f, this, ##__VA_ARGS__)

#ifdef LOCK_MUTEX
#undef LOCK_MUTEX
#endif // LOCK_MUTEX
/**
 * \brief Создание \c lock_guard с именем \c lock на указанный мьютекс.CL2_CMD_PAY_CHECK_CARD
 */
#define LOCK_MUTEX(a) std::lock_guard<std::mutex> lock(a)

#ifdef LOCK_RMUTEX
#undef LOCK_RMUTEX
#endif // LOCK_RMUTEX
/**
 * \brief Создание \c lock_guard с именем \c lock на указанный рекурсивный мьютекс.
 */
#define LOCK_RMUTEX(a) std::lock_guard<std::recursive_mutex> lock(a)

#endif // INITPLUS_SYSTEM_MACRO_HPP_INCLUDED
