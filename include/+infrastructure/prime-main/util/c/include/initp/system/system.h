#ifndef INITPLUS_SYSTEM_SYSTEM_H_INCLUDED
#define INITPLUS_SYSTEM_SYSTEM_H_INCLUDED

/**
 * \file
 */

/**
 * \brief Системная команда \c echo.
 * \param[in] message Сообщения.
 * \param[in] device Устройство (опционально).
 */
void sys_echo(const char* message, const char* device);

/**
 * \brief Установить яркость диода на максимум.
 * \param[in] led Имя устройства в системе.
 */
void sys_led_on(const char* led);

/**
 * \brief Установить яркость диода на минимум.
 * \param[in] led Имя устройства в системе.
 */
void sys_led_off(const char* led);

/**
 * \brief Установить триггер диода на \c heartbeat.
 * \param[in] led Имя устройства в системе.
 */
void sys_led_blink(const char* led);

/**
 * \brief Синхронизировать файловую систему.
 */
void sys_sync(void);

/**
 * \brief Изменить права доступа к файлу.
 * \param[in] path Путь к файлу или папке.
 * \param[in] mode Флаги доступа или модификатор (например, 755 или a+x).
 */
void sys_change_mode(const char* path, const char* mode);

/**
 * \brief Перезапустить устройство.
 */
void sys_reboot(void);

/**
 * \brief Выполнить указанную системную команду.
 * \param[in] command Текст команды.
 */
void sys_execute(const char* command);

#endif // INITPLUS_SYSTEM_SYSTEM_H_INCLUDED
