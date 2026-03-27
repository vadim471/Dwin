#ifndef ITP_LIST_H_INCLUDED
#define ITP_LIST_H_INCLUDED

/**
 * \file list.h
 * \brief Файл содержит функции для работы со списками узлов и кадров.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "callback.h"
#include "handler.h"

struct itp_pending_node;

/**
 * \brief Тип структуры узла, ожидающего соединения.
 */
typedef struct itp_pending_node itp_pending_node_t;

/**
 * \brief Указатель на структуру узла, ожидающего соединения.
 */
typedef itp_pending_node_t* itp_pending_node_tp;

/**
 * \brief Структура узла, ожидающего соединения.
 */
struct itp_pending_node {
    /**
     * \brief Указатель на структуру узла \c itp_node.
     */
    itp_node_tp node;
    /**
     * \brief Указатель на следующий узел в списке.
     */
    itp_pending_node_tp next;
};

/**
 * \brief Структура списка узлов, ожидающих соединения.
 */
struct itp_pending_list {
    /**
     * \brief Указатель на первый узел в списке.
     */
    itp_pending_node_tp first;
};

/**
 * \brief Тип структуры списка узлов, ожидающих соединения.
 */
typedef struct itp_pending_list itp_pending_list_t;

/**
 * \brief Указатель на структуру списка узлов, ожидающих соединения.
 */
typedef itp_pending_list_t* itp_pending_list_tp;

/**
 * \brief Инициализация узла \c itp_pending_node.
 * \ingroup private_functions
 * \param[in] pending Указатель на узел \c itp_pending_node.
 * \param[in] node Указатель на узел \c itp_node.
 */
void itp_init_pending_node(itp_pending_node_tp pending, itp_node_tp node);

/**
 * \brief Инициализация списка узлов \c itp_pending_list.
 * \ingroup private_functions
 * \param[in] list Указатель на список узлов \c itp_pending_list.
 */
void itp_init_pending_list(itp_pending_list_tp list);

/**
 * \brief Добавление узла в конец списка.
 * \ingroup private_functions
 * \param[in] list Указатель на список \c itp_pending_list.
 * \param[in] node Указатель на добавляемый узел \c itp_node.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_push_pending_node(itp_pending_list_tp list, itp_node_tp node);

/**
 * \brief Удаление указанного узла из списка.
 *
 * Память, занятая структурой узла не освобождается.
 * \ingroup private_functions
 * \param[in] list Указатель на список узлов \c itp_pending_list.
 * \param[in] node Указатель на удаляемый узел \c itp_node.
 * \return Указатель на следующий узел в списке, если он есть.
 */
itp_pending_node_tp itp_erase_pending_node(itp_pending_list_tp list, itp_node_tp node);

/**
 * \brief Проверка существования указанного узла.
 * \ingroup private_functions
 * \param[in] list Указатель на список узлов \c itp_pending_list.
 * \param[in] node Указатель на структуру узла сети \c itp_node.
 * \return 1 - узел есть в списке, 0 - нет.
 */
uint8_t itp_pending_node_is_exist(itp_pending_list_tp list, itp_node_tp node);

/**
 * \brief Проверка списка узлов на пустоту.
 * \ingroup private_functions
 * \param[in] list Указатель на список \c itp_pending_list.
 * \return Если список пуст - 0 (ноль), если нет - 1 (единицу).
 */
uint8_t itp_pending_list_is_empty(itp_pending_list_tp list);

/**
 * \brief Очистка списка узлов.
 * \ingroup private_functions
 * \param[in] list Указатель на список \c itp_pending_list.
 */
void itp_clear_pending_list(itp_pending_list_tp list);

struct itp_frame_item;

/**
 * \brief Тип структуры кадра, ожидающего обработки.
 */
typedef struct itp_frame_item itp_frame_item_t;

/**
 * \brief Указатель на структуру кадра, ожидающего обработки.
 */
typedef itp_frame_item_t* itp_frame_item_tp;

/**
 * \brief Структура кадра, ожидающего обработки.
 */
