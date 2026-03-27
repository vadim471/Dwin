#ifndef ITP_ERROR_CODE_H_INCLUDED
#define ITP_ERROR_CODE_H_INCLUDED

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
    ITP_ERRC_NONE = 0,
    /**
     * \brief Вывод служебной информации.
     */
    ITP_ERRC_TRACE,
    /**
     * \brief Код ошибки общего назначения.
     */
    ITP_ERRC_GENERAL,
    /**
     * \brief Предупреждение о возможной внутренней ошибке.
     */
    ITP_ERRC_WARNING,
    /**
     * \brief Внутренняя ошибка библиотеки.
     */
    ITP_ERRC_INTERNAL,
    /**
     * \brief Время ожидания истекло.
     */
    ITP_ERRC_TIMEOUT,
    /**
     * \brief Операция отменена.
     */
    ITP_ERRC_CANCELLED,
    /**
     * \brief Команда не поддерживается библиотекой.
     */
    ITP_ERRC_NOT_IMPLEMENTED,
    /**
     * \brief Ошибка в структуре пакета.
     */
    ITP_ERRC_PARSE_FAILED,
    /**
     * \brief Найден недействительный указатель.
     */
    ITP_ERRC_NULL_POINTER,
    /**
     * \brief Ошибка при динамическом выделении памяти.
     */
    ITP_ERRC_MALLOC_FAILED,
    /**
     * \brief Переполнение динамически выделяемой памяти.
     */
    ITP_ERRC_OVERFLOW,
    /**
     * \brief Выход за пределы выделенной памяти.
     */
    ITP_ERRC_OUT_OF_BOUNDS,
    /**
     * \brief Интерфейс передачи данных не настроен.
     */
    ITP_ERRC_EP_NOT_CONFIGURED,
    /**
     * \brief Сбой во время приёма/передачи.
     */
    ITP_ERRC_TRANSMIT_FAILED,
    /**
     * \brief Искомый или свободный адрес не найден.
     */
    ITP_ERRC_ADDRESS_NOT_FOUND,
    /**
     * \brief Получено сообщение с отрицанием или неверной командой.
     */
    ITP_ERRC_BAD_RESPONSE,
    /**
     * \brief Аргемент функции имеет недопустимое значение.
     */
    ITP_ERRC_BAD_ARGUMENT,
    /**
     * \brief Ошибка в контрольной сумме.
     */
    ITP_ERRC_CRC_FAILED,
    /**
     * \brief Команда недопустима или не найдена.
     */
    ITP_ERRC_WRONG_COMMAND
} itp_error_code_t;

#endif // ITP_ERROR_CODE_H_INCLUDED
