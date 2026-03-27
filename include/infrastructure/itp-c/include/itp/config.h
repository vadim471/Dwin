#ifndef ITP_CONFIG_H_INCLUDED
#define ITP_CONFIG_H_INCLUDED

/**
 * \file config.h
 * \brief Файл содержит настройки библиотеки.
 *
 * Все макросы можно определить перед включением модулей библиотеки или передать компилятору напрямую (например, gcc -DKEY=VALUE -c *.c).
 */

#include <stdint.h>

#ifndef ITP_REVISION
/**
 * \brief Максимально поддерживаемая версия протокола.
 */
#define ITP_REVISION 1
#endif // ITP_REVISION

#ifndef ITP_MIN_BUFFER_SIZE
/**
 * \brief Начальный размер буфера приёма/передачи данных (в байтах).
 */
#define ITP_MIN_BUFFER_SIZE 0x20
#endif // ITP_MIN_BUFFER_SIZE

#ifndef ITP_MAX_BUFFER_SIZE
/**
 * \brief Максимальный размер буфера приёма/передачи данных (в байтах).
 *
 * Согласно протоколу, максимальный размер пакета - 65548 (0x1000C) байт.
 */
#define ITP_MAX_BUFFER_SIZE 0x400
#endif // ITP_MAX_BUFFER_SIZE

#ifndef ITP_TUNNEL_BUFFER_SIZE
/**
 * \brief Размер буфера туннеля данных (в байтах).
 *
 * У каждого устройства на шине отдельный буфер.
 */
#define ITP_TUNNEL_BUFFER_SIZE 0x80
#endif // ITP_TUNNEL_BUFFER_SIZE

#ifndef ITP_BUS_BUFFER_SIZE
/**
 * \brief Размер буфера шины данных (в байтах).
 *
 * У каждого устройства на шине отдельный буфер.
 */
#define ITP_BUS_BUFFER_SIZE 0x80
#endif // ITP_BUS_BUFFER_SIZE

#ifndef ITP_PAIRED_BUFFER_SIZE
/**
 * \brief Размер буфера удалённых точек передачи данных (в байтах).
 */
#define ITP_PAIRED_BUFFER_SIZE 0x100
#endif // ITP_PAIRED_BUFFER_SIZE

#ifndef ITP_MAX_PENDING_CONNECTIONS
/**
 * \brief Максимальное количество узлов, ожидающих соединения.
 */
#define ITP_MAX_PENDING_CONNECTIONS 20
#endif // ITP_MAX_PENDING_CONNECTIONS

#ifndef ITP_MAX_REQUEST_HANDLERS
/**
 * \brief Максимальное количество обработчиков запросов.
 */
#define ITP_MAX_REQUEST_HANDLERS 100
#endif // ITP_MAX_REQUEST_HANDLERS

#ifndef ITP_MAX_PROXY_HANDLERS
/**
 * \brief Максимальное количество обработчиков перехваченных запросов.
 */
#define ITP_MAX_PROXY_HANDLERS 5
#endif // ITP_MAX_PROXY_HANDLERS

#ifndef ITP_MAX_CONNECTION_LISTENERS
/**
 * \brief Максимальное количество обработчиков соединения.
 */
#define ITP_MAX_CONNECTION_LISTENERS 5
#endif // ITP_MAX_CONNECTION_LISTENERS

#ifndef ITP_MAX_COMMAND_LISTENERS
/**
 * \brief Максимальное количество слушателей команд.
 */
#define ITP_MAX_COMMAND_LISTENERS 50
#endif // ITP_MAX_COMMAND_LISTENERS

#ifndef ITP_MAX_REMOTE_ENDPOINTS
/**
 * \brief Максимальное количество удалённых каналов передачи данных.
 */
#define ITP_MAX_REMOTE_ENDPOINTS 5
#endif // ITP_MAX_REMOTE_ENDPOINTS

#ifndef ITP_MAX_INPUT_PACKAGES
/**
 * \brief Максимальное количество пакетов в очереди приёма (значение по умолчанию).
 *
 * У каждого узла своя собственная динамическая очередь, на которую распространяется это правило.
 */
