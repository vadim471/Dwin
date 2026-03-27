#ifndef ITP_NODE_H_INCLUDED
#define ITP_NODE_H_INCLUDED

/**
 * \file node.h
 * \brief Файл содержит функции для работы с узлом сети.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "callback.h"
#include "transmit.h"
#include "queue.h"
#include "array.h"

/**
 * \brief Структура узла сети.
 */
struct itp_node {
    /**
     * \brief Виртуальный адрес узла.
     */
    uint8_t address;
    /**
     * \brief Список адресов дочерних узлов.
     *
     * Используется для поиска адресов при каскадном подключении.
     * В этот массив не включаются адреса узлов, подключённых напрямую.
     */
    itp_address_array_t subnodes;
    /**
     * \brief Буфер для принимаемых данных.
     */
    itp_buffer_t input;
    /**
     * \brief Очередь принятых пакетов.
     */
    itp_package_queue_t queue_in;
    /**
     * \brief Буфер для отправляемых данных.
     */
    itp_buffer_t output;
    /**
     * \brief Очередь отправляемых пакетов.
     */
    itp_package_queue_t queue_out;
    /**
     * \brief Указатель на интерфейс передачи данных на этот узел.
     */
    itp_endpoint_tp endpoint;
    /**
     * \brief Обработчик ошибок узла сети.
     */
    itp_on_error_fp on_error;
    #ifdef ITP_EXTEND_CALLBACKS
    size_t on_error_eparam;
    #endif // ITP_EXTEND_CALLBACKS
};

/**
 * \brief Инициализация структуры узла сети.
 * \ingroup private_functions
 * \param[in] node Указатель на структуру узла сети \c itp_node.
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c itp_endpoint.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_init_node(itp_node_tp node, itp_endpoint_tp endpoint);

/**
 * \brief Проверка наличия адреса в подсети.
 * \ingroup private_functions
 * \param[in] node Указатель на структуру узла сети \c itp_node.
 * \param[in] address Искомый виртуальный адрес.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_resolve_address(itp_node_tp node, uint8_t address);

/**
 * \brief Обновить состояние узла сети.
 * \ingroup private_functions
 * \param[in] node Указатель на структуру узла сети \c itp_node.
 * \param[in] time Текущее время в системе (учитывается только дельта времени).
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_poll_node(
#ifdef ITP_TRACE_BUFFERS
uint8_t root_address,
#endif // ITP_TRACE_BUFFERS
itp_node_tp node, itp_time_t time);

/**
 * \brief Освобождение памяти, занятой структурой узла сети.
 * \ingroup private_functions
 * \param[in] node Указатель на структуру узла сети \c itp_node.
 */
void itp_free_node(itp_node_tp node);

#endif // ITP_NODE_H_INCLUDED
