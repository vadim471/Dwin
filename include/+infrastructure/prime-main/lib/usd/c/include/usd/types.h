#ifndef USD_TYPES_H_INCLUDED
#define USD_TYPES_H_INCLUDED

/**
 * \file types.h
 * \brief Файл содержит предварительные объявления структур и указателей на них.
 */

#include "error_code.h"

struct usd_device;

/**
 * \brief Тип структуры универсального устройства <code>\link usd_device \endlink</code>.
 */
typedef struct usd_device usd_device_t;

/**
 * \brief Указатель на структуру универсального устройства <code>\link usd_device \endlink</code>.
 */
typedef usd_device_t* usd_device_tp;

struct usd_parameter;

/**
 * \brief Тип структуры универсального параметра <code>\link usd_parameter \endlink</code>.
 */
typedef struct usd_parameter usd_parameter_t;

/**
 * \brief Указатель на структуру универсального параметра <code>\link usd_parameter \endlink</code>.
 */
typedef usd_parameter_t* usd_parameter_tp;

/**
 * \brief Тип функции, используемой добавления устройств к объекту.
 * \param[in] object Произвольный объект.
 * \param[in] device Указатель на структуру универсального устройства \c usd_device.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
typedef usd_error_code_t(*usd_push_device_fp)(void* object, usd_device_tp device);

#endif // USD_TYPES_H_INCLUDED
