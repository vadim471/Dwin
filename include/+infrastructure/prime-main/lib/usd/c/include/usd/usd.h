#ifndef USD_USD_H_INCLUDED
#define USD_USD_H_INCLUDED

/**
 * \file usd.h
 * \brief Файл содержит включения всех необходимых заголовков.
 *
 * В большинстве случаев для работы с библиотекой достаточно подключить только этот заголовочный файл.
 */

#include "config.h"
#include "types.h"
#include "memory.h"
#include "error_code.h"
#include "device.h"
#include "group.h"
#include "parameter.h"

/**
 * \mainpage Universal Sensor Device
 * Данный программный интерфейс предназначен для унификации доступа к устройствам ввода-вывода.<br>
 * Набор параметров \c usd_parameter хранится в виде массива указателей в структуре устройства \c usd_device.
 * В свою очередь, устройство может быть добавлено в односвязный список структуры \c usd_device_group.<br>
 * В структуре параметра допускается хранить как значение, так и указатель на функцию получения значения.
 * Если параметр хранится в структуре, запись в него осуществляется функцией <code>\link usd_set_parameter \endlink</code>.
 * Чтение значений всегда осуществляется функцией <code>\link usd_get_parameter \endlink</code>.
 * \see \ref group_functions, \ref device_functions, \ref parameter_functions, \ref variant_definitions
 */

/**
 * \defgroup group_functions Группа устройств
 * \brief Функции для работы с группой устройств.
 */

/**
 * \defgroup device_functions Устройство
 * \brief Функции для работы с универсальным устройством.
 */

/**
 * \defgroup parameter_functions Параметр
 * \brief Функции для работы с параметрами.
 */

/**
 * \defgroup variant_definitions Тип variant
 * \brief Описание типов данных, используемых для хранения параметров.
 */

#endif // USD_USD_H_INCLUDED