struct itp_frame_item {
    /**
     * \brief Указатель на структуру кадра \c itp_frame.
     */
    itp_frame_tp frame;
    /**
     * \brief Время приёма/отправки пакетов.
     *
     * Поле используется для вычисления таймаутов.
     */
    itp_time_t time;
    /**
     * \brief Адрес отправителя.
     */
    uint8_t sender;
    /**
     * \brief Количество ошибок при обработке кадра.
     *
     * При достижении максимально допустимого значения <code>\link ITP_MAX_FRAME_ERRORS \endlink</code> кадр удаляется из очереди.
     */
    uint8_t errors;
    /**
     * \brief Было получено подтверждение на запрос.
     */
    uint8_t acknowledged;
    /**
     * \brief Запрос был отправлен, ожидается ответ.
     */
    uint8_t processed;
    /**
     * \brief После получения подтверждения ожидаются данные.
     */
    uint8_t wait_data;
    /**
     * \brief Обработчик события.
     */
    itp_handler_wrapper_t handler;
    /**
     * \brief Указатель на следующий кадр в списке.
     */
    itp_frame_item_tp next;
};

/**
 * \brief Структура списка кадров.
 */
struct itp_frame_list {
    /**
     * \brief Указатель на первый кадр \c itp_frame_item в списке.
     */
    itp_frame_item_tp first;
    /**
     * \brief Максимальное количество кадров в списке.
     */
    itp_size_t max;
};

/**
 * \brief Тип структуры списка кадров <code>\link itp_frame_list \endlink</code>.
 */
typedef struct itp_frame_list itp_frame_list_t;

/**
 * \brief Указатель на структуру списка кадров <code>\link itp_frame_list \endlink</code>.
 */
typedef itp_frame_list_t* itp_frame_list_tp;

/**
 * \brief Инициализация кадра \c itp_frame_item.
 * \ingroup private_functions
 * \param[in] item Указатель на кадр \c itp_frame_item.
 * \param[in] frame Указатель на кадр \c itp_frame.
 */
void itp_init_frame_item(itp_frame_item_tp item, itp_frame_tp frame);

/**
 * \brief Удаление кадра \c itp_frame_item.
 * \ingroup private_functions
 * \param[in] item Указатель на кадр \c itp_frame_item.
 */
void itp_free_frame_item(itp_frame_item_tp item);

/**
 * \brief Инициализация списка кадров.
 * \ingroup private_functions
 * \param[in] list Указатель на список кадров \c itp_frame_list.
 * \param[in] max Максимальное количество кадров в списке.
 */
void itp_init_frame_list(itp_frame_list_tp list, itp_size_t max);

/**
 * \brief Добавление кадра в список.
 * \ingroup private_functions
 * \param[in] list Указатель на список кадров \c itp_frame_list.
 * \param[in] item Указатель на структуру кадра \c itp_frame_item.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_push_frame_item(itp_frame_list_tp list, itp_frame_item_tp item);

/**
 * \brief Удаление указанного кадра из списка.
 *
 * Память, занятая всеми структурами, также освобождается.
 * \ingroup private_functions
 * \param[in] list Указатель на список кадров \c itp_frame_list.
 * \param[in] frame Указатель на удаляемый кадр \c itp_frame.
 * \return Указатель на следующий кадр в списке, если он есть.
 */
itp_frame_item_tp itp_erase_frame_item(itp_frame_list_tp list, itp_frame_tp frame);

/**
 * \brief Проверка списка кадров на пустоту.
 * \ingroup private_functions
 * \param[in] list Указатель на список кадров \c itp_frame_list.
 * \return Если список пуст - 0 (ноль), если нет - 1 (единицу).
 */
uint8_t itp_frame_list_is_empty(itp_frame_list_tp list);

/**
 * \brief Очистка списка кадров.
 * \ingroup private_functions
 * \param[in] list Указатель на список кадров \c itp_frame_list.
 */
void itp_clear_frame_list(itp_frame_list_tp list);

struct itp_handler_item;

/**
 * \brief Тип структуры обработчика запросов.
 */
typedef struct itp_handler_item itp_handler_item_t;

/**
 * \brief Указатель на структуру обработчика запросов.
 */
typedef itp_handler_item_t* itp_handler_item_tp;

/**
 * \brief Структура обработчика запросов.
 */
