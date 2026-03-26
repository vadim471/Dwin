#ifndef INITPLUS_PLATFORM_KEYBOARD_H_INCLUDED
#define INITPLUS_PLATFORM_KEYBOARD_H_INCLUDED

/**
 * \file
 */

/**
 * \brief Тип обработчика нажатия Ctrl+C.
 * \ingroup platform_keyboard
 */
typedef void(*sys_on_ctrlc_fp)(void);

/**
 * \brief Установить неблокирующий режим ввода.
 * \ingroup platform_keyboard
 * \param[in] on_ctrlc Обработчик нажатия Ctrl+C.
 */
void sys_enable_async_keyboard(sys_on_ctrlc_fp on_ctrlc);

/**
 * \brief Отключить неблокирующий режим ввода.
 * \ingroup platform_keyboard
 */
void sys_disable_async_keyboard(void);

/**
 * \brief Проверка событий при включённом неблокирующем режиме ввода.
 * \ingroup platform_keyboard
 * \return Зависит от платформы. Если нажатий не было, возвращает 0 (ноль).
 */
int sys_is_keyboard_hit(void);

/**
 * \brief Получение символа при включённом неблокирующем режиме ввода.
 * \ingroup platform_keyboard
 * \return Если не удалось получить символ возвращает 0 (ноль).
 */
char sys_keyboard_get_char(void);

#endif // INITPLUS_PLATFORM_KEYBOARD_H_INCLUDED
