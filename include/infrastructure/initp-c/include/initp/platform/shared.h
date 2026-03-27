#ifndef INITPLUS_PLATFORM_SHARED_H_INCLUDED
#define INITPLUS_PLATFORM_SHARED_H_INCLUDED

/**
 * \file
 */

extern const char* sys_default_module_extention;
extern const char* sys_default_module_prefix;

/**
 * \brief Загрузить динамическую библиотеку.
 * \ingroup platform_shared
 * \param[in] path Путь к библиотеке.
 * \return Дескриптор модуля или \c NULL в случае ошибки.
 */
void* sys_load_module(const char* path);

/**
 * \brief Найти функцию по имени.
 * \ingroup platform_shared
 * \param[in] handle Дескриптор модуля.
 * \param[in] name Имя функции.
 * \return Адрес функции или \c NULL в случае ошибки.
 */
void* sys_get_address(void* handle, const char* name);

/**
 * \brief Освободить динамическую библиотеку.
 * \ingroup platform_shared
 * \param[in] handle Дескриптор модуля.
 */
void sys_free_module(void* handle);

/**
 * \brief Освободить динамически выделенную память.
 * \ingroup platform_shared
 * \param[in] address Адрес в памяти.
 */
void sys_global_free(void* address);

#endif // INITPLUS_PLATFORM_SHARED_H_INCLUDED