struct itp_handler_item {
    /**
     * \brief Адрес отправителя команды.
     */
    uint8_t address;
    /**
     * \brief Команда, ассоциированная с обработчиком.
     */
    uint16_t command;
    /**
     * \brief Указатель на обработчик запроса <code>\link itp_on_request_fp \endlink</code>.
     */
    itp_on_request_fp on_request;
    #ifdef ITP_EXTEND_CALLBACKS
    size_t on_request_eparam;
    #endif // ITP_EXTEND_CALLBACKS
    /**
     * \brief Указатель на следующий обработчик в списке.
     */
    itp_handler_item_tp next;
};

/**
 * \brief Структура списка обработчиков запросов.
 */
struct itp_handler_list {
    /**
     * \brief Указатель на первый обработчик в списке.
     */
    itp_handler_item_tp first;
};

/**
 * \brief Тип структуры списка обработчиков запросов.
 */
typedef struct itp_handler_list itp_handler_list_t;

/**
 * \brief Указатель на структуру списка обработчиков запросов.
 */
typedef itp_handler_list_t* itp_handler_list_tp;

/**
 * \brief Инициализация обработчика запроса \c itp_handler_item.
 * \ingroup private_functions
 * \param[in] handler Указатель на обработчик запроса \c itp_handler_item.
 * \param[in] address Адрес отправителя команды.
 * \param[in] command Команда, ассоциированная с обработчиком.
 * \param[in] on_request Указатель на функцию обработки запроса <code>\link itp_on_request_fp \endlink</code>.
 */
