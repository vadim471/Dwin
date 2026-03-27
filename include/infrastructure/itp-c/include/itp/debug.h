#ifndef ITP_DEBUG_H_INCLUDED
#define ITP_DEBUG_H_INCLUDED

/**
 * \file debug.h
 * \brief Файл содержит отладочные макросы и функции.
 */

#include "config.h"

#if defined(ITP_DEBUG) || defined(ITP_DEBUG_CUSTOM)
#ifdef itp_debug_print
#undef itp_debug_print
#endif // itp_debug_print
#ifndef ITP_DEBUG_CUSTOM
#include <stdio.h>

/**
 * \brief Вывод отладочной информации.
 * \param[in] err
 * \param[in] fmt
 * \param[in] ...
 */
#define itp_debug_print(err, fmt, ...) \
    do { fprintf(stderr, "" fmt"\r\n", ##__VA_ARGS__); } while (0)

#else // ITP_DEBUG_CUSTOM
#include <stdarg.h>

/**
 * \brief Вывод отладочной информации.
 * \param[in] err
 * \param[in] fmt
 * \param[in] ...
 */
void itp_debug_print(uint8_t err, const char* fmt, ...);

#endif // ITP_DEBUG_CUSTOM
#else // ITP_DEBUG || ITP_DEBUG_CUSTOM
#define itp_debug_print(err, fmt, ...)
#endif // ITP_DEBUG || ITP_DEBUG_CUSTOM

#endif // ITP_DEBUG_H_INCLUDED
