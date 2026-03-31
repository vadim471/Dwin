#ifndef MODBUS_TOOLS_H_INCLUDED
#define MODBUS_TOOLS_H_INCLUDED

/**
 * \file tools.h
 * \brief Файл содержит служебные функции.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"

/**
 * \brief Чтение одного регистра.
 * \param[in] data Указатель на блок данных.
 * \return Значение регистра.
 */
uint16_t mbs_read_register(const mbs_byte_t* data);

/**
 * \brief Запись одного регистра.
 * \param[in] data Указатель на блок данных.
 * \param[in] value Значение регистра.
 */
void mbs_write_register(mbs_byte_t* data, uint16_t value);

#endif // MODBUS_TOOLS_H_INCLUDED
