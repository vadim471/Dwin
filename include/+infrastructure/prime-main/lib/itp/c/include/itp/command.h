#ifndef ITP_COMMAND_H_INCLUDED
#define ITP_COMMAND_H_INCLUDED

/**
 * \file command.h
 * \brief Файл содержит команды для взаимодействия с узлами сети.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "callback.h"

/**
 * \brief Добавление оповещения в очередь.
 *
 * Будет сформирован пакет с данным и добавлен в очередь отправки, а кадр будет добавлен в очередь ожидания.<br>
 * Обработчик будет вызван как при получении подтверждения, так и при возникновении ошибок (например, таймауте).<br>
 * При возникновении ошибки, память, занятая кадром, не освобождается.
 * \note В заголовке пакета должна быть установлена команда.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] frame Указатель на структуру кадра \c itp_frame.
 * \param[in] address Адрес получателя пакета с запросом.
 * \param[in] on_result Указатель на функцию обработки результата <code>\link itp_on_result_fp \endlink</code>. Опционально.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_push_notification(itp_root_tp root, itp_frame_tp frame, uint8_t address, itp_on_result_fp on_result
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Добавление события в очередь.
 *
 * Пакет будет отправлен всем подписчикам.<br>
 * При возникновении ошибки, память, занятая кадром, не освобождается.
 * \note В заголовке пакета должна быть установлена команда.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] frame Указатель на структуру кадра \c itp_frame.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_push_event(itp_root_tp root, itp_frame_tp frame);

/**
 * \brief Добавление запроса в очередь.
 *
 * Будет сформирован пакет с данным и добавлен в очередь отправки, а кадр будет добавлен в очередь ожидания.<br>
 * Обработчик будет вызван как при получении ответа, так и при возникновении ошибок (например, таймауте).<br>
 * При возникновении ошибки, память, занятая кадром, не освобождается.
 * \note В заголовке пакета должна быть установлена команда.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] frame Указатель на структуру кадра \c itp_frame.
 * \param[in] address Адрес получателя пакета с запросом.
 * \param[in] on_response Указатель на функцию обработки ответа <code>\link itp_on_response_fp \endlink</code>. Опционально.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_push_request(itp_root_tp root, itp_frame_tp frame, uint8_t address, itp_on_response_fp on_response
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Добавление ответа в очередь.
 *
 * Будет сформирован пакет с данным и добавлен в очередь отправки, а кадр будет добавлен в очередь ожидания.<br>
 * Обработчик будет вызван как при получении подтверждения, так и при возникновении ошибок (например, таймауте).<br>
 * При возникновении ошибки память, занятая кадром, не освобождается.
 * \note В заголовке пакета должна быть установлена команда.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] request Указатель на структура запроса \c itp_frame.
 * \param[in] response Указатель на структуру ответа \c itp_frame.
 * \param[in] on_result Указатель на функцию обработки результата <code>\link itp_on_result_fp \endlink</code>. Опционально.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_push_response(itp_root_tp root, itp_frame_tp request, itp_frame_tp response, itp_on_result_fp on_result
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Добавление ответа в очередь.
 *
 * Будет сформирован пакет с данным и добавлен в очередь отправки, а кадр будет добавлен в очередь ожидания.<br>
 * Обработчик будет вызван как при получении подтверждения, так и при возникновении ошибок (например, таймауте).<br>
 * При возникновении ошибки память, занятая кадром, не освобождается.
 * \note В заголовке пакета должна быть установлена команда.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] address Адрес узла, отправившего запрос.
 * \param[in] order Порядковый номер пакета с запросом.
 * \param[in] response Указатель на структуру ответа \c itp_frame.
 * \param[in] on_result Указатель на функцию обработки результата <code>\link itp_on_result_fp \endlink</code>. Опционально.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_push_response_for(itp_root_tp root, uint8_t address, uint16_t order, itp_frame_tp response, itp_on_result_fp on_result
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Регистрация обработчика входящих запросов.
 *
 * Обработчик будет вызываться каждый раз при получении запроса с указанной командой в заголовке.<br>
 * Нельзя установить обработчик для команд из диапазона \c 0x00.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] address Адрес отправителя команды. Для обработки команд от любых узлов указывается 0 (ноль).
 * \param[in] command Комманда с которой ассоциируется обработчик.
 * \param[in] on_request Указатель на фукнцию обработки запроса <code>\link itp_on_request_fp \endlink</code>.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_register_handler(itp_root_tp root, uint8_t address, uint16_t command, itp_on_request_fp on_request
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Удаление обработчика входящих запросов.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] address Адрес отправителя команды. Для удаления всех обработчиков указывается 0 (ноль).
 * \param[in] command Комманда с которой ассоциирован обработчик.
 */
void itp_unregister_handler(itp_root_tp root, uint8_t address, uint16_t command);

/**
 * \brief Удаление всех обработчиков входящих запросов.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 */
void itp_unregister_all_handlers(itp_root_tp root);

/**
 * \brief Подписка на события.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] address Адрес конечного узла сети.
 * \param[in] commands Список прослушиваемых команд (опционально).
 * \param[in] length Количество команд в списке.
 * \param[in] on_result Указатель на функцию обработки результата <code>\link itp_on_result_fp \endlink</code>. Опционально.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_listen_remote(itp_root_tp root, uint8_t address, const uint16_t* commands, uint8_t length, itp_on_result_fp on_result
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Трассировка маршрута.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] address Адрес конечного узла сети.
 * \param[in] on_response Указатель на функцию обработки ответа <code>\link itp_on_response_fp \endlink</code>. Опционально.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_trace_route(itp_root_tp root, uint8_t address, itp_on_response_fp on_response
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Получить список всех дочерних узлов.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] address Адрес узла сети, формирующего список.
 * \param[in] on_response Указатель на функцию обработки ответа <code>\link itp_on_response_fp \endlink</code>. Опционально.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_get_children_address(itp_root_tp root, uint8_t address, itp_on_response_fp on_response
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Отменить запросы с указанной командой.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] command Команда.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_cancel_requests(itp_root_tp root, uint16_t command);

/**
 * \brief Отменить все запросы, находящиеся на исполнении.
 * \ingroup command_layer
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_cancel_all_requests(itp_root_tp root);

/**
 * \brief Функция приёма данных через канал связи.
 * \ingroup private_functions
 * \param[in] object Указатель на двунаправленный буфер.
 * \param[in] data Указатель на буфер, в который производится чтение.
 * \param[in] size Размер буфера данных.
 * \return Количество прочитанных байт данных.
 */
itp_size_t itp_remote_endpoint_read(void* object, itp_byte_t* data, itp_size_t size);

/**
 * \brief Функция отправки данных через канал связи.
 * \ingroup private_functions
 * \param[in] object Указатель на двунаправленный буфер.
 * \param[in] data Указатель на буфер, содержащий передаваемые данные.
 * \param[in] length Количество байт, ожидающих записи.
 * \return Количество переданных байт данных.
 */
itp_size_t itp_remote_endpoint_write(void* object, const itp_byte_t* data, itp_size_t length);

#endif // ITP_COMMAND_H_INCLUDED
