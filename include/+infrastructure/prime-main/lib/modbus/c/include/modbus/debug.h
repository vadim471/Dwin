#ifndef MODBUS_DEBUG_H_INCLUDED
#define MODBUS_DEBUG_H_INCLUDED

/**
 * \file debug.h
 * \brief Файл содержит отладочные макросы и функции.
 */

#include "config.h"

#if defined(MODBUS_DEBUG) || defined(MODBUS_DEBUG_CUSTOM)
#ifdef mbs_debug_print
#undef mbs_debug_print
#endif // mbs_debug_print
#ifndef MODBUS_DEBUG_CUSTOM
#include <stdio.h>

#define mbs_debug_print(err, fmt, ...) \
    do { fprintf(stderr, "" fmt"\r\n", ##__VA_ARGS__); } while (0)

#else // MODBUS_DEBUG_CUSTOM
#include <stdarg.h>

void mbs_debug_print(uint8_t err, const char* fmt, ...);

#endif // MODBUS_DEBUG_CUSTOM
#else // MODBUS_DEBUG || MODBUS_DEBUG_CUSTOM

/**
 * \brief Вывод отладочной информации.
 * \param[in] err Код ошибки \c sens_error_code_t.
 * \param[in] fmt Сообщение с форматированием.
 * \param[in] ... Параметры форматирования.
 */
#define mbs_debug_print(err, fmt, ...)

#endif // MODBUS_DEBUG || MODBUS_DEBUG_CUSTOM

#endif // MODBUS_DEBUG_H_INCLUDED
