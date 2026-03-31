#ifndef MODBUS_STACK_TRACE_H_INCLUDED
#define MODBUS_STACK_TRACE_H_INCLUDED

/**
 * \file stack_trace.h
 * \brief Файл содержит отладочные функции.
 */

#include "config.h"

#include <stdio.h>

#ifdef MODBUS_USE_STACK_TRACE

void mbs_init_stack_trace(const char* executable);

void mbs_print_stack_trace(FILE* stream);

#else // MODBUS_USE_STACK_TRACE

/**
 * \brief Инициализация модуля работы со стеком вызовов.
 *
 * Исполняемый файл должен быть скомпилирован с отладочной информацией и без оптимизации.
 * \param[in] executable Имя исполняемого файла, содержащего код библиотеки.
 */
#define mbs_init_stack_trace(executable)

/**
 * \brief Вывести в указанный файл текущий стек вызовов.
 * \param[in] stream Файл для вывода информации.
 */
#define mbs_print_stack_trace(stream)

#endif // MODBUS_USE_STACK_TRACE

#endif // MODBUS_STACK_TRACE_H_INCLUDED