void itp_init_handler_item(itp_handler_item_tp handler, uint8_t address, uint16_t command, itp_on_request_fp on_request
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Освобождение памяти, занятой структурой обработчика запроса \c itp_handler_item.
 * \ingroup private_functions
 * \param[in] handler Указатель на обработчик запроса \c itp_handler_item.
 */
void itp_free_handler_item(itp_handler_item_tp handler);

/**
 * \brief Инициализация списка обработчиков \c itp_handler_list.
 * \ingroup private_functions
 * \param[in] list Указатель на список обработчиков \c itp_handler_list.
 */
void itp_init_handler_list(itp_handler_list_tp list);

/**
 * \brief Добавление обработчика в конец списка.
 * \ingroup private_functions
 * \param[in] list Указатель на список \c itp_handler_list.
 * \param[in] address Адрес отправителя команды (или ноль).
 * \param[in] command Команда, ассоциированная с обработчиком.
 * \param[in] on_request Указатель на функцию обработки запроса <code>\link itp_on_request_fp \endlink</code>.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_push_handler_item(itp_handler_list_tp list, uint8_t address, uint16_t command, itp_on_request_fp on_request
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Удаление указанного обработчика из списка.
 *
 * Память, занятая всеми структурами, также освобождается.
 * \ingroup private_functions
 * \param[in] list Указатель на список обработчиков \c itp_handler_list.
 * \param[in] address Адрес отправителя команды (или ноль).
 * \param[in] command Команда, ассоциированная с обработчиком.
 * \return Указатель на следующий узел в списке, если он есть.
 */
itp_handler_item_tp itp_erase_handler_item(itp_handler_list_tp list, uint8_t address, uint16_t command);

/**
 * \brief Поиск обработчика по команде.
 * \ingroup private_functions
 * \param[in] list Указатель на список обработчиков \c itp_handler_list.
 * \param[in] address Адрес отправителя команды (или ноль).
 * \param[in] command Команда, ассоциированная с обработчиком.
 * \return Указатель на обработчик, если он найден.
 */
itp_handler_item_tp itp_find_handler(itp_handler_list_tp list, uint8_t address, uint16_t command);

/**
 * \brief Проверка списка обработчиков на пустоту.
 * \ingroup private_functions
 * \param[in] list Указатель на список обработчиков \c itp_handler_list.
 * \return Если список пуст - 0 (ноль), если нет - 1 (единицу).
 */
uint8_t itp_handler_list_is_empty(itp_handler_list_tp list);

/**
 * \brief Очистка списка обработчиков.
 * \ingroup private_functions
 * \param[in] list Указатель на список обработчиков \c itp_handler_list.
 */
void itp_clear_handler_list(itp_handler_list_tp list);

struct itp_proxy_item;

/**
 * \brief Тип структуры обработчика перехваченных запросов.
 */
typedef struct itp_proxy_item itp_proxy_item_t;

/**
 * \brief Указатель на структуру обработчика перехваченных запросов.
 */
typedef itp_proxy_item_t* itp_proxy_item_tp;

/**
 * \brief Структура обработчика перехваченных запросов.
 */
struct itp_proxy_item {
    /**
     * \brief Время добавления запроса.
     */
    itp_time_t time;
    /**
     * \brief Адрес отправителя.
     */
    uint8_t address;
    /**
     * \brief Команда, ассоциированная с обработчиком.
     */
    uint16_t command;
    /**
     * \brief Номер пакета.
     */
    uint16_t order;
    /**
     * \brief Обработчик события.
     */
    itp_handler_wrapper_t handler;
    /**
     * \brief Указатель на следующий обработчик в списке.
     */
    itp_proxy_item_tp next;
};

/**
 * \brief Структура списка обработчиков перехваченных запросов.
 */
struct itp_proxy_list {
    /**
     * \brief Указатель на первый обработчик в списке.
     */
    itp_proxy_item_tp first;
};

/**
 * \brief Тип структуры списка обработчиков перехваченных запросов.
 */
typedef struct itp_proxy_list itp_proxy_list_t;

/**
 * \brief Указатель на структуру списка обработчиков перехваченных запросов.
 */
typedef itp_proxy_list_t* itp_proxy_list_tp;

/**
 * \brief Инициализация обработчика перехваченного запроса \c itp_proxy_item.
 * \ingroup private_functions
 * \param[in] item Указатель на обработчик запроса \c itp_proxy_item.
 * \param[in] time Текущее время в системе.
 * \param[in] handler Указатель на составной обработчик \c itp_handler.
 */
void itp_init_proxy_item(itp_proxy_item_tp item, itp_time_t time, itp_handler_tp handler);

/**
 * \brief Освобождение памяти, занятой структурой обработчика перехваченного запроса \c itp_proxy_item.
 * \ingroup private_functions
 * \param[in] item Указатель на обработчик запроса \c itp_proxy_item.
 */
void itp_free_proxy_item(itp_proxy_item_tp item);

/**
 * \brief Инициализация списка обработчиков \c itp_proxy_list.
 * \ingroup private_functions
 * \param[in] list Указатель на список обработчиков \c itp_proxy_list.
 */
void itp_init_proxy_list(itp_proxy_list_tp list);

/**
 * \brief Добавление обработчика в конец списка.
 * \ingroup private_functions
 * \param[in] list Указатель на список \c itp_proxy_list.
 * \param[in] time Текущее время в системе.
 * \param[in] address Адрес отправителя.
 * \param[in] command Команда, ассоциированная с обработчиком.
 * \param[in] order Порядковый номер пакета.
 * \param[in] handler Указатель на составной обработчик запроса <code>\link itp_handler \endlink</code>.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_push_proxy_item(itp_proxy_list_tp list, itp_time_t time, uint8_t address, uint16_t command, uint16_t order, itp_handler_tp handler);

/**
 * \brief Удаление указанного обработчика из списка.
 *
 * Память, занятая всеми структурами, также освобождается.
 * \ingroup private_functions
 * \param[in] list Указатель на список обработчиков \c itp_proxy_list.
 * \param[in] item Указатель на удаляемый обработчик \c itp_proxy_item.
 * \return Указатель на следующий узел в списке, если он есть.
 */
itp_proxy_item_tp itp_erase_proxy_item(itp_proxy_list_tp list, itp_proxy_item_tp item);

/**
 * \brief Поиск обработчика по номеру пакета.
 * \ingroup private_functions
 * \param[in] list Указатель на список обработчиков \c itp_proxy_list.
 * \param[in] address Адрес отправителя.
 * \param[in] order Порядковый номер пакета.
 * \return Указатель на обработчик, если он найден.
 */
itp_proxy_item_tp itp_find_proxy(itp_proxy_list_tp list, uint8_t address, uint16_t order);

/**
 * \brief Проверка списка обработчиков на пустоту.
 * \ingroup private_functions
 * \param[in] list Указатель на список обработчиков \c itp_proxy_list.
 * \return Если список пуст - 0 (ноль), если нет - 1 (единицу).
 */
uint8_t itp_proxy_list_is_empty(itp_proxy_list_tp list);

/**
 * \brief Очистка списка обработчиков.
 * \ingroup private_functions
 * \param[in] list Указатель на список обработчиков \c itp_proxy_list.
 */
void itp_clear_proxy_list(itp_proxy_list_tp list);

struct itp_listener_item;

/**
 * \brief Тип структуры слушателя соединений.
 */
typedef struct itp_listener_item itp_listener_item_t;

/**
 * \brief Указатель на структуру слушателя соединений.
 */
typedef itp_listener_item_t* itp_listener_item_tp;

/**
 * \brief Структура слушателя соединений.
 */
struct itp_listener_item {
    /**
     * \brief Узел сети.
     */
    itp_node_tp node;
    /**
     * \brief Уникальная метка.
     */
    uint8_t mark;
    /**
     * \brief Указатель на следующий элемент в списке.
     */
    itp_listener_item_tp next;
};

/**
 * \brief Структура списка слушателей соединений.
 */
struct itp_listener_list {
    /**
     * \brief Указатель на первого слушателя в списке.
     */
    itp_listener_item_tp first;
};

/**
 * \brief Тип структуры списка слушателей соединений.
 */
typedef struct itp_listener_list itp_listener_list_t;

/**
 * \brief Указатель на структуру списка слушателей соединений.
 */
typedef itp_listener_list_t* itp_listener_list_tp;

/**
 * \brief Инициализация слушателя соединений \c itp_listener_item.
 * \ingroup private_functions
 * \param[in] listener Указатель на слушателя соединений \c itp_listener_item.
 * \param[in] mark Уникальная метка.
 * \param[in] node Указатель на добавляемый узел \c itp_node.
 */
void itp_init_listener_item(itp_listener_item_tp listener, uint8_t mark, itp_node_tp node);

/**
 * \brief Освобождение памяти, занятой структурой слушателя соединений \c itp_listener_item.
 * \ingroup private_functions
 * \param[in] listener Указатель на слушателя соединений \c itp_listener_item.
 */
void itp_free_listener_item(itp_listener_item_tp listener);

/**
 * \brief Инициализация списка слушателей соединений \c itp_listener_list.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей соединений \c itp_listener_list.
 */
void itp_init_listener_list(itp_listener_list_tp list);

/**
 * \brief Добавление слушателя соединений в конец списка.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей \c itp_listener_list.
 * \param[in] mark Уникальная метка.
 * \param[in] node Указатель на добавляемый узел \c itp_node.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_push_listener_item(itp_listener_list_tp list, uint8_t mark, itp_node_tp node);

/**
 * \brief Удаление указанного слушателя соединений из списка.
 *
 * Память, занятая всеми структурами, также освобождается.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей \c itp_listener_list.
 * \param[in] node Указатель на добавляемый узел \c itp_node.
 * \return Указатель на следующий узел в списке, если он есть.
 */
itp_listener_item_tp itp_erase_listener_by_node(itp_listener_list_tp list, itp_node_tp node);

/**
 * \brief Удаление указанного слушателя соединений из списка.
 *
 * Память, занятая всеми структурами, также освобождается.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей \c itp_listener_list.
 * \param[in] endpoint Указатель на интерфейс передачи данных \c itp_endpoint.
 * \return Указатель на следующий узел в списке, если он есть.
 */
itp_listener_item_tp itp_erase_listener_by_endpoint(itp_listener_list_tp list, itp_endpoint_tp endpoint);

/**
 * \brief Поиск слушателя по метке.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей \c itp_listener_list.
 * \param[in] mark Адрес отправителя команды (или ноль).
 * \return Указатель на слушателя, если он найден.
 */
itp_listener_item_tp itp_find_listener(itp_listener_list_tp list, uint8_t mark);

/**
 * \brief Проверка списка слушателей соединений на пустоту.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей соединений \c itp_listener_list.
 * \return Если список пуст - 0 (ноль), если нет - 1 (единицу).
 */
uint8_t itp_listener_list_is_empty(itp_listener_list_tp list);

/**
 * \brief Очистка списка слушателей соединений.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей соединений \c itp_listener_list.
 */
void itp_clear_listener_list(itp_listener_list_tp list);

struct itp_remote_listener_item;

/**
 * \brief Тип структуры слушателя команд.
 */
typedef struct itp_remote_listener_item itp_remote_listener_item_t;

/**
 * \brief Указатель на структуру слушателя команд.
 */
typedef itp_remote_listener_item_t* itp_remote_listener_item_tp;

/**
 * \brief Структура слушателя команд.
 */
struct itp_remote_listener_item {
    /**
     * \brief Адрес узла сети.
     */
    uint8_t address;
    /**
     * \brief Команда (или 0 для всех команд).
     */
    uint16_t command;
    /**
     * \brief Указатель на следующий элемент в списке.
     */
    itp_remote_listener_item_tp next;
};

/**
 * \brief Структура списка слушателей команд.
 */
struct itp_remote_listener_list {
    /**
     * \brief Указатель на первого слушателя в списке.
     */
    itp_remote_listener_item_tp first;
};

/**
 * \brief Тип структуры списка слушателей команд.
 */
typedef struct itp_remote_listener_list itp_remote_listener_list_t;

/**
 * \brief Указатель на структуру списка слушателей команд.
 */
typedef itp_remote_listener_list_t* itp_remote_listener_list_tp;

/**
 * \brief Инициализация слушателя команд \c itp_remote_listener_item.
 * \ingroup private_functions
 * \param[in] listener Указатель на слушателя команд \c itp_remote_listener_item.
 * \param[in] address Адрес удалённого узла.
 * \param[in] command Команда.
 */
void itp_init_remote_listener_item(itp_remote_listener_item_tp listener, uint8_t address, uint16_t command);

/**
 * \brief Освобождение памяти, занятой структурой слушателя команд \c itp_remote_listener_item.
 * \ingroup private_functions
 * \param[in] listener Указатель на слушателя команд \c itp_remote_listener_item.
 */
void itp_free_remote_listener_item(itp_remote_listener_item_tp listener);

/**
 * \brief Инициализация списка слушателей команд \c itp_remote_listener_list.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей команд \c itp_remote_listener_list.
 */
void itp_init_remote_listener_list(itp_remote_listener_list_tp list);

/**
 * \brief Добавление слушателя команд в конец списка.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей \c itp_remote_listener_list.
 * \param[in] address Адрес удалённого узла.
 * \param[in] command Команда.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_push_remote_listener_item(itp_remote_listener_list_tp list, uint8_t address, uint16_t command);

/**
 * \brief Удаление указанных слушателей команд из списка.
 *
 * Память, занятая всеми структурами, также освобождается.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей \c itp_remote_listener_list.
 * \param[in] address Адрес удалённого узла.
 * \return Указатель на следующий узел в списке, если он есть.
 */
itp_remote_listener_item_tp itp_erase_remote_listeners_by_address(itp_remote_listener_list_tp list, uint8_t address);

/**
 * \brief Удаление указанных слушателей команд из списка.
 *
 * Память, занятая всеми структурами, также освобождается.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей \c itp_remote_listener_list.
 * \param[in] command Команда.
 * \return Указатель на следующий узел в списке, если он есть.
 */
itp_remote_listener_item_tp itp_erase_remote_listeners_by_command(itp_remote_listener_list_tp list, uint16_t command);

/**
 * \brief Вывод списка слушателей команд в отладку.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей команд \c itp_remote_listener_list.
 * \return Если список пуст - 0 (ноль), если нет - 1 (единицу).
 */
void itp_dump_remote_listener_list(itp_remote_listener_list_tp list);

/**
 * \brief Проверка списка слушателей команд на пустоту.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей команд \c itp_remote_listener_list.
 * \return Если список пуст - 0 (ноль), если нет - 1 (единицу).
 */
uint8_t itp_remote_listener_list_is_empty(itp_remote_listener_list_tp list);

/**
 * \brief Очистка списка слушателей команд.
 * \ingroup private_functions
 * \param[in] list Указатель на список слушателей команд \c itp_remote_listener_list.
 */
void itp_clear_remote_listener_list(itp_remote_listener_list_tp list);

struct itp_paired_buffer;

/**
 * \brief Тип структуры двунаправленного буфера <code>\link itp_paired_buffer \endlink</code>.
 */
typedef struct itp_paired_buffer itp_paired_buffer_t;

/**
 * \brief Указатель на структуру двунаправленного буфера <code>\link itp_paired_buffer \endlink</code>.
 */
typedef itp_paired_buffer_t* itp_paired_buffer_tp;

/**
 * \brief Структура двунаправленного буфера.
 */
struct itp_paired_buffer {
    /**
     * \brief Узел, которому принадлежит буфер.
     */
    itp_root_tp root;
    /**
     * \brief Адрес узла сети.
     */
    uint8_t address;
    /**
     * \brief Указатель на интерфейс передачи данных.
     */
    itp_endpoint_tp endpoint;
    /**
     * \brief Промежуточный буфер для хранения принятых данных.
     */
    itp_byte_t data[ITP_PAIRED_BUFFER_SIZE];
    /**
     * \brief Длина данных в буфере приёма.
     */
    itp_size_t length;
    /**
     * \brief Блокировка пакетов на чтение.
     */
    itp_time_t lock_read;
    /**
     * \brief Блокировка пакетов на запись.
     */
    itp_time_t lock_write;
    /**
     * \brief Указатель на следующий буфер в списке.
     */
    itp_paired_buffer_tp next;
};

/**
 * \brief Инициализация двунаправленного буфера.
 * \ingroup private_functions
 * \param[in] buffer Указатель на буфер \c itp_paired_buffer.
 * \param[in] root Узел, которому принадлежит буфер.
 * \param[in] address Адрес узла сети.
 * \param[in] endpoint Указатель на интерфейс передачи данных.
 */
void itp_init_paired_buffer(itp_paired_buffer_tp buffer, itp_root_tp root, uint8_t address, itp_endpoint_tp endpoint);

/**
 * \brief Освобождение памяти, занятой двунаправленным буфером.
 * \ingroup private_functions
 * \param[in] buffer Указатель на буфер \c itp_paired_buffer.
 */
void itp_free_paired_buffer(itp_paired_buffer_tp buffer);

/**
 * \brief Структура списка двунаправленных буферов.
 */
struct itp_buffer_list {
    /**
     * \brief Указатель на первый буфер в списке.
     */
    itp_paired_buffer_tp first;
};

/**
 * \brief Тип структуры списка двунаправленных буферов.
 */
typedef struct itp_buffer_list itp_buffer_list_t;

/**
 * \brief Указатель на структуру списка двунаправленных буферов.
 */
typedef itp_buffer_list_t* itp_buffer_list_tp;

/**
 * \brief Инициализация списка двунаправленных буферов \c itp_buffer_list.
 * \ingroup private_functions
 * \param[in] list Указатель на список двунаправленных буферов \c itp_buffer_list.
 */
void itp_init_buffer_list(itp_buffer_list_tp list);

/**
 * \brief Создание двунаправленного буфера в конце списка.
 * \ingroup private_functions
 * \param[in] list Указатель на список двунаправленных буферов \c itp_buffer_list.
 * \param[in] root Узел, которому принадлежит буфер.
 * \param[in] address Адрес узла сети.
 * \param[in] endpoint Указатель на интерфейс передачи данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_emplace_paired_buffer(itp_buffer_list_tp list, itp_root_tp root, uint8_t address, itp_endpoint_tp endpoint);

/**
 * \brief Удаление указанного двунаправленного буфера из списка.
 *
 * Память, занятая всеми структурами, также освобождается.
 * \ingroup private_functions
 * \param[in] list Указатель на список двунаправленных буферов \c itp_buffer_list.
 * \param[in] address Адрес узла сети.
 * \return Указатель на следующий узел в списке, если он есть.
 */
itp_paired_buffer_tp itp_erase_paired_buffer(itp_buffer_list_tp list, uint8_t address);

/**
 * \brief Поиск двунаправленного буфера по адресу.
 * \ingroup private_functions
 * \param[in] list Указатель на список двунаправленных буферов \c itp_buffer_list.
 * \param[in] address Адрес узла сети.
 * \return Указатель на двунаправленный буфер, если он найден.
 */
itp_paired_buffer_tp itp_find_buffer(itp_buffer_list_tp list, uint8_t address);

/**
 * \brief Проверка списка двунаправленных буферов на пустоту.
 * \ingroup private_functions
 * \param[in] list Указатель на список двунаправленных буферов \c itp_buffer_list.
 * \return Если список пуст - 0 (ноль), если нет - 1 (единицу).
 */
uint8_t itp_buffer_list_is_empty(itp_buffer_list_tp list);

/**
 * \brief Очистка списка двунаправленных буферов.
 * \ingroup private_functions
 * \param[in] list Указатель на список двунаправленных буферов \c itp_buffer_list.
 */
void itp_clear_buffer_list(itp_buffer_list_tp list);

#endif // ITP_LIST_H_INCLUDED
