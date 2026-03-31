#ifndef USD_ERROR_CODE_H_INCLUDED
#define USD_ERROR_CODE_H_INCLUDED

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
    USD_ERRC_NONE = 0,
    /**
     * \brief Вывод служебной информации.
     */
    USD_ERRC_TRACE,
    /**
     * \brief Код ошибки общего назначения.
     */
    USD_ERRC_GENERAL,
    /**
     * \brief Предупреждение о возможной внутренней ошибке.
     */
    USD_ERRC_WARNING,
    /**
     * \brief Внутренняя ошибка библиотеки.
     */
    USD_ERRC_INTERNAL,
    /**
     * \brief Найден недействительный указатель.
     */
    USD_ERRC_NULL_POINTER,
    /**
     * \brief Ошибка при динамическом выделении памяти.
     */
    USD_ERRC_MALLOC_FAILED,
    /**
     * \brief Переполнение динамически выделяемой памяти.
     */
    USD_ERRC_OVERFLOW,
    /**
     * \brief Выход за пределы выделенной памяти.
     */
    USD_ERRC_OUT_OF_BOUNDS,
    /**
     * \brief Аргемент функции имеет недопустимое значение.
     */
    USD_ERRC_BAD_ARGUMENT,
    /**
     * \brief Идентификатор уже существует.
     */
    USD_ERRC_DUPLICATE_KEY,
    /**
     * \brief Операция не поддерживается.
     */
    USD_ERRC_NOT_SUPPORTED,
    /**
     * \brief Элемент не найден.
     */
    USD_ERRC_NOT_FOUND
} usd_error_code_t;

#endif // USD_ERROR_CODE_H_INCLUDED
