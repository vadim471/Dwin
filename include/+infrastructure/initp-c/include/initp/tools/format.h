#ifndef INITPLUS_TOOLS_FORMAT_H_INCLUDED
#define INITPLUS_TOOLS_FORMAT_H_INCLUDED

/**
 * \file
 */

#include <stddef.h>
#include <stdint.h>

/**
 * \brief Записать массив в строку в виде шестнадцатиричной последовательности.
 *
 * Если строка не входит в буфер, будет записано только то, что возможно.
 * \ingroup tools_format
 * \param[in] data Массив байт для вывода.
 * \param[in] size Размера массива.
 * \param[out] buffer Буфер для записи строки. Должен быть в 3 раза больше массива.
 * \param[in] length Размер буфера для строки.
 * \return В случае успеха 1, иначе 0.
 */
uint8_t tools_array_to_string(const uint8_t* data, size_t size, char* buffer, size_t length);

void sys_trace_array(const char* what, const char* data, size_t size);

/**
 * \brief Конвертировать массив в BCD.
 * \param[in] data Указатель на массив данных.
 * \param[in] size Размер массива.
 * \param[in] buffer Указатель на буфер для вывода.
 */
void tools_char_to_bcd(const char* data, uint8_t size, uint8_t* buffer);

#endif // INITPLUS_TOOLS_FORMAT_H_INCLUDED
