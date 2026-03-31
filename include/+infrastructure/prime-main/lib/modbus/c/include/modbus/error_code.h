#ifndef MODBUS_ERROR_CODE_H_INCLUDED
#define MODBUS_ERROR_CODE_H_INCLUDED

/**
 * \file error_code.h
 * \brief Файл содержит перечисление кодов ошибок, возвращаемых функциями библиотеки.
 */

/**
 * \brief Перечисление кодов ошибок.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Ошибка отсутствует.
     */
    MBS_ERRC_NONE = 0,
    /**
     * \brief Вывод служебной информации.
     */
    MBS_ERRC_TRACE,
    /**
     * \brief Код ошибки общего назначения.
     */
    MBS_ERRC_GENERAL,
    /**
     * \brief Предупреждение о возможной внутренней ошибке.
     */
    MBS_ERRC_WARNING,
    /**
     * \brief Внутренняя ошибка библиотеки.
     */
    MBS_ERRC_INTERNAL,
    /**
     * \brief Время ожидания истекло.
     */
    MBS_ERRC_TIMEOUT,
    /**
     * \brief Операция отменена.
     */
    MBS_ERRC_CANCELLED,
    /**
     * \brief Команда не поддерживается библиотекой.
     */
    MBS_ERRC_NOT_IMPLEMENTED,
    /**
     * \brief Ошибка в структуре пакета.
     */
    MBS_ERRC_PARSE_FAILED,
    /**
     * \brief Найден недействительный указатель.
     */
    MBS_ERRC_NULL_POINTER,
    /**
     * \brief Ошибка при динамическом выделении памяти.
     */
    MBS_ERRC_MALLOC_FAILED,
    /**
     * \brief Переполнение динамически выделяемой памяти.
     */
    MBS_ERRC_OVERFLOW,
    /**
     * \brief Выход за пределы выделенной памяти.
     */
    MBS_ERRC_OUT_OF_BOUNDS,
    /**
     * \brief Интерфейс передачи данных не настроен.
     */
    MBS_ERRC_EP_NOT_CONFIGURED,
    /**
     * \brief Сбой во время приёма/передачи.
     */
    MBS_ERRC_TRANSMIT_FAILED,
    /**
     * \brief Получено сообщение с неверным адресом.
     */
    MBS_ERRC_WRONG_ADDRESS,
    /**
     * \brief Получено сообщение с отрицанием или неверной командой.
     */
    MBS_ERRC_BAD_RESPONSE,
    /**
     * \brief Аргемент функции имеет недопустимое значение.
     */
    MBS_ERRC_BAD_ARGUMENT,
    /**
     * \brief Получено сообщение с неверной контрольной суммой.
     */
    MBS_ERRC_CRC_FAILED,
    /**
     * \brief Неизвестная команда.
     */
    MBS_ERRC_UNKNOWN_COMMAND
} mbs_error_code_t;

#endif // MODBUS_ERROR_CODE_H_INCLUDED
