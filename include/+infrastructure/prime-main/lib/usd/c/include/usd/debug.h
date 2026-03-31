#ifndef USD_DEBUG_H_INCLUDED
#define USD_DEBUG_H_INCLUDED

/**
 * \file debug.h
 * \brief Файл содержит отладочные макросы и функции.
 */

#include "config.h"

#if defined(USD_DEBUG) || defined(USD_DEBUG_CUSTOM)
#ifdef usd_debug_print
#undef usd_debug_print
#endif // usd_debug_print
#ifndef USD_DEBUG_CUSTOM
#include <stdio.h>

#define usd_debug_print(err, fmt, ...) \
    do { fprintf(stderr, "" fmt"\r\n", ##__VA_ARGS__); } while (0)

#else // USD_DEBUG_CUSTOM
#include <stdarg.h>

void usd_debug_print(uint8_t err, const char* fmt, ...);

#endif // USD_DEBUG_CUSTOM
#else // USD_DEBUG || USD_DEBUG_CUSTOM

/**
 * \brief Вывод отладочной информации.
 * \param[in] err Код ошибки \c usd_error_code_t.
 * \param[in] fmt Сообщение с форматированием.
 * \param[in] ... Параметры форматирования.
 */
#define usd_debug_print(err, fmt, ...)

#endif // USD_DEBUG || USD_DEBUG_CUSTOM

#endif // USD_DEBUG_H_INCLUDED
