#ifndef INITPLUS_UTILS_MACRO_HPP_INCLUDED
#define INITPLUS_UTILS_MACRO_HPP_INCLUDED

#ifdef CAST
#undef CAST
#endif // CAST
/**
 * \brief Приведение типов.
 */
#define CAST(t, p) ((t)(p))

#ifdef PTR
#undef PTR
#endif // PTR
/**
 * \brief Приведение типа с побайтовым смещением указателя.
 */
#define PTR(type, address, offset) ((type*)((unsigned char*)(address) + (offset)))

#ifdef BTS
#undef BTS
#endif // BTS
/**
 * \brief Строковое представление типа \c bool.
 */
#define BTS(name) ((name) ? "true" : "false")

#ifdef IT
#undef IT
#endif // IT
/**
 * \brief Разыменовыванный итератор с именем \c it.
 */
#define IT (*it)

#ifdef MAX
#undef MAX
#endif // MAX
/**
 * \brief Выбор максимального из двух операндов.
 */
#define MAX(a, b) ( (a) >= (b) ? (a) : (b) )
#ifdef MIN
#undef MIN
#endif // MIN
/**
 * \brief Выбор минимального из двух операндов.
 */
#define MIN(a, b) ( (a) <  (b) ? (a) : (b) )

#ifdef each_i
#undef each_i
#endif // each_i
/**
 * \brief Цикл \c for с простым итератором.
 */
#define each_i(type, it, arr) (type::iterator it = (arr).begin(), it##_end = (arr).end(); it != it##_end; ++it)
#ifdef each_c
#undef each_c
#endif // each_c
/**
 * \brief Цикл \c for с константным итератором.
 */
#define each_c(type, it, arr) (type::const_iterator it = (arr).cbegin(), it##_cend = (arr).cend(); it != it##_cend; ++it)
#ifdef each_r
#undef each_r
#endif // each_r
/**
 * \brief Цикл \c for с обратным итератором.
 */
#define each_r(type, it, arr) (type::reverse_iterator it = (arr).rbegin(), it##_rend = (arr).rend(); it != it##_rend; ++it)

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
 * \brief Создание \c lock_guard с именем \c lock на указанный мьютекс.
 */
#define LOCK_MUTEX(a) std::lock_guard<std::mutex> lock(a)

#ifdef LOCK_RMUTEX
#undef LOCK_RMUTEX
#endif // LOCK_RMUTEX
/**
 * \brief Создание \c lock_guard с именем \c lock на указанный рекурсивный мьютекс.
 */
#define LOCK_RMUTEX(a) std::lock_guard<std::recursive_mutex> lock(a)

#endif // INITPLUS_UTILS_MACRO_HPP_INCLUDED
