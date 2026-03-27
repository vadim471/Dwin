#ifndef INITPLUS_PLATFORM_NAMED_PIPE_H_INCLUDED
#define INITPLUS_PLATFORM_NAMED_PIPE_H_INCLUDED

/**
 * \file
 */

#include <stdint.h>
#include <stddef.h>

typedef void(*sys_on_named_pipe_connect_fp)(size_t);

#if defined(WIN32)

#include <windows.h>

#define NAMEDPIPE_BUFSIZE 4096

typedef struct {
    HANDLE handle;
    HANDLE hEvent;
    OVERLAPPED oOverlap;
    BOOL fPending;
    BOOL flush;
    BOOL is_server;
    BOOL connected;
    sys_on_named_pipe_connect_fp on_connect;
    size_t on_connect_eparam;
    sys_on_named_pipe_connect_fp on_disconnect;
    size_t on_disconnect_eparam;
    char path[0x80];
} sys_named_pipe_t;

#elif defined(POSIX)

typedef struct {
    int fd_r;
    int fd_w;
    int error;
    uint8_t flush;
    uint8_t is_server;
    uint8_t connected;
    sys_on_named_pipe_connect_fp on_connect;
    size_t on_connect_eparam;
    sys_on_named_pipe_connect_fp on_disconnect;
    size_t on_disconnect_eparam;
    char path[0x80];
} sys_named_pipe_t;

#endif // WIN32|POSIX|etc

/**
 * \brief Указатель на структуру именованного канала \c sys_named_pipe_t.
 *
 * Поля структуры зависят от платформы.
 * \ingroup platform_named_pipe
 */
typedef sys_named_pipe_t* sys_named_pipe_tp;

/**
 * \brief Создать именованный канал.
 * \ingroup platform_named_pipe
 * \param[in] object Указатель на структуру именованного канала.
 * \param[in] path Путь к устройству (зависит от платформы).
 * \return Если во время операции возникли ошибки - 0 (ноль), иначе - 1 (единицу).
 */
uint8_t sys_init_named_pipe(sys_named_pipe_tp object, const char* path);

/**
 * \brief Создать именованный канал.
 * \ingroup platform_named_pipe
 * \param[in] object Указатель на структуру именованного канала.
 * \return Указатель на структуру именованного канала или \c NULL.
 */
uint8_t sys_create_named_pipe(sys_named_pipe_tp object);

/**
 * \brief Открыть именованный канал.
 * \ingroup platform_named_pipe
 * \param[in] object Указатель на структуру именованного канала.
 * \param[in] flush Очистить канал после открытия.
 * \return Если во время операции возникли ошибки - 0 (ноль), иначе - 1 (единицу).
 */
uint8_t sys_open_named_pipe(sys_named_pipe_tp object, uint8_t flush);

/**
 * \brief Закрыть именованный канал.
 * \ingroup platform_named_pipe
 * \param[in] object Указатель на структуру именованного канала.
 */
void sys_close_named_pipe(sys_named_pipe_tp object);

/**
 * \brief Удалить именованный канал.
 * \ingroup platform_named_pipe
 * \param[in] object Указатель на структуру именованного канала.
 */
void sys_delete_named_pipe(sys_named_pipe_tp object);

/**
 * \brief Открыт ли именованный канал.
 * \ingroup platform_named_pipe
 * \param[in] object Указатель на структуру именованного канала.
 */
uint8_t sys_named_pipe_opened(sys_named_pipe_tp object);

/**
 * \brief Неблокирующая операция чтения из именованного канала.
 * \ingroup platform_named_pipe
 * \param[in] object Указатель на структуру именованного канала.
 * \param[in] data Указатель на буфер для приёма данных.
 * \param[in] size Размер буфера для приёма данных.
 * \return Число прочитанных байт данных.
 */
size_t sys_read_named_pipe(sys_named_pipe_tp object, char* data, size_t size);

typedef size_t(*sys_named_pipe_read_fp)(sys_named_pipe_tp, char*, size_t);

/**
 * \brief Неблокирующая операция записи в именованный канал.
 * \ingroup platform_named_pipe
 * \param[in] object Указатель на структуру именованного канала.
 * \param[in] data Указатель на буфер с данными.
 * \param[in] length Количество байт данных, ожидающих записи.
 * \return Число записанных байт данных.
 */
size_t sys_write_named_pipe(sys_named_pipe_tp object, const char* data, size_t length);

typedef size_t(*sys_named_pipe_write_fp)(sys_named_pipe_tp, const char*, size_t);

#endif // INITPLUS_PLATFORM_NAMED_PIPE_H_INCLUDED
