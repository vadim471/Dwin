#ifndef INITPLUS_PLATFORM_SERIAL_PORT_H_INCLUDED
#define INITPLUS_PLATFORM_SERIAL_PORT_H_INCLUDED

/**
 * \file
 */

#include <stdint.h>
#include <stddef.h>

#if defined(WIN32)

#include <windows.h>

#define MAX_SERIAL_PORT_PATH_LENGTH 10

typedef struct {
    char path[MAX_SERIAL_PORT_PATH_LENGTH + 1];
    uint32_t baudrate;
    uint8_t databits;
    uint8_t stopbits;
    uint8_t parity;
    uint8_t configured;
    uint8_t opened;
    HANDLE handle;
} sys_serial_port_t;

#elif defined(POSIX)

#define MAX_SERIAL_PORT_PATH_LENGTH 40

typedef struct {
    char path[MAX_SERIAL_PORT_PATH_LENGTH + 1];
    uint32_t baudrate;
    uint8_t databits;
    uint8_t stopbits;
    uint8_t parity;
    uint8_t configured;
    uint8_t opened;
    int fd;
} sys_serial_port_t;

#elif defined(STM32F407VG)

typedef struct {
    int portId;
} sys_serial_port_t;

#endif // WIN32|POSIX|etc

/**
 * \brief Указатель на структуру последовательного порта \c sys_serial_port_t.
 *
 * Поля структуры зависят от платформы.
 * \ingroup platform_serial_port
 */
typedef sys_serial_port_t* sys_serial_port_tp;

/**
 * \brief Инициализировать структуру последовательного порта.
 * \ingroup platform_serial_port
 * \param[in] object Указатель на структуру последовательного порта.
 */
void sys_init_serial_port(sys_serial_port_tp object);

/**
 * \brief Задать настройки последовательного порта.
 * \ingroup platform_serial_port
 * \param[in] object Указатель на структуру последовательного порта.
 * \param[in] path Путь к устройству (зависит от платформы).
 * \param[in] baudrate Скорость соединения (до 115200).
 * \param[in] databits Количество значащих бит (7 или 8).
 * \param[in] stopbits Количество стоповых бит (1 или 2).
 * \param[in] parity Контроль чётности (0 - нет, 1 - нечет, 2 - чёт).
 * \return Если во время операции возникли ошибки - 0 (ноль), иначе - 1 (единицу).
 */
uint8_t sys_configure_serial_port(sys_serial_port_tp object, const char* path, uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity);

/**
 * \brief Открыть последовательный порт с указанными настройками.
 * \ingroup platform_serial_port
 * \param[in] object Указатель на структуру последовательного порта.
 * \param[in] path Путь к устройству (зависит от платформы).
 * \param[in] baudrate Скорость соединения (до 115200).
 * \param[in] databits Количество значащих бит (7 или 8).
 * \param[in] stopbits Количество стоповых бит (1 или 2).
 * \param[in] parity Контроль чётности (0 - нет, 1 - нечет, 2 - чёт).
 * \return Если во время операции возникли ошибки - 0 (ноль), иначе - 1 (единицу).
 */
uint8_t sys_open_serial_port(sys_serial_port_tp object, const char* path, uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity);

/**
 * \brief Открыть последовательный порт с сохранёнными настройками.
 * \ingroup platform_serial_port
 * \param[in] object Указатель на структуру последовательного порта.
 * \return Если во время операции возникли ошибки - 0 (ноль), иначе - 1 (единицу).
 */
uint8_t sys_reopen_serial_port(sys_serial_port_tp object);

/**
 * \brief Закрыть последовательный порт.
 * \ingroup platform_serial_port
 * \param[in] object Указатель на структуру последовательного порта.
 */
void sys_close_serial_port(sys_serial_port_tp object);

/**
 * \brief Неблокирующая операция чтения из последовательного порта.
 * \ingroup platform_serial_port
 * \param[in] object Указатель на структуру последовательного порта.
 * \param[in] data Указатель на буфер для приёма данных.
 * \param[in] size Размер буфера для приёма данных.
 * \return Число прочитанных байт данных.
 */
size_t sys_read_serial_port(sys_serial_port_tp object, char* data, size_t size);

typedef size_t(*sys_serial_port_read_fp)(sys_serial_port_tp, char*, size_t);

/**
 * \brief Неблокирующая операция записи в последовательный порт.
 * \ingroup platform_serial_port
 * \param[in] object Указатель на структуру последовательного порта.
 * \param[in] data Указатель на буфер с данными.
 * \param[in] length Количество байт данных, ожидающих записи.
 * \return Число записанных байт данных.
 */
size_t sys_write_serial_port(sys_serial_port_tp object, const char* data, size_t length);

typedef size_t(*sys_serial_port_write_fp)(sys_serial_port_tp, const char*, size_t);

#endif // INITPLUS_PLATFORM_SERIAL_PORT_H_INCLUDED