#define ITP_MAX_INPUT_PACKAGES 200
#endif // ITP_MAX_INPUT_PACKAGES

#ifndef ITP_MAX_OUTPUT_PACKAGES
/**
 * \brief Максимальное количество пакетов в очереди отправки (значение по умолчанию).
 *
 * У каждого узла своя собственная динамическая очередь, на которую распространяется это правило.
 */
#define ITP_MAX_OUTPUT_PACKAGES 200
#endif // ITP_MAX_OUTPUT_PACKAGES

#ifndef ITP_MAX_PENDING_FRAMES
/**
 * \brief Максимальное количество кадров, ожидающих ответа (значение по умолчанию).
 *
 * Очередь кадров принадлежит только корневой структуре.
 */
#define ITP_MAX_PENDING_FRAMES 250
#endif // ITP_MAX_PENDING_FRAMES

#ifndef ITP_WAIT_SENDING
/**
 * \brief Время отправки сообщения (от начала до конца).
 */
#define ITP_WAIT_SENDING 200
#endif // ITP_WAIT_SENDING

#ifndef ITP_WAIT_RECEIVING
/**
 * \brief Время приёма сообщения (от начала до конца).
 */
#define ITP_WAIT_RECEIVING 200
#endif // ITP_WAIT_RECEIVING

#ifndef ITP_MAX_FRAME_ERRORS
/**
 * \brief Максимальное количество ошибок при обработке кадра.
 */
#define ITP_MAX_FRAME_ERRORS 3
#endif // ITP_MAX_FRAME_ERRORS

#ifndef ITP_WAIT_ACKNOWLEDGE
/**
 * \brief Время ожидания подтверждения приёма пакета.
 */
#define ITP_WAIT_ACKNOWLEDGE 500
#endif // ITP_WAIT_ACKNOWLEDGE

#ifndef ITP_WAIT_BUSY
/**
 * \brief Время ожидания перед повторной отправкой запроса занятому устройству.
 */
#define ITP_WAIT_BUSY 800
#endif // ITP_WAIT_BUSY

#ifndef ITP_WAIT_DATA
/**
 * \brief Время ожидания пакета с данными.
 */
#define ITP_WAIT_DATA 3000
#endif // ITP_WAIT_DATA

#ifndef ITP_WAIT_CONTROL
/**
 * \brief Время ожидания ответа на внутренние команды.
 */
#define ITP_WAIT_CONTROL 1000
#endif // ITP_WAIT_CONTROL

#ifndef ITP_WAIT_PROXY
/**
 * \brief Время ожидания ответа на перехваченные команды.
 */
#define ITP_WAIT_PROXY 1000
#endif // ITP_WAIT_PROXY

#ifndef NULL
/**
 * \brief Нулевой указатель.
 */
#define NULL ((void*)0)
#endif // NULL

/**
 * \brief Тип, используемый для представления времени.
 */
typedef uint64_t itp_time_t;

/**
 * \brief Тип, используемый для представления байт данных.
 */
typedef uint8_t itp_byte_t;

/**
 * \brief Тип, используемый для представления размера буфера.
 */
typedef uint16_t itp_size_t;

#ifdef ITP_ENABLE_ALL_CHECKS

#ifndef ITP_ENABLE_NULLPTR_CHECKS
#define ITP_ENABLE_NULLPTR_CHECKS
#endif // ITP_ENABLE_NULLPTR_CHECKS

#ifndef ITP_ENABLE_INTERNAL_CHECKS
#define ITP_ENABLE_INTERNAL_CHECKS
#endif // ITP_ENABLE_INTERNAL_CHECKS

#ifndef ITP_ENABLE_BOUNDARY_CHECKS
#define ITP_ENABLE_BOUNDARY_CHECKS
#endif // ITP_ENABLE_BOUNDARY_CHECKS

#endif // ITP_ENABLE_ALL_CHECKS

#endif // ITP_CONFIG_H_INCLUDED
