#ifndef MODBUS_MEMORY_H_INCLUDED
#define MODBUS_MEMORY_H_INCLUDED

/**
 * \file memory.h
 * \brief Файл содержит функции и макросы для работы с динамической памятью.
 *
 * По умолчанию работы с памятью осуществляется функциями \c malloc и \c free из библиотеки \c stdlib.h.
 * Для того, чтобы использовать сторонние функции необходимо определить макрос \b \c MODBUS_MEMORY_CUSTOM.
 * В этом случае, вместо макросов <code>\link mbs_malloc \endlink</code> и <code>\link mbs_free \endlink</code>
 * будут вызываться функции с идентичными именами и сигнатурами.
 */

#ifdef mbs_malloc
#undef mbs_malloc
#endif // mbs_malloc

#ifdef mbs_free
#undef mbs_free
#endif // mbs_free

#ifdef MODBUS_MEMORY_CUSTOM

#include <stddef.h>

/**
 * \brief Динамическое выделение памяти.
 * \param[in] size Необходимый размер в байтах.
 * \param[in] what Цель выделения памяти (для отладки).
 * \return Указатель на выделенную область памяти.
 */
void* mbs_malloc(size_t size, const char* what);

/**
 * \brief Освобождение динамически выделенной памяти.
 * \param[in] ptr Указатель на область памяти.
 */
void mbs_free(void*);

#else // MODBUS_MEMORY_CUSTOM

#include <stdlib.h>

/**
 * \brief Динамическое выделение памяти.
 * \param[in] size Необходимый размер в байтах.
 * \param[in] what Цель выделения памяти (для отладки).
 * \return Указатель на выделенную область памяти.
 */
#define mbs_malloc(size, what) malloc(size)

/**
 * \brief Освобождение динамически выделенной памяти.
 * \param[in] ptr Указатель на область памяти.
 */
#define mbs_free(ptr) free(ptr)

#endif // MODBUS_MEMORY_CUSTOM

#endif // MODBUS_MEMORY_H_INCLUDED
