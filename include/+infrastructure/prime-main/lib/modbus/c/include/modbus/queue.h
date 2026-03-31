#ifndef MODBUS_QUEUE_H_INCLUDED
#define MODBUS_QUEUE_H_INCLUDED

/**
 * \file queue.h
 * \brief Файл содержит функции для работы со списком запросов.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "callback.h"

/**
 * \brief Структура запроса.
 */
struct mbs_request {
    /**
     * \brief Флаг начала выполнения задачи.
     */
    uint8_t in_progress;
    /**
     * \brief Указатель на выполняемую задачу.
     */
    mbs_task_tp task;
    /**
     * \brief Количество ошибок при обработке запроса.
     *
     * При достижении максимально допустимого значения <code>\link MODBUS_MAX_REQUEST_ERRORS \endlink</code> запрос удаляется из очереди, а задача отключается.
     */
    uint8_t errors;
    /**
     * \brief Указатель на следующий запрос в списке.
     */
    mbs_request_tp next;
};

/**
 * \brief Структура списка запросов.
 */
struct mbs_requests_queue {
    /**
     * \brief Указатель на первый запрос в списке.
     */
    mbs_request_tp first;
};

/**
 * \brief Тип структуры списка запросов.
 */
typedef struct mbs_requests_queue mbs_requests_queue_t;

/**
 * \brief Указатель на структуру списка запросов.
 */
typedef mbs_requests_queue_t* mbs_requests_queue_tp;

/**
 * \brief Инициализация запроса.
 * \ingroup private_functions
 * \param[in] request Указатель на запрос \c mbs_request.
 */
void mbs_init_request(mbs_request_tp request);

/**
 * \brief Создание нового запроса.
 * \ingroup private_functions
 * \param[in] task Указатель на задачу \c mbs_task.
 * \return Указатель на созданный запрос или \c NULL.
 */
mbs_request_tp mbs_create_request(mbs_task_tp task);

/**
 * \brief Освобождение памяти, занятой структурой запроса.
 * \ingroup private_functions
 * \param[in] request Указатель на запрос \c mbs_request.
 */
void mbs_free_request(mbs_request_tp request);

/**
 * \brief Инициализация списка запросов.
 * \ingroup private_functions
 * \param[in] queue Указатель на список запросов \c mbs_requests_queue.
 */
void mbs_init_requests_queue(mbs_requests_queue_tp queue);

/**
 * \brief Добавление запроса в очередь, согласно приоритету.
 * \ingroup private_functions
 * \param[in] queue Указатель на список запросов \c mbs_requests_queue.
 * \param[in] task Указатель на задачу \c mbs_task.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_push_request(mbs_requests_queue_tp queue, mbs_task_tp task);

/**
 * \brief Удаление запроса из начала очереди.
 *
 * Память, занятая структурой, не освобождается.
 * \ingroup private_functions
 * \param[in] queue Указатель на очередь запросов \c mbs_requests_queue.
 * \return Указатель на удалённый из очереди запрос.
 */
mbs_request_tp mbs_pop_request(mbs_requests_queue_tp queue);

/**
 * \brief Удаление указанного запроса из очереди.
 * \ingroup private_functions
 * \param[in] queue Указатель на список запросов \c mbs_requests_queue.
 * \param[in] task Указатель на задачу \c mbs_task.
 * \return Указатель на следующий запрос в списке, если он есть.
 */
mbs_request_tp mbs_erase_request(mbs_requests_queue_tp queue, mbs_task_tp task);

/**
 * \brief Количество запросов в очереди.
 * \ingroup private_functions
 * \param[in] queue Указатель на очередь запросов \c mbs_requests_queue.
 * \return Количество запросов в очереди.
 */
mbs_size_t mbs_requests_queue_size(mbs_requests_queue_tp queue);

/**
 * \brief Проверка существования запроса в очереди.
 * \ingroup private_functions
 * \param[in] queue Указатель на очередь запросов \c mbs_requests_queue.
 * \param[in] task Указатель на задачу \c mbs_task.
 * \return Если запрос найден - 1 (единицу), если нет - 0 (ноль).
 */
uint8_t mbs_is_request_exist(mbs_requests_queue_tp queue, mbs_task_tp task);

/**
 * \brief Проверка очереди запросов на переполнение.
 * \ingroup private_functions
 * \param[in] queue Указатель на очередь \c mbs_requests_queue.
 * \return Если очередь полна - 1 (единицу), если нет - 0 (ноль).
 */
uint8_t mbs_is_requests_queue_overflow(mbs_requests_queue_tp queue);

/**
 * \brief Проверка списка запросов на пустоту.
 * \ingroup private_functions
 * \param[in] queue Указатель на список запросов \c mbs_requests_queue.
 * \return Если список пуст - 0 (ноль), если нет - 1 (единицу).
 */
uint8_t mbs_is_requests_queue_empty(mbs_requests_queue_tp queue);

/**
 * \brief Очистка списка запросов.
 * \ingroup private_functions
 * \param[in] queue Указатель на список запросов \c mbs_requests_queue.
 */
void mbs_clear_requests_queue(mbs_requests_queue_tp queue);

#endif // MODBUS_LIST_H_INCLUDED
