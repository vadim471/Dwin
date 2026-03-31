#ifndef ITP_CL2_ERROR_CODE_H_INCLUDED
#define ITP_CL2_ERROR_CODE_H_INCLUDED

/**
 * \file error_code.h
 * \brief Файл содержит набор используемых кодов ошибок.
 */

#ifdef ITP_COMPILE_CXX
namespace itp {
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление используемых кодов ошибок.
 * \ingroup errors_list
 */
typedef enum {

    /**
     * \brief \c <b>0x0101</b> Внутренняя ошибка.
     */
    CL2_ERR_INTERNAL = 0x0101,
    /**
     * \brief \c <b>0x0102</b> Ошибка при инициализации интерфейса передачи данных.
     */
    CL2_ERR_ENDPOINT_INIT_FAILED = 0x0102,
    /**
     * \brief \c <b>0x0103</b> Неизвестный тип устройства.
     */
    CL2_ERR_UNKNOWN_DEVICE_TYPE = 0x0103,
    /**
     * \brief \c <b>0x0104</b> Неверный адрес устройства.
     */
    CL2_ERR_WRONG_ADDRESS = 0x0104,
    /**
     * \brief \c <b>0x0105</b> Неверный код продукта.
     */
    CL2_ERR_WRONG_PRODUCT = 0x0105,
    /**
     * \brief \c <b>0x0106</b> Неверное состояние устройства.
     */
    CL2_ERR_WRONG_STATE = 0x0106,
    /**
     * \brief \c <b>0x0107</b> Команда не поддерживается устройством.
     */
    CL2_ERR_NOT_IMPLEMENTED = 0x0107,
    /**
     * \brief \c <b>0x0108</b> Ошибка в конфигурации устройства.
     */
    CL2_ERR_BAD_CONFIGURATION = 0x0108,
    /**
     * \brief \c <b>0x0109</b> Устройство занято.
     */
    CL2_ERR_BUSY = 0x0109,
    /**
     * \brief \c <b>0x010A</b> При работе с внешним модулем возникла ошибка.
     */
    CL2_ERR_MODULE_FAILED = 0x010A,
    /**
     * \brief \c <b>0x010B</b> Устройство или сервис не найдены.
     */
    CL2_ERR_NOT_FOUND = 0x010B,
    /**
     * \brief \c <b>0x010C</b> Устройство не отвечает.
     */
    CL2_ERR_TIMEOUT = 0x010C,
    /**
     * \brief \c <b>0x0301</b> Неверная команда считывателя.
     */
    CL2_ERR_CRD_BAD_COMMAND = 0x0301,
    /**
     * \brief \c <b>0x0302</b> Неверный тип ключа.
     */
    CL2_ERR_CRD_BAD_KEY_TYPE = 0x0302,
    /**
     * \brief \c <b>0x0303</b> Неверный ключ.
     */
    CL2_ERR_CRD_WRONG_KEY = 0x0303,
    /**
     * \brief \c <b>0x0304</b> Карта или прочее устройство не обнаружены.
     */
    CL2_ERR_CRD_EMPTY = 0x0304,
    /**
     * \brief \c <b>0x0305</b> Не удалось установить соединение с чипом или прочим устройством.
     */
    CL2_ERR_CRD_NOT_CONNECTED = 0x0305,
    /**
     * \brief \c <b>0x0306</b> Не удалось прочитать карту.
     */
    CL2_ERR_CRD_READ_FAILED = 0x0306,
    /**
     * \brief \c <b>0x0501</b> Попытка печати без текста.
     */
    CL2_ERR_PRT_EMPTY_TEXT = 0x0501,
    /**
     * \brief \c <b>0x0601</b> Карта не принадлежит сервису.
     */
    CL2_ERR_PAY_NOT_VALID = 0x0601,
    /**
     * \brief \c <b>0x0602</b> Сервис не смог выполнить операцию.
     */
    CL2_ERR_PAY_INTERNAL = 0x0602,
    /**
     * \brief \c <b>0x0603</b> Необходим ввод пин-кода.
     *
     * Структура ошибки:
     * <ul>
     * <li>\b uint8 Тип пин-кода <code>\link itp_cl2_pin_type_t\endlink</code>.</li>
     * </ul>
     */
    CL2_ERR_PAY_PIN_REQUIRED = 0x0603,
    /**
     * \brief \c <b>0x0604</b> Неподходящая версия апплета.
     */
    CL2_ERR_PAY_BAD_SAM_VERSION = 0x0604

#ifdef ITP_COMPILE_CXX
} cl2_error_t;
#else // ITP_COMPILE_CXX
} itp_cl2_error_t;
#endif // ITP_COMPILE_CXX

/**
 * \brief Перечисление используемых платёжными сервисами кодов ошибок.
 * \ingroup errors_list
 */
typedef enum {

    /**
     * \brief \c <b>0x00</b> Нет ошибки.
     */
    CL2_PAY_ERR_NONE = 0x00,

    /**
     * \brief \c <b>0x01</b> Универсальный код ошибки.
     *
     * Операция была отменена, подтверждение не требуется.
     */
    CL2_PAY_ERR_GENERIC_ERROR = 0x01,

    /**
     * \brief \c <b>0x02</b> Неверный пин-код.
     *
     * Операция была отменена, подтверждение не требуется.
     */
    CL2_PAY_ERR_INVALID_PIN = 0x02,

    /**
     * \brief \c <b>0x03</b> Карта заблокирована.
     *
     * Операция была отменена, подтверждение не требуется.
     */
    CL2_PAY_ERR_CARD_BLOCKED = 0x03,

    /**
     * \brief \c <b>0x04</b> Операция выполнена частично.
     *
     * Это код означает, что услуга выполнена не в полной мере. Например, транзакция одобрена лишь на часть средств.<br>
     * При возврате этого кода требуется дальнейшее подтверждение или отмена операции.
     */
    CL2_PAY_ERR_PARTIAL_OPERATION = 0x04,

    /**
     * \brief \c <b>0x05</b> Операция будет выполнена позже.
     *
     * При возврате этого кода операция считается выполненной, но результат не может быть получен из ответа.
     */
    CL2_PAY_ERR_DELAYED_OPERATION = 0x05

#ifdef ITP_COMPILE_CXX
} cl2_payment_error_t;
#else // ITP_COMPILE_CXX
} itp_cl2_payment_error_t;
#endif // ITP_COMPILE_CXX

#ifdef ITP_COMPILE_CXX
}
#endif // ITP_COMPILE_CXX

#endif // ITP_CL2_ERROR_CODE_H_INCLUDED
