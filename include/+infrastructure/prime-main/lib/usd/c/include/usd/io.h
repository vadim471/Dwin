#ifndef USD_IO_H_INCLUDED
#define USD_IO_H_INCLUDED

/**
 * \file io.h
 * \brief Файл содержит описание универсального ввода-вывода.
 */

#include "group.h"

/**
 * \brief Структура универсального ввода-вывода.
 */
struct usd_io {
    /**
     * \brief Структура группы устройств.
     */
    usd_device_group_t devices;
};

/**
 * \brief Тип структуры универсального ввода-вывода <code>\link usd_io \endlink</code>.
 */
typedef struct usd_io usd_io_t;

/**
 * \brief Указатель на структуру универсального ввода-вывода <code>\link usd_io \endlink</code>.
 */
typedef usd_io_t* usd_io_tp;

/**
 * \brief Инициализация структуры универсального ввода-вывода.
 * \param[in] object Указатель на структуру универсального ввода-вывода \c usd_io.
 */
void usd_init_io(usd_io_tp io);

/**
 * \brief Очистка памяти, занимаемой структурой универсального ввода-вывода.
 * \param[in] object Указатель на структуру универсального ввода-вывода \c usd_io.
 */
void usd_free_io(usd_io_tp io);

#endif // USD_IO_H_INCLUDED
