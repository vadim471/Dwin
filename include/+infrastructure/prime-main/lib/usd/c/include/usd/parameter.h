#ifndef USD_PARAMETER_H_INCLUDED
#define USD_PARAMETER_H_INCLUDED

/**
 * \file parameter.h
 * \brief Файл содержит описание универсального параметра.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "variant.h"

/**
 * \brief Структура универсального параметра.
 */
struct usd_parameter {
    /**
     * \brief Ключ параметра.
     */
    uint8_t key;
    /**
     * \brief Тип параметра.
     */
    usd_variant_type_t type;
    /**
     * \brief Значение параметра.
     */
    usd_variant_t value;
    /**
     * \brief Доступ только для чтения (по умолчанию).
     */
    uint8_t read_only;
    /**
     * \brief Ошибка нижнего уровня.
     */
    uint16_t error;
};

/**
 * \brief Инициализация структуры универсального параметра.
 * \ingroup parameter_functions
 * \param[in] parameter Указатель на структуру универсального параметра \c usd_parameter.
 * \param[in] key Идентификатор параметра.
 * \param[in] type Хранимый тип параметра \c usd_variant_type_t.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_init_parameter(usd_parameter_tp parameter, uint8_t key, usd_variant_type_t type);

/**
 * \brief Получить значение параметра.
 * \ingroup parameter_functions
 * \param[in] parameter Указатель на структуру универсального параметра \c usd_parameter.
 * \param[in] value Указатель на переменную, куда будет записана мантисса числа.
 * \param[in] exponent Указатель на переменную, куда будет записана экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_get_parameter_value(usd_parameter_tp parameter, int32_t* value, uint8_t* exponent);

/**
 * \brief Записать значение параметра.
 * \ingroup parameter_functions
 * \param[in] parameter Указатель на структуру универсального параметра \c usd_parameter.
 * \param[in] value Мантисса числа.
 * \param[in] exponent Экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_set_parameter_value(usd_parameter_tp parameter, int32_t value, uint8_t exponent);

/**
 * \brief Записать значение параметра (без вызова связанных функций).
 * \ingroup parameter_functions
 * \param[in] parameter Указатель на структуру универсального параметра \c usd_parameter.
 * \param[in] value Мантисса числа.
 * \param[in] exponent Экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_store_parameter_value(usd_parameter_tp parameter, int32_t value, uint8_t exponent);

/**
 * \brief Получить значение параметра (64-разрядная версия).
 * \ingroup parameter_functions
 * \param[in] parameter Указатель на структуру универсального параметра \c usd_parameter.
 * \param[in] value Указатель на переменную, куда будет записана мантисса числа.
 * \param[in] exponent Указатель на переменную, куда будет записана экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_get_parameter_value_64(usd_parameter_tp parameter, int64_t* value, uint8_t* exponent);

/**
 * \brief Записать значение параметра (64-разрядная версия).
 * \ingroup parameter_functions
 * \param[in] parameter Указатель на структуру универсального параметра \c usd_parameter.
 * \param[in] value Мантисса числа.
 * \param[in] exponent Экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_set_parameter_value_64(usd_parameter_tp parameter, int64_t value, uint8_t exponent);

/**
 * \brief Записать значение параметра (64-разрядная версия, без вызова связанных функций).
 * \ingroup parameter_functions
 * \param[in] parameter Указатель на структуру универсального параметра \c usd_parameter.
 * \param[in] value Мантисса числа.
 * \param[in] exponent Экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_store_parameter_value_64(usd_parameter_tp parameter, int64_t value, uint8_t exponent);

#endif // USD_PARAMETER_H_INCLUDED
