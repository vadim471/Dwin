#ifndef MODBUS_CONFIG_H_INCLUDED
#define MODBUS_CONFIG_H_INCLUDED

/**
 * \file config.h
 * \brief Файл содержит настройки библиотеки.
 *
 * Все макросы можно определить перед включением модулей библиотеки или передать компилятору напрямую (например, gcc -DKEY=VALUE -c *.c).
 */

#include <stdint.h>

#ifndef MODBUS_BUFFER_SIZE
/**
 * \brief Размер буфера приёма/передачи данных (в байтах).
 */
#define MODBUS_BUFFER_SIZE 0xFF
#endif // MODBUS_BUFFER_SIZE

#ifndef MODBUS_MAX_QUEUE_SIZE
/**
 * \brief Максимальное количество запросов в очереди отправки.
 */
#define MODBUS_MAX_QUEUE_SIZE 20
#endif // MODBUS_MAX_QUEUE_SIZE

#ifndef MODBUS_MAX_TASKS_COUNT
/**
 * \brief Максимальное количество задач в списке устройства.
 */
#define MODBUS_MAX_TASKS_COUNT 20
#endif // MODBUS_MAX_TASKS_COUNT

#ifndef MODBUS_WAIT_SWITCHING
/**
 * \brief Задержка перед отправкой после принятия команды.
 */
#define MODBUS_WAIT_SWITCHING 0
#endif // MODBUS_WAIT_SWITCHING

#ifndef MODBUS_WAIT_RESPONSE
/**
 * \brief Время ожидания ответа на запрос.
 */
#define MODBUS_WAIT_RESPONSE 200
#endif // MODBUS_WAIT_RESPONSE

#ifndef MODBUS_WAIT_RECEIVING
/**
 * \brief Время приёма сообщения (от начала до конца).
 */
#define MODBUS_WAIT_RECEIVING 200
#endif // MODBUS_WAIT_RECEIVING

#ifndef MODBUS_WAIT_SENDING
/**
 * \brief Время отправки сообщения (от начала до конца).
 */
#define MODBUS_WAIT_SENDING 200
#endif // MODBUS_WAIT_SENDING

#ifndef MODBUS_MAX_REQUEST_ERRORS
/**
 * \brief Максимальное количество ошибок при обработке запроса.
 */
#define MODBUS_MAX_REQUEST_ERRORS 3
#endif // MODBUS_MAX_REQUEST_ERRORS

#ifndef NULL
/**
 * \brief Нулевой указатель.
 */
#define NULL ((void*)0)
#endif // NULL

/**
 * \brief Тип, используемый для представления времени.
 */
typedef uint64_t mbs_time_t;

/**
 * \brief Тип, используемый для представления байт данных.
 */
typedef uint8_t mbs_byte_t;

/**
 * \brief Тип, используемый для представления размера буфера.
 */
typedef uint16_t mbs_size_t;

#ifdef MODBUS_ENABLE_ALL_CHECKS

#ifndef MODBUS_ENABLE_NULLPTR_CHECKS
#define MODBUS_ENABLE_NULLPTR_CHECKS
#endif // MODBUS_ENABLE_NULLPTR_CHECKS

#ifndef MODBUS_ENABLE_INTERNAL_CHECKS
#define MODBUS_ENABLE_INTERNAL_CHECKS
#endif // MODBUS_ENABLE_INTERNAL_CHECKS

#ifndef MODBUS_ENABLE_BOUNDARY_CHECKS
#define MODBUS_ENABLE_BOUNDARY_CHECKS
#endif // MODBUS_ENABLE_BOUNDARY_CHECKS

#endif // MODBUS_ENABLE_ALL_CHECKS

#endif // MODBUS_CONFIG_H_INCLUDED
