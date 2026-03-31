#ifndef MODBUS_LIST_H_INCLUDED
#define MODBUS_LIST_H_INCLUDED

/**
 * \file list.h
 * \brief Файл содержит функции для работы со списком задач.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "callback.h"

/**
 * \brief Перечисление состояний задачи.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Задача больше не может быть добавлен в очередь.
     */
    MBS_RS_IDLE = 0,
    /**
     * \brief Задача готова к исполнению.
     */
    MBS_RS_READY,
    /**
     * \brief Задача ещё не готова к исполнению.
     */
    MBS_RS_PENDING,
    /**
     * \brief Задача не может быть исполнена из-за ошибки.
     */
    MBS_RS_ERROR
} mbs_repeat_status_t;

/**
 * \brief Тип функции проверки состояния задачи.
 * \ingroup extentions
 * \param[in] object Указатель на объект, ассоциированный с задачей.
 * \param[in] time Текущее время в системе.
 * \param[in] error Указатель на переменную, куда будет записан код ошибки, в случае её возникновения.
 * \return Результат выполнения операции <code>\link mbs_repeat_status_t \endlink</code>.
 */
typedef mbs_repeat_status_t(*mbs_repeat_check_fp)(
    void* object,
    mbs_time_t time,
    mbs_error_code_t* error
);

/**
 * \brief Тип функции сброса состояния задачи.
 * \ingroup extentions
 * \param[in] object Указатель на объект, ассоциированный с задачей.
 * \param[in] time Текущее время в системе.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
typedef mbs_error_code_t(*mbs_repeat_reset_fp)(
    void* object,
    mbs_time_t time
);

/**
 * \brief Структура задачи.
 */
struct mbs_task {
    /**
     * \brief Команда.
     */
    uint8_t command;
    /**
     * \brief Приоритет.
     */
    uint8_t priority;
    /**
     * \brief Флаг активности задачи.
     */
    uint8_t enabled;
    /**
     * \brief Устройство, владеющее задачей.
     */
    mbs_device_tp device;
    /**
     * \brief Адрес первого регистра.
     */
    uint16_t address;
    /**
     * \brief Количество регистров.
     */
    uint16_t count;
    /**
     * \brief Функция проверки состояния задачи.
     */
    mbs_repeat_check_fp repeat_check;
    /**
     * \brief Функция сброса состояния задачи.
     */
    mbs_repeat_reset_fp repeat_reset;
    /**
     * \brief Объект, ассоциированный с функцией проверки состояния.
     */
    void* repeat_object;
    /**
     * \brief Функция построения тела запроса.
     *
     * При вызове тип будет приведён исходя из команды.
     */
    void* on_request;
    /**
     * \brief Функция обработки ответа на запрос.
     *
     * При вызове тип будет приведён исходя из команды.
     */
    void* on_response;
    /**
     * \brief Указатель на следующую задачу в списке.
     */
    mbs_task_tp next;
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam;
    size_t on_response_eparam;
    #endif // MODBUS_EXTEND_CALLBACKS
};

/**
 * \brief Структура списка задач.
 */
struct mbs_tasks_list {
    /**
     * \brief Указатель на первую задачу в списке.
     */
    mbs_task_tp first;
};

/**
 * \brief Тип структуры списка задач.
 */
typedef struct mbs_tasks_list mbs_tasks_list_t;

/**
 * \brief Указатель на структуру списка задач.
 */
typedef mbs_tasks_list_t* mbs_tasks_list_tp;

/**
 * \brief Инициализация задачи \c mbs_task.
 * \ingroup private_functions
 * \param[in] request Указатель на задачу \c mbs_task.
 */
void mbs_init_task(mbs_task_tp task);

/**
 * \brief Освобождение памяти, занятой структурой задачи \c mbs_task.
 * \ingroup private_functions
 * \param[in] request Указатель на задачу \c mbs_task.
 */
void mbs_free_task(mbs_task_tp task);

/**
 * \brief Инициализация списка задач \c mbs_tasks_list.
 * \ingroup private_functions
 * \param[in] list Указатель на список задач \c mbs_tasks_list.
 */
void mbs_init_tasks_list(mbs_tasks_list_tp list);

/**
 * \brief Добавление задачи в конец списка.
 * \ingroup private_functions
 * \param[in] list Указатель на список задач \c mbs_tasks_list.
 * \param[in] task Указатель на задачу \c mbs_task.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_push_task(mbs_tasks_list_tp list, mbs_task_tp task);

/**
 * \brief Удаление указанной задачи из списка.
 * \ingroup private_functions
 * \param[in] list Указатель на список задач \c mbs_tasks_list.
 * \param[in] task Указатель на задачу \c mbs_task.
 * \return Указатель на следующую задачу в списке, если она есть.
 */
mbs_task_tp mbs_erase_task(mbs_tasks_list_tp list, mbs_task_tp task);

/**
 * \brief Проверка списка задач на пустоту.
 * \ingroup private_functions
 * \param[in] list Указатель на список задач \c mbs_tasks_list.
 * \return Если список пуст - 0 (ноль), если нет - 1 (единицу).
 */
uint8_t mbs_tasks_list_is_empty(mbs_tasks_list_tp list);

/**
 * \brief Проверка списка задач на пустоту с условием.
 * \ingroup private_functions
 * \param[in] list Указатель на список задач \c mbs_tasks_list.
 * \return Если список пуст - 0 (ноль), если нет - 1 (единицу).
 */
uint8_t mbs_tasks_list_is_empty_conditional(
    mbs_tasks_list_tp list,
    mbs_task_condition_fp condition
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t condition_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Очистка списка задач.
 * \ingroup private_functions
 * \param[in] list Указатель на список задач \c mbs_tasks_list.
 */
void mbs_clear_tasks_list(mbs_tasks_list_tp list);

#endif // MODBUS_LIST_H_INCLUDED
