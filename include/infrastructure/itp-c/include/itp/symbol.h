#ifndef ITP_SYMBOL_H_INCLUDED
#define ITP_SYMBOL_H_INCLUDED

/**
 * \file symbol.h
 * \brief Файл содержит управляющие символы и внутренние команды протокола.
 */

/**
 * \brief Перечисление управляющих символов протокола.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Запрос к узлу.
     */
    ITP_SYM_REQ = 0x02,
    /**
     * \brief Подтверждение доставки.
     */
    ITP_SYM_ACK = 0x00,
    /**
     * \brief Данные приняты с ошибкой или команда неверна.
     */
    ITP_SYM_NAK = 0x01,
    /**
     * \brief Данные в ответ на запрос.
     */
    ITP_SYM_DAT = 0x03,
    /**
     * \brief Служебные запросы и конфигурирование сети.
     */
    ITP_SYM_CTR = 0x04
} itp_symbol_t;

/**
 * \brief Перечисление поддерживаемых команд протокола.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Задание виртуального адреса узла.
     */
    ITP_CMD_SET_ADDRESS = 0x0001,
    /**
     * \brief Создание дочернего узла.
     */
    ITP_CMD_CREATE_NODE = 0x0002,
    /**
     * \brief Запрос списка дочерних узлов.
     */
    ITP_CMD_GET_CHILDREN = 0x0003,
    /**
     * \brief Трассировка маршрута.
     */
    ITP_CMD_TRACE_ROUTE = 0x0004,
    /**
     * \brief Общий сброс.
     */
    ITP_CMD_RESET = 0x0005,
    /**
     * \brief Перейти в режим обновления встроенного ПО.
     */
    ITP_CMD_UPDATE_FIRMWARE = 0x0006,
    /**
     * \brief Подписка на события.
     */
    ITP_CMD_LISTEN_REMOTE = 0x0007,
    /**
     * \brief Передать данные целевому узлу.
     */
    ITP_CMD_TRANSMIT_DATA = 0x0008,
    /**
     * \brief Запрос на чтение данных.
     */
    ITP_CMD_REQUEST_DATA = 0x0009,
    /**
     * \brief Запрос на подключение дочернего узла.
     */
    ITP_CMD_REQUEST_CONNECT = 0x000A,
    /**
     * \brief Запрос на отключение от сети.
     */
    ITP_CMD_REQUEST_DISCONNECT = 0x000B,
    /**
     * \brief Событие с отладочным сообщением.
     *
     * Формат данных:
     * <ul>
     * <li>\b uint8 Уровень <code>\link itp_debug_level_t\endlink</code>;</li>
     * <li>\b string Объект, создавший сообщение;</li>
     * <li>\b wstring Тело сообщения.</li>
     * </ul>
     */
    ITP_CMD_DEBUG_MESSAGE = 0x00A0
} itp_command_t;

/**
 * \brief Перечисление поддерживаемых типов подключения.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Прямое подключение (инициируется корневым узлом).
     */
    ITP_CON_FORWARD = 0x01,
    /**
     * \brief Обратное подключение (происходит в ответ на запрос подключения).
     */
    ITP_CON_REVERSE = 0x02,
    /**
     * \brief Создать канал передачи данных.
     */
    ITP_CON_CHANNEL = 0x03
} itp_connection_type_t;

/**
 * \brief Тип узла сети.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Тип по умолчанию.
     */
    ITP_NODE_TYPE_DEFAULT = 0,
    /**
     * \brief Ожидающий соединения узел.
     */
    ITP_NODE_TYPE_PENDING,
    /**
     * \brief Родительский узел.
     */
    ITP_NODE_TYPE_PARENT,
    /**
     * \brief Дочерний узел.
     */
    ITP_NODE_TYPE_CHILD,
    /**
     * \brief Слушатель соединений.
     */
    ITP_NODE_TYPE_LISTENER
} itp_node_type_t;

/**
 * \brief Уровни отладочных сообщений.
 * \ingroup enum_definitions
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Отсутствует.
     */
    ITP_DEBUG_LEVEL_NONE = 0x00,

    /**
     * \brief \c <b>0x01</b> FATAL.
     */
    ITP_DEBUG_LEVEL_FATAL = 0x01,

    /**
     * \brief \c <b>0x02</b> ERROR.
     */
    ITP_DEBUG_LEVEL_ERROR = 0x02,

    /**
     * \brief \c <b>0x03</b> WARNING.
     */
    ITP_DEBUG_LEVEL_WARNING = 0x03,

    /**
     * \brief \c <b>0x04</b> INFO.
     */
    ITP_DEBUG_LEVEL_INFO = 0x04,

    /**
     * \brief \c <b>0x05</b> DEBUG.
     */
    ITP_DEBUG_LEVEL_DEBUG = 0x05,

    /**
     * \brief \c <b>0x06</b> TRACE.
     */
    ITP_DEBUG_LEVEL_TRACE = 0x06

} itp_debug_level_t;

#endif // ITP_SYMBOL_H_INCLUDED
