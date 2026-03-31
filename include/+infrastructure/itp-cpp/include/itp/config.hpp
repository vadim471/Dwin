#ifndef ITP_CONFIG_HPP_INCLUDED
#define ITP_CONFIG_HPP_INCLUDED

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
#define ITP_MAX_PROXY_HANDLERS 15
#endif // ITP_MAX_PROXY_HANDLERS

#ifndef ITP_MAX_INPUT_PACKAGES
/**
 * \brief Максимальное количество пакетов в очереди приёма (значение по умолчанию).
 *
 * У каждого узла своя собственная динамическая очередь, на которую распространяется это правило.
 */
#define ITP_MAX_INPUT_PACKAGES 30
#endif // ITP_MAX_INPUT_PACKAGES

#ifndef ITP_MAX_OUTPUT_PACKAGES
/**
 * \brief Максимальное количество пакетов в очереди отправки (значение по умолчанию).
 *
 * У каждого узла своя собственная динамическая очередь, на которую распространяется это правило.
 */
#define ITP_MAX_OUTPUT_PACKAGES 30
#endif // ITP_MAX_OUTPUT_PACKAGES

#ifndef ITP_MAX_PENDING_FRAMES
/**
 * \brief Максимальное количество кадров, ожидающих ответа (значение по умолчанию).
 *
 * Очередь кадров принадлежит только корневой структуре.
 */
#define ITP_MAX_PENDING_FRAMES 50
#endif // ITP_MAX_PENDING_FRAMES

extern "C" {

#include <itp/config.h>

}

#endif // ITP_CONFIG_HPP_INCLUDED
