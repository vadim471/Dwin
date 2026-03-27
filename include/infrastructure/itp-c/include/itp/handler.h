#ifndef ITP_HANDLER_H_INCLUDED
#define ITP_HANDLER_H_INCLUDED

/**
 * \file handler.h
 * \brief Файл содержит методы для работы с составным обработчиком событий.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "callback.h"

typedef enum {
    /**
     * \brief Неизвестный тип обработчика.
     */
    ITP_HND_UNKNOWN = 0,
    /**
     * \brief Запрос на подключение к локальному узлу.
     */
    ITP_HND_LOCAL,
    /**
     * \brief Запрос на подключение к удалённому узлу.
     */
    ITP_HND_REMOTE,
    /**
     * \brief Запрос на переподключение узла.
     */
    ITP_HND_RECONNECT,
    /**
     * \brief Запрос на подключение к удалённому интерфейсу связи.
     */
    ITP_HND_CHANNEL,
    /**
     * \brief Запрос с верхнего уровня на подключение к локальному узлу.
     */
    ITP_HND_HOST,
    /**
     * \brief Запрос с верхнего уровня на подключение к удалённому узлу.
     */
    ITP_HND_PROXY
} itp_handler_type_t;

/**
 * \brief Структура контейнера для составного обработчика событий.
 */
struct itp_handler_wrapper {
    /**
     * \brief Указатель на обработчика событий.
     */
    void* handler;
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam;
    #endif // ITP_EXTEND_CALLBACKS
    /**
     * \brief Тип обработчика событий.
     *
     * 0 - простой указатель, 1 - указатель на структуру \c itp_handler.
     */
    uint8_t complex;
    /**
     * \brief Тип простого обработчика событий.
     */
    uint8_t response;
};

/**
 * \brief Тип структуры контейнера для составного обработчика событий \c itp_handler_wrapper.
 */
typedef struct itp_handler_wrapper itp_handler_wrapper_t;

/**
 * \brief Указатель на структуру контейнера для составного обработчика событий \c itp_handler_wrapper.
 */
typedef itp_handler_wrapper_t* itp_handler_wrapper_tp;

/**
 * \brief Инициализация структуры контейнера для составного обработчика событий \c itp_handler_wrapper.
 * \ingroup private_functions
 * \param[in] wrapper Указатель на структуру \c itp_handler_wrapper.
 * \param[in] handler Указатель на обработчик событий.
 * \param[in] complex Тип обработчика событий.
 * \param[in] response Тип простого обработчика событий (если complex=0).
 */
void itp_init_handler_wrapper(
    itp_handler_wrapper_tp wrapper,
    void* handler,
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    uint8_t complex,
    uint8_t response
);

/**
 * \brief Очистка структуры контейнера для составного обработчика событий \c itp_handler_wrapper.
 * \ingroup private_functions
 * \param[in] wrapper Указатель на структуру \c itp_handler_wrapper.
 */
void itp_dispose_handler_wrapper(itp_handler_wrapper_tp wrapper);

/**
 * \brief Структура составного обработчика событий.
 */
struct itp_handler {
    /**
     * \brief Тип обработчика.
     */
    uint8_t type;
    /**
     * \brief Адрес узла.
     */
    uint8_t address;
    /**
     * \brief Указатель на узел передачи данных.
     */
    itp_node_tp node;
    /**
     * \brief Указатель на функцию обработки события.
     */
    void* on_result;
    #ifdef ITP_EXTEND_CALLBACKS
    size_t on_result_eparam;
    #endif // ITP_EXTEND_CALLBACKS
    /**
     * \brief Команда.
     */
    uint16_t command;
    /**
     * \brief Порядковый номер пакета.
     */
    uint16_t order;
};

/**
 * \brief Тип структуры составного обработчика событий <code>\link itp_handler \endlink</code>.
 */
typedef struct itp_handler itp_handler_t;

/**
 * \brief Указатель на структуру составного обработчика событий <code>\link itp_handler \endlink</code>.
 */
typedef itp_handler_t* itp_handler_tp;

/**
 * \brief Создание составного обработчика событий \c itp_handler.
 * \ingroup private_functions
 * \param[in] type Тип обработчика <code>\link itp_handler_type_t \endlink</code>.
 * \return Указатель на созданную структуру \c itp_handler.
 */
itp_handler_tp itp_create_handler(uint8_t type);

/**
 * \brief Удаление составного обработчика событий \c itp_handler.
 * \ingroup private_functions
 * \param[in] handler Указатель на созданную структуру \c itp_handler.
 */
void itp_free_handler(itp_handler_tp handler);

/**
 * \brief Вызов составного обработчика событий \c itp_handler.
 * \ingroup private_functions
 * \param[in] wrapper Указатель на структуру составного обработчика \c itp_handler_wrapper.
 * \param[in] root Указатель на корневую структуру узла сети \c itp_root.
 * \param[in] error Результат выполнения операции <code>\link itp_error_t \endlink</code>.
 * \param[in] frame Указатель на кадр \c itp_frame.
 */
void itp_call_handler(itp_handler_wrapper_tp wrapper, itp_root_tp root, uint16_t error, itp_frame_tp frame);

#endif // ITP_HANDLER_H_INCLUDED
