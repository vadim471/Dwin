#ifndef ITP_ERROR_H_INCLUDED
#define ITP_ERROR_H_INCLUDED

/**
 * \file error.h
 * \brief Файл содержит описание сообщений об ошибках.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"

/**
 * \brief Перечисление кодов внутренних ошибок протокола.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Ошибка отсутствует.
     */
    ITP_ERR_NONE = 0x0000,
    /**
     * \brief Внутренняя ошибка на устройстве.
     */
    ITP_ERR_INTERNAL = 0x0001,
    /**
     * \brief Недействительный указатель.
     */
    ITP_ERR_NULL_POINTER = 0x0002,
    /**
     * \brief Контрольная сумма не совпала.
     */
    ITP_ERR_CRC_FAILED = 0x0003,
    /**
     * \brief Ошибка приёма/передачи.
     */
    ITP_ERR_TRANSMIT_FAILED = 0x0004,
    /**
     * \brief Истекло время ожидания.
     */
    ITP_ERR_TIMEOUT = 0x0005,
    /**
     * \brief Неверный статус запроса.
     */
    ITP_ERR_WRONG_STATUS = 0x0006,
    /**
     * \brief Неверный виртуальный адрес.
     */
    ITP_ERR_WRONG_ADDRESS = 0x0007,
    /**
     * \brief Неверная команда.
     */
    ITP_ERR_WRONG_COMMAND = 0x0008,
    /**
     * \brief Нарушение последовательности отправки.
     */
    ITP_ERR_WRONG_SEQUENCE = 0x0009,
    /**
     * \brief Операция проигнорирована.
     */
    ITP_ERR_IGNORED = 0x000A,
    /**
     * \brief Неверный путь к устройству.
     */
    ITP_ERR_WRONG_PATH = 0x000B,
    /**
     * \brief Неверные данные в пакете.
     */
    ITP_ERR_BAD_DATA = 0x000C,
    /**
     * \brief Неверная конфигурация узла.
     */
    ITP_ERR_NOT_CONFIGURED = 0x000D,
    /**
     * \brief Неизвестная ошибка.
     */
    ITP_ERR_UNKNOWN = 0x000E,
    /**
     * \brief Ошибка при выделении памяти.
     */
    ITP_ERR_MALLOC_FAILED = 0x000F,
    /**
     * \brief Функция не реализована.
     */
    ITP_ERR_NOT_IMPLEMENTED = 0x0010,
    /**
     * \brief Операция отменена.
     */
    ITP_ERR_CANCELLED = 0x0011,
    /**
     * \brief Устройство занято и не может обработать запрос.
     */
    ITP_ERR_BUSY = 0x0012
} itp_error_t;

/**
 * \brief Структура описания внутренней ошибки протокола.
 */
struct itp_error_description {
    /**
     * \brief Код ошибки.
     */
    uint16_t error_code;
    /**
     * \brief Текстовое описание.
     */
    char* description;
};

/**
 * \brief Тип структуры описания внутренней ошибки протокола.
 */
typedef struct itp_error_description itp_error_description_t;

/**
 * \brief Вернуть описание ошибки протокола по её коду.
 * \param[in] error_code Код ошибки.
 * \return Строку с описанием ошибки.
 */
const char* itp_get_error_description(uint16_t error_code);

/**
 * \brief Вернуть код ошибки протокола по коду внутренней ошибки.
 * \param[in] error_code Код внутренней ошибки.
 * \return Код ошибки протокола.
 */
uint16_t itp_cast_error_code(itp_error_code_t error_code);

#endif // ITP_ERROR_H_INCLUDED
