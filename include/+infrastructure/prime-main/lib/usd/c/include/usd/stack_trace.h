#ifndef USD_STACK_TRACE_H_INCLUDED
#define USD_STACK_TRACE_H_INCLUDED

/**
 * \file stack_trace.h
 * \brief Файл содержит отладочные функции.
 */

#include "config.h"

#include <stdio.h>

#ifdef USD_USE_STACK_TRACE

void usd_init_stack_trace(const char* executable);

void usd_print_stack_trace(FILE* stream);

#else // USD_USE_STACK_TRACE

/**
 * \brief Инициализация модуля работы со стеком вызовов.
 *
 * Исполняемый файл должен быть скомпилирован с отладочной информацией и без оптимизации.
 * \param[in] executable Имя исполняемого файла, содержащего код библиотеки.
 */
#define usd_init_stack_trace(executable)

/**
 * \brief Вывести в указанный файл текущий стек вызовов.
 * \param[in] stream Файл для вывода информации.
 */
#define usd_print_stack_trace(stream)

#endif // USD_USE_STACK_TRACE

#endif // USD_STACK_TRACE_H_INCLUDED
