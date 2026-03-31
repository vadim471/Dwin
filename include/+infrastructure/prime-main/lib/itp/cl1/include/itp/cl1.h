#ifndef ITP_CL1_CL1_H_INCLUDED
#define ITP_CL1_CL1_H_INCLUDED

/**
 * \file cl1.h
 * \brief Файл содержит включения всех необходимых заголовков.
 */

#ifdef ITP_COMPILE_CXX
extern "C" {
#endif // ITP_COMPILE_CXX
#include <itp/itp.h>
#ifdef ITP_COMPILE_CXX
}
#endif // ITP_COMPILE_CXX

#include "command.h"
#include "error_code.h"
#include "enum.h"

/**
 * \mainpage ITP Command Layer 1
 * Уровень команд расширяет транспортный протокол \b ITP.<br>
 * Предназначен для управления ТРК и прочими периферийными устройствами на АЗС.<br>
 * \note При использованиии в рамках языка \b C++ необходимо определить макрос \b ITP_COMPILE_CXX.
 * \see \ref commands_list, \ref errors_list, \ref enums_list, \ref tools_list
 */

/**
 * \defgroup commands_list Список команд
 * \brief Команды, поддерживаемые в рамках данного расширения.
 */

/**
 * \defgroup errors_list Список кодов ошибок
 * \brief Коды ошибок, возвращаемые в ответ на команды.
 */

/**
 * \defgroup enums_list Перечисления
 * \brief Полный список перечислений, используемых в командах.
 */

/**
 * \defgroup tools_list Инструменты
 * \brief Различные функции, облегчающие работу с командами.
 */

#endif // ITP_CL1_CL1_H_INCLUDED
