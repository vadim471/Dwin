#ifndef ITP_ROOT_H_INCLUDED
#define ITP_ROOT_H_INCLUDED

/**
 * \file root.h
 * \brief Файл содержит функции для работы с корневой структурой узла сети.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "array.h"
#include "list.h"

/**
 * \brief Корневая структура узла сети.
 */
struct itp_root {
    /**
     * \brief Виртуальный адрес узла.
     */
    uint8_t address;
    /**
     * \brief Адрес корневого узла.
     */
    uint8_t root;
    /**
     * \brief Счётчик отправляемых пакетов.
     */
    uint16_t order;
    /**
     * \brief Время последнего опроса.
     */
    itp_time_t time;
    /**
     * \brief Указатель на родительский узел.
     */
    itp_node_tp parent;
    /**
     * \brief Список дочерних узлов сети.
     */
    itp_node_array_t children;
    /**
     * \brief Список слушателей соединения.
     */
    itp_listener_list_t listeners;
    /**
     * \brief Список слушателей команд.
     */
    itp_remote_listener_list_t remote_listeners;
    /**
     * \brief Список узлов, ожидающих подключения.
     */
    itp_pending_list_t pending;
    /**
     * \brief Список обработчиков запросов.
     */
    itp_handler_list_t handlers;
    /**
     * \brief Список кадров, ожидающих обработки.
     */
    itp_frame_list_t frames;
    /**
     * \brief Адреса удалённых узлов, ожидающих подключения.
     */
    itp_address_array_t suspend;
    /**
     * \brief Список обработчиков перехваченных запросов.
     */
    itp_proxy_list_t proxy;
    /**
     * \brief Список буферов для передачи данных на удалённый узел.
     */
    itp_buffer_list_t buffers;
    /**
     * \brief Обработчик создания новых узлов.
     */
    itp_on_create_fp on_create;
    /**
     * \brief Обработчик подключаемых узлов.
     */
    itp_on_connect_fp on_connect;
    /**
     * \brief Обработчик сброса конфигурации.
     */
    itp_on_signal_fp on_reset;
    /**
     * \brief Обработчик сигнала на обновление встроенного ПО.
     */
    itp_on_update_fp on_update_firmware;
    #ifdef ITP_EXTEND_CALLBACKS
    size_t on_create_eparam;
    size_t on_connect_eparam;
    size_t on_reset_eparam;
    size_t on_update_firmware_eparam;
    #endif // ITP_EXTEND_CALLBACKS
};

/**
 * \brief Вычисление порядкового номера нового кадра.
 * \ingroup private_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \return Порядковый номер кадра.
 */
uint16_t itp_enumerate_frame(itp_root_tp root);

/**
 * \brief Инициализация корневой структуры узла сети.
 * \ingroup public_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] on_create Указатель на обработчик создания узлов <code>\link itp_on_create_fp \endlink</code> или \c NULL.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_init_root(itp_root_tp root, itp_on_create_fp on_create
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Установка интерфейса передачи данных для родительского узла.
 * \ingroup public_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c itp_endpoint или \c NULL.
 * \param[in] on_error Указатель на обработчик ошибок <code>\link itp_on_error_fp \endlink</code> или \c NULL.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_set_parent(itp_root_tp root, itp_endpoint_tp endpoint, itp_on_error_fp on_error
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Получение свободного адреса в подсети.
 *
 * Поиск выполняется только на дочерних узлах сети, поэтому адрес следует запрашивать на самом верхнем узле.
 * \ingroup public_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] error Указатель на переменную, куда будет записан код ошибки. Опционально.
 * \return Незанятый адрес или 0 (ноль) в случае ошибки.
 */
uint8_t itp_get_free_address(itp_root_tp root, itp_error_code_t* error);

/**
 * \brief Существует ли прямое соединение до узла с указанным адресом.
 * \ingroup public_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] address Искомый адрес.
 * \return Если узел найден - 1, иначе - 0.
 */
uint8_t itp_is_local_child(itp_root_tp root, uint8_t address);

