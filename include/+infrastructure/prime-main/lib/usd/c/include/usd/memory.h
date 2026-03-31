#ifndef USD_MEMORY_H_INCLUDED
#define USD_MEMORY_H_INCLUDED

/**
 * \file memory.h
 * \brief Файл содержит функции и макросы для работы с динамической памятью.
 *
 * По умолчанию работы с памятью осуществляется функциями \c malloc и \c free из библиотеки \c stdlib.h.
 * Для того, чтобы использовать сторонние функции необходимо определить макрос \b \c USD_MEMORY_CUSTOM.
 * В этом случае, вместо макросов <code>\link usd_malloc \endlink</code> и <code>\link usd_free \endlink</code>
 * будут вызываться функции с идентичными именами и сигнатурами.
 *
 * \warning This library eats memory like a Pacman.
 */

#ifdef usd_malloc
#undef usd_malloc
#endif // usd_malloc

#ifdef usd_free
#undef usd_free
#endif // usd_free

#ifdef USD_MEMORY_CUSTOM

#include <stddef.h>

/**
 * \brief Динамическое выделение памяти.
 * \param[in] size Необходимый размер в байтах.
 * \param[in] what Цель выделения памяти (для отладки).
 * \return Указатель на выделенную область памяти.
 */
void* usd_malloc(size_t size, const char* what);

/**
 * \brief Освобождение динамически выделенной памяти.
 * \param[in] ptr Указатель на область памяти.
 */
void usd_free(void* ptr);

#else // USD_MEMORY_CUSTOM

#include <stdlib.h>

/**
 * \brief Динамическое выделение памяти.
 * \param[in] size Необходимый размер в байтах.
 * \param[in] what Цель выделения памяти (для отладки).
 * \return Указатель на выделенную область памяти.
 */
#define usd_malloc(size, what) malloc(size)

/**
 * \brief Освобождение динамически выделенной памяти.
 * \param[in] ptr Указатель на область памяти.
 */
#define usd_free(ptr) free(ptr)

#endif // USD_MEMORY_CUSTOM

#endif // USD_MEMORY_H_INCLUDED
