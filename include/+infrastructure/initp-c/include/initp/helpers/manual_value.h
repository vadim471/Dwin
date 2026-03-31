#ifndef INITPLUS_HELPERS_MANUAL_VALUE_H_INCLUDED
#define INITPLUS_HELPERS_MANUAL_VALUE_H_INCLUDED

#include <stdint.h>

/**
 * \file
 */

/**
 * \brief Структура для хранения \c uint8 с разными значениями.
 */
typedef struct {
    uint8_t value;
    uint8_t manual;
} tools_manual_uint8_t;

/**
 * \brief Инициализировать структуру \c tools_manual_uint8_t.
 * \param[in] object Указатель на структуру \c tools_manual_uint8_t.
 * \param[in] value Текущее значение.
 */
void tools_manual_init_uint8(tools_manual_uint8_t* object, uint8_t value);

/**
 * \brief Получить текущее значение из структуры \c tools_manual_uint8_t.
 * \param[in] object Указатель на структуру \c tools_manual_uint8_t.
 */
uint8_t tools_manual_get_uint8(const tools_manual_uint8_t* object);

#endif // INITPLUS_HELPERS_MANUAL_VALUE_H_INCLUDED
