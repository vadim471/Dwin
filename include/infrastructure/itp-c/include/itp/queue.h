#ifndef ITP_QUEUE_H_INCLUDED
#define ITP_QUEUE_H_INCLUDED

/**
 * \file queue.h
 * \brief Файл содержит функции для работы с очередью пакетов.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"

/**
 * \brief Структура очереди пакетов.
 */
struct itp_package_queue {
    /**
     * \brief Указатель на первый пакет \c itp_package в очереди.
     */
    itp_package_tp first;
    /**
     * \brief Максимальное количество пакетов в очереди.
     */
    itp_size_t max;
};

/**
 * \brief Тип структуры очереди пакетов <code>\link itp_package_queue \endlink</code>.
 */
typedef struct itp_package_queue itp_package_queue_t;

/**
 * \brief Указатель на структуру очереди пакетов <code>\link itp_package_queue \endlink</code>.
 */
typedef itp_package_queue_t* itp_package_queue_tp;

/**
 * \brief Инициализация очереди пакетов.
 * \ingroup private_functions
 * \param[in] queue Указатель на очередь пакетов \c itp_package_queue.
 * \param[in] max Максимальное количество пакетов в очереди.
 */
void itp_init_package_queue(itp_package_queue_tp queue, itp_size_t max);

/**
 * \brief Добавление пакета в очередь (в конец).
 * \ingroup private_functions
 * \param[in] queue Указатель на очередь пакетов \c itp_package_queue.
 * \param[in] package Указатель на структуру пакета \c itp_package.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_push_package(itp_package_queue_tp queue, itp_package_tp package);

/**
 * \brief Удаление пакета из очереди (из начала).
 *
 * Память, занятая структурой, не освобождается.
 * \ingroup private_functions
 * \param[in] queue Указатель на очередь пакетов \c itp_package_queue.
 * \return Указатель на удалённый из очереди пакет.
 */
itp_package_tp itp_pop_package(itp_package_queue_tp queue);

/**
 * \brief Копирование данных из первого пакета в буфер и удаление его из очереди.
 * \ingroup private_functions
 * \param[in] queue Указатель на очередь пакетов \c itp_package_queue.
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c itp_endpoint.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_bufferize_package(itp_package_queue_tp queue, itp_buffer_tp buffer, itp_endpoint_tp endpoint);

/**
 * \brief Текущий размер очереди пакетов.
 * \ingroup private_functions
 * \param[in] queue Указатель на очередь \c itp_package_queue.
 * \return Количество пакетов в очереди.
 */
uint8_t itp_package_queue_size(itp_package_queue_tp queue);

/**
 * \brief Проверка очереди пакетов на пустоту.
 * \ingroup private_functions
 * \param[in] queue Указатель на очередь \c itp_package_queue.
 * \return Если очередь пуста - 1 (единицу), если нет - 0 (ноль).
 */
uint8_t itp_package_queue_is_empty(itp_package_queue_tp queue);

/**
 * \brief Очистка очереди пакетов.
 * \ingroup private_functions
 * \param[in] queue Указатель на очередь \c itp_package_queue.
 */
void itp_clear_package_queue(itp_package_queue_tp queue);

#endif // ITP_QUEUE_H_INCLUDED
