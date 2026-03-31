#ifndef MODBUS_MODBUS_H_INCLUDED
#define MODBUS_MODBUS_H_INCLUDED

/**
 * \file modbus.h
 * \brief Файл содержит включения всех необходимых заголовков.
 *
 * В большинстве случаев для работы с библиотекой достаточно подключить только этот заголовочный файл.
 */

#include "config.h"
#include "master.h"
#include "slave.h"
#include "memory.h"
#include "symbol.h"
#include "stack_trace.h"
#include "debug.h"

/**
 * \mainpage Описание библиотеки
 * \section chapter_1 1. Назначение
 * Библиотека обеспечивает обмен сообщениями с внешними устройствами по протоколу <b>Modbus</b>.
 * Команды управления передаются с верхнего уровня, посредством набора функций,
 * входящих в состав библиотеки. Код библиотеки написан на языке C (стандарт \b C99).
 */

/**
 * \defgroup master_functions Команды управления (Master)
 * \brief Функции, позволяющие управлять подключёнными устройствами.
 */

/**
 * \defgroup slave_functions Команды управления (Slave)
 * \brief Функции, позволяющие добавлять обработчики команд.
 */

/**
 * \defgroup public_functions Внешние функции
 * \brief Функции, вызываемые пользователем библиотеки.
 */

/**
 * \defgroup private_functions Внутренние функции
 * \brief Функции, вызываемые внутри библиотеки.
 */

/**
 * \defgroup endpoint_definition Интерфейс передачи данных
 * \brief Функции, необходимые для обеспечения передачи данных.
 */

/**
 * \defgroup extentions Инструменты расширения
 * \brief Список типов и вспомогательных функций для расширения возможностей библиотеки.
 */

/**
 * \defgroup callback_definitions Обработчики событий
 * \brief Список типов функций, используемых в качестве обработчиков событий библиотеки.
 */

/**
 * \defgroup enum_definitions Перечисления
 * \brief Список перечислений библиотеки.
 */

#endif // MODBUS_MODBUS_H_INCLUDED
