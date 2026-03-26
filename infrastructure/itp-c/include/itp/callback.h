#ifndef ITP_CALLBACK_H_INCLUDED
#define ITP_CALLBACK_H_INCLUDED

/**
 * \file callback.h
 * \brief Файл содержит типы функций обратной связи.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "error.h"

#include <stddef.h>

/**
 * \brief Тип обработчика очистки памяти.
 * \ingroup callback_definitions
 * \param[in] ptr Указатель на функцию-обработчик.
 */
typedef void(*itp_free_user_handler_fp)(
    void* ptr
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Установка обработчика очистки памяти.
 * \ingroup callback_definitions
 * \param[in] f Указатель на функцию очистки.
 */
void itp_set_free_user_handler(
    itp_free_user_handler_fp f
);

/**
 * \brief Очистка памяти, занятой обработчиками.
 * \ingroup callback_definitions
 * \param[in] ptr Указатель на функцию-обработчик.
 */
void itp_free_user_handler(
    void* ptr
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Очистка памяти, занятой обработчиками.
 *
 * Вызывает \c itp_free_user_handler только для пользовательских обработчиков.
 * \ingroup callback_definitions
 * \param[in] ptr Указатель на функцию-обработчик.
 */
void itp_free_any_handler(
    void* ptr
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
);

/**
 * \brief Указатель на функцию обработки внутренних ошибок узла сети.
 * \ingroup callback_definitions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] address Адрес узла, вызвавшего ошибку.
 * \param[in] error Код ошибки <code>\link itp_error_code_t \endlink</code>.
 */
typedef void(*itp_on_error_fp)(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    itp_root_tp root,
    uint8_t address,
    itp_error_code_t error
);
#ifdef ITP_EXTEND_CALLBACKS
typedef void(itp_on_error_f)(itp_root_tp, uint8_t, itp_error_code_t);
#endif // ITP_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию обработки внутренних сигналов сети.
 * \ingroup callback_definitions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 */
typedef void(*itp_on_signal_fp)(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    itp_root_tp root
);
#ifdef ITP_EXTEND_CALLBACKS
typedef void(itp_on_signal_f)(itp_root_tp);
#endif // ITP_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию обработки подключения.
 * \ingroup callback_definitions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] address Адрес подключаемого узла.
 * \param[in] error Код ошибки <code>\link itp_error_t \endlink</code>.
 */
typedef void(*itp_on_connect_fp)(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    itp_root_tp root,
    uint8_t address,
    uint16_t error
);
#ifdef ITP_EXTEND_CALLBACKS
typedef void(itp_on_connect_f)(itp_root_tp, uint8_t, uint16_t);
#endif // ITP_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию обработки связывания интерфейсов передачи данных.
 * \ingroup callback_definitions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] address Адрес подключаемого узла.
 * \param[in] error Код ошибки <code>\link itp_error_t \endlink</code>.
 * \param[in] endpoint Указатель на интерфейс передачи данных.
 */
typedef void(*itp_on_connect_endpoint_fp)(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    itp_root_tp root,
    uint8_t address,
    uint16_t error,
    itp_endpoint_tp endpoint
);
#ifdef ITP_EXTEND_CALLBACKS
typedef void(itp_on_connect_endpoint_f)(itp_root_tp, uint8_t, uint16_t, itp_endpoint_tp);
#endif // ITP_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию создания нового узла.
 * \ingroup callback_definitions
 * \param[in] path Строка подключения устройства.
 * \param[in] error Указатель на переменную, куда будет записан код ошибки <code>\link itp_error_t \endlink</code>.
 * \return Указатель на структуру приёма/передачи \c itp_endpoint, если создание прошло успешно.
 */
typedef itp_endpoint_tp(*itp_on_create_fp)(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    const char* path,
    uint16_t* error
);
#ifdef ITP_EXTEND_CALLBACKS
typedef itp_endpoint_tp(itp_on_create_f)(const char*, uint16_t*);
#endif // ITP_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию обработки запроса от узла.
 * \ingroup callback_definitions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] frame Указатель на структуру кадра \c itp_frame.
 * \return Результат выполнения операции <code>\link itp_error_t \endlink</code>.
 */
typedef uint16_t(*itp_on_request_fp)(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    itp_root_tp root,
    itp_frame_tp frame
);
#ifdef ITP_EXTEND_CALLBACKS
typedef uint16_t(itp_on_request_f)(itp_root_tp, itp_frame_tp);
#endif // ITP_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию обработки результата операции.
 * \ingroup callback_definitions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] error Код ошибки <code>\link itp_error_t \endlink</code>.
 */
typedef void(*itp_on_result_fp)(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    itp_root_tp root,
    uint16_t error
);
#ifdef ITP_EXTEND_CALLBACKS
typedef void(itp_on_result_f)(itp_root_tp, uint16_t);
#endif // ITP_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию обработки ответа от узла.
 * \ingroup callback_definitions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] error Код ошибки протокола <code>\link itp_error_t \endlink</code>.
 * \param[in] frame Указатель на структуру кадра \c itp_frame, если он есть.
 */
typedef void(*itp_on_response_fp)(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    itp_root_tp root,
    uint16_t error,
    itp_frame_tp frame
);
#ifdef ITP_EXTEND_CALLBACKS
typedef void(itp_on_response_f)(itp_root_tp, uint16_t, itp_frame_tp);
#endif // ITP_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию обработки сигнала обновления.
 * \ingroup callback_definitions
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] major Старшая версия.
 * \param[in] minor Младшая версия.
 * \param[in] build Версия сборки.
 */
typedef void(*itp_on_update_fp)(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    itp_root_tp root,
    uint16_t major,
    uint16_t minor,
    uint32_t build
);
#ifdef ITP_EXTEND_CALLBACKS
typedef void(itp_on_update_f)(itp_root_tp, uint16_t, uint16_t, uint32_t);
#endif // ITP_EXTEND_CALLBACKS

#endif // ITP_CALLBACK_H_INCLUDED