/**
 * \brief Создать новый узел, подключённый к локальному узлу сети.
 *
 * Память для структуры узла выделяется динамически путём вызова функции <code>\link itp_malloc \endlink</code>.
 * На время соединения структура добавляется в список ожидания.<br>
 * Функция выполняет следующие шаги:
 * <ul>
 * <li>создание структуры узла;</li>
 * <li>добавление во временный список (\c pending);</li>
 * <li>формирование запроса на задание адреса.</li>
 * </ul>
 * Остальная работа выполняется во время опроса узла:
 * <ul>
 * <li>опрос временного списка до получения таймаута/ответа;</li>
 * <li>удаление структуры из временного списка;</li>
 * <ul>
 * <li>добавление в список подключённых узлов в случае успеха;</li>
 * </ul>
 * <li>вызов пользовательского обработчика.</li>
 * </ul>
 * \ingroup public_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] time Текущее время в системе.
 * \param[in] endpoint Указатель на структуру передачи данных \c itp_endpoint.
 * \param[in] address Виртуальный адрес нового узла.
 * \param[in] on_connect Указатель на обработчик результата <code>\link itp_on_connect_fp \endlink</code>.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_connect_local_node(itp_root_tp root, itp_time_t time, itp_endpoint_tp endpoint, uint8_t address, itp_on_connect_fp on_connect
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Создать новый узел, подключённый к удалённому узлу сети.
 *
 * Функция отправляет запрос удалённому узлу сети, после чего ожидает подтверждения.
 * Адресат создаёт новый узел сети и отправляет пакет на задание сетевого адреса.
 * После получения подтверждения отправляется ответ родительскому узлу.
 * Узлы, передавшие пакет вниз по сети, при получении подтверждения добавляют
 * адрес узла в список дочерних адресов для обеспечения быстрой трассировки сети.
 * По завершении цепочки вызывается пользовательский обработчик результата.
 * \ingroup public_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] time Текущее время в системе.
 * \param[in] host Адрес узла, к которому подключено устройство.
 * \param[in] address Виртуальный адрес нового узла.
 * \param[in] path Локальный путь до устройства.
 * \param[in] on_connect Указатель на обработчик результата <code>\link itp_on_connect_fp \endlink</code>.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_connect_remote_node(itp_root_tp root, itp_time_t time, uint8_t host, uint8_t address, const char* path, itp_on_connect_fp on_connect
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Создать новый интерфейс передачи данных, связанный с удалённым узлом сети.
 *
 * На удалённом узле будет вызван обработчик \c on_create.
 * \ingroup public_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] time Текущее время в системе.
 * \param[in] address Виртуальный адрес нового узла.
 * \param[in] path Локальный путь до устройства.
 * \param[in] on_connect Указатель на обработчик результата <code>\link itp_on_connect_endpoint_fp \endlink</code>.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_connect_remote_endpoint(itp_root_tp root, itp_time_t time, uint8_t address, const char* path, itp_on_connect_endpoint_fp on_connect
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Заново установить соединение с дочерним узлом.
 * \ingroup public_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] time Текущее время в системе.
 * \param[in] address Виртуальный адрес дочернего узла.
 * \param[in] on_connect Указатель на обработчик результата <code>\link itp_on_connect_fp \endlink</code>.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_reconnect_local_node(itp_root_tp root, itp_time_t time, uint8_t address, itp_on_connect_fp on_connect
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Отправить запрос на установку соединения.
 * \ingroup public_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] time Текущее время в системе.
 * \param[in] on_result Указатель на обработчик результата <code>\link itp_on_result_fp \endlink</code>.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_request_connect(itp_root_tp root, itp_time_t time, itp_on_result_fp on_result
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Послать команду общего сброса.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] address Адрес конечного узла сети.
 */
itp_error_code_t itp_signal_reset(itp_root_tp root, uint8_t address);

/**
 * \brief Послать команду обновления встроенного ПО.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] address Адрес конечного узла сети.
 * \param[in] major Старшая версия.
 * \param[in] minor Младшая версия.
 * \param[in] build Версия сборки.
 */
itp_error_code_t itp_signal_update_firmware(itp_root_tp root, uint8_t address, uint16_t major, uint16_t minor, uint32_t build);

/**
 * \brief Сгенерировать буфер команды обновления встроенного ПО.
 * \ingroup command_layer
 * \param[in] buffer Буфер для данных.
 * \param[in] length На входе - размер буфера, на выходе - количество записанных байт.
 * \param[in] major Старшая версия.
 * \param[in] minor Младшая версия.
 * \param[in] build Версия сборки.
 * \param[in] use_checksum Флаг использования контрольной суммы при передаче данных.
 */
itp_error_code_t itp_generate_update_firmware(itp_byte_t* buffer, itp_size_t* length, uint16_t major, uint16_t minor, uint32_t build, uint8_t use_checksum);

/**
 * \brief Установить слушателя на указанную точку доступа.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] endpoint Точка доступа.
 */
itp_error_code_t itp_set_connection_listener(itp_root_tp root, itp_endpoint_tp endpoint);

/**
 * \brief Поиск узла с указанным адресом в подсети.
 *
 * Если дочерний узел не найден, будет позвращён родительский.
 * \ingroup private_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] address Искомый виртуальный адрес.
 * \return Указатель на структуру узла сети \c itp_node, если адрес найден.
 */
itp_node_tp itp_find_node(itp_root_tp root, uint8_t address);

/**
 * \brief Поиск кадра из списка по принятому кадру.
 * \ingroup private_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] frame Кадр для сопосталвения.
 * \return Указатель на кадр, если он найден.
 */
itp_frame_item_tp itp_find_frame(itp_root_tp root, itp_frame_tp frame);

/**
 * \brief Разбор очереди входящих пакетов узла.
 * \ingroup private_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] node Указатель на структуру узла сети \c itp_node.
 * \param[in] listener Уникальная метка, если обрабатывается слушатель.
 * \param[in] time Текущее время в системе.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_process_queue(itp_root_tp root, itp_node_tp node, itp_byte_t listener, itp_time_t time);

/**
 * \brief Обновление состояния кадров.
 * \ingroup private_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] time Текущее время в системе.
 */
void itp_process_frames(itp_root_tp root, itp_time_t time);

/**
 * \brief Обновить состояние корневой структуры узла сети.
 * \ingroup public_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] time Текущее время в системе (учитывается только дельта времени).
 */
void itp_poll_root(itp_root_tp root, itp_time_t time);

/**
 * \brief Выделение памяти для корневой структуры узла сети и её инициализация.
 * \ingroup public_functions
 * \param[in] on_create Указатель на обработчик создания узлов <code>\link itp_on_create_fp \endlink</code> или \c NULL.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_root_tp itp_create_root(itp_on_create_fp on_create
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Освобождение памяти, занятой полями корневой структуры узла сети.
 *
 * Память, занятая самой структурой не освобождается.
 * \ingroup public_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 */
void itp_dispose_root(itp_root_tp root);

/**
 * \brief Освобождение памяти, занятой корневой структурой узла сети.
 * \ingroup public_functions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 */
void itp_free_root(itp_root_tp root);

#endif // ITP_ROOT_H_INCLUDED
