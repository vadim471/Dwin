#ifndef INITPLUS_SYSTEM_DEBUG_H_INCLUDED
#define INITPLUS_SYSTEM_DEBUG_H_INCLUDED

/**
 * \file
 */

#define SYSTEM_LEVEL_TRACE 0
#define SYSTEM_LEVEL_DEBUG 1
#define SYSTEM_LEVEL_INFO 2
#define SYSTEM_LEVEL_WARNING 3
#define SYSTEM_LEVEL_ERROR 4
#define SYSTEM_LEVEL_FATAL 5

#if defined(SYSTEM_DEBUG) || defined(SYSTEM_DEBUG_CUSTOM)

#ifdef sys_debug_print
#undef sys_debug_print
#endif // sys_debug_print

#ifndef SYSTEM_DEBUG_CUSTOM

#include <stdio.h>

/**
 * \brief Вывод отладочной информации в \c stdout.
 * \ingroup system_debug
 * \param[in] level
 * \param[in] what
 * \param[in] fmt
 * \param[in] ...
 */
#define sys_debug_print(level, what, fmt, ...) \
    do { fprintf(stderr, "%s> " fmt "\r\n", what, ##__VA_ARGS__); } while (0)

#else // SYSTEM_DEBUG_CUSTOM

#include <stdarg.h>

/**
 * \brief Вывод отладочной информации в \c stdout.
 * \ingroup system_debug
 * \param[in] level
 * \param[in] what
 * \param[in] fmt
 * \param[in] ...
 */
void sys_debug_print(char level, const char* what, const char* fmt, ...);

#endif // SYSTEM_DEBUG_CUSTOM

#else // SYSTEM_DEBUG || SYSTEM_DEBUG_CUSTOM

/**
 * \brief Вывод отладочной информации в \c stdout.
 * \ingroup system_debug
 * \param[in] level
 * \param[in] what
 * \param[in] fmt
 * \param[in] ...
 */
#define sys_debug_print(level, what, fmt, ...)

#endif // SYSTEM_DEBUG || SYSTEM_DEBUG_CUSTOM

#endif // INITPLUS_SYSTEM_DEBUG_H_INCLUDED
