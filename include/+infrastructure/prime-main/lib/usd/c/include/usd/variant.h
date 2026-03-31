#ifndef USD_VARIANT_H_INCLUDED
#define USD_VARIANT_H_INCLUDED

/**
 * \file variant.h
 * \brief Файл содержит составные типы для \c variant.
 */

#include "types.h"

#include <stddef.h>

struct usd_dec32f_param;
typedef struct usd_dec32f_param usd_dec32f_param_t;
typedef usd_dec32f_param_t* usd_dec32f_param_tp;

struct usd_dec64f_param;
typedef struct usd_dec64f_param usd_dec64f_param_t;
typedef usd_dec64f_param_t* usd_dec64f_param_tp;

struct usd_boolf_param;
typedef struct usd_boolf_param usd_boolf_param_t;
typedef usd_boolf_param_t* usd_boolf_param_tp;

/**
 * \brief Тип функции, используемой для чтения параметра \c dec32.
 * \ingroup variant_definitions
 * \param[in] param Указатель на структуру параметра.
 * \param[in] value Указатель на переменную, куда будет записана мантисса числа.
 * \param[in] exponent Указатель на переменную, куда будет записана экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
typedef usd_error_code_t(*usd_get_dec32_parameter_fp)(usd_dec32f_param_tp param, int32_t* value, uint8_t* exponent);

/**
 * \brief Тип функции, используемой для записи параметра \c dec32.
 * \ingroup variant_definitions
 * \param[in] param Указатель на структуру параметра.
 * \param[in] value Мантисса числа.
 * \param[in] exponent Экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
typedef usd_error_code_t(*usd_set_dec32_parameter_fp)(usd_dec32f_param_tp param, int32_t value, uint8_t exponent);

/**
 * \brief Тип функции, используемой для чтения параметра \c dec64.
 * \ingroup variant_definitions
 * \param[in] param Указатель на структуру параметра.
 * \param[in] value Указатель на переменную, куда будет записана мантисса числа.
 * \param[in] exponent Указатель на переменную, куда будет записана экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
typedef usd_error_code_t(*usd_get_dec64_parameter_fp)(usd_dec64f_param_tp param, int64_t* value, uint8_t* exponent);

/**
 * \brief Тип функции, используемой для записи параметра \c dec64.
 * \ingroup variant_definitions
 * \param[in] param Указатель на структуру параметра.
 * \param[in] value Мантисса числа.
 * \param[in] exponent Экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
typedef usd_error_code_t(*usd_set_dec64_parameter_fp)(usd_dec64f_param_tp param, int64_t value, uint8_t exponent);

/**
 * \brief Тип функции, используемой для чтения булевого параметра.
 * \ingroup variant_definitions
 * \param[in] param Указатель на структуру параметра.
 * \param[in] value Указатель на переменную, куда будет записано значение.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
typedef usd_error_code_t(*usd_get_bool_parameter_fp)(usd_boolf_param_tp param, uint8_t* value);

/**
 * \brief Тип функции, используемой для записи булевого параметра.
 * \ingroup variant_definitions
 * \param[in] param Указатель на структуру параметра.
 * \param[in] value Значение.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
typedef usd_error_code_t(*usd_set_bool_parameter_fp)(usd_boolf_param_tp param, uint8_t value);

/**
 * \brief Перечисление типов для \c variant.
 * \ingroup variant_definitions
 */
typedef enum {
    /**
     * \brief Мантисса \c int32 и экспонента \c uint8.
     */
    USD_VTYPE_DEC32 = 0,
    /**
     * \brief Указатель на функцию чтения значений \c dec32.
     */
    USD_VTYPE_DEC32F,
    /**
     * \brief Мантисса \c int64 и экспонента \c uint8.
     */
    USD_VTYPE_DEC64,
    /**
     * \brief Указатель на функцию чтения значений \c dec64.
     */
    USD_VTYPE_DEC64F,
    /**
     * \brief Булево значение типа \c uint8.
     */
    USD_VTYPE_BOOL,
    /**
     * \brief Указатель на функцию чтения булевого значения.
     */
    USD_VTYPE_BOOLF
} usd_variant_type_t;

/**
 * \brief Структура числа, представленного мантиссой \c int32 и степенью \c uint8.
 * \ingroup variant_definitions
 */
typedef struct {
    /**
     * \brief Мантисса числа.
     */
    int32_t value;
    /**
     * \brief Экспонента числа.
     */
    uint8_t exponent;
} usd_dec32_param_t;

/**
 * \brief Структура функций доступа к числу \c dec32.
 * \ingroup variant_definitions
 */
struct usd_dec32f_param {
    /**
     * \brief Функция чтения значения.
     */
    usd_get_dec32_parameter_fp get;
    /**
     * \brief Функция записи значения.
     */
    usd_set_dec32_parameter_fp set;
    /**
     * \brief Связанный объект.
     */
    void* object;
    /**
     * \brief Значение параметра.
     */
    int32_t value;
    /**
     * \brief Экспонента.
     */
    uint8_t exponent;
    /**
     * \brief Дополнительный параметр.
     */
    size_t eparam;
};

/**
 * \brief Структура числа, представленного мантиссой \c int64 и степенью \c uint8.
 * \ingroup variant_definitions
 */
typedef struct {
    /**
     * \brief Мантисса числа.
     */
    int64_t value;
    /**
     * \brief Экспонента числа.
     */
    uint8_t exponent;
} usd_dec64_param_t;

/**
 * \brief Структура функций доступа к числу \c dec32.
 * \ingroup variant_definitions
 */
struct usd_dec64f_param {
    /**
     * \brief Функция чтения значения.
     */
    usd_get_dec64_parameter_fp get;
    /**
     * \brief Функция записи значения.
     */
    usd_set_dec64_parameter_fp set;
    /**
     * \brief Связанный объект.
     */
    void* object;
    /**
     * \brief Значение параметра.
     */
    int64_t value;
    /**
     * \brief Экспонента.
     */
    uint8_t exponent;
    /**
     * \brief Дополнительный параметр.
     */
    size_t eparam;
};

/**
 * \brief Структура булевого числа.
 * \ingroup variant_definitions
 */
typedef struct {
    /**
     * \brief Значение.
     */
    uint8_t value;
} usd_bool_param_t;

/**
 * \brief Структура функций доступа к булевому числу.
 * \ingroup variant_definitions
 */
struct usd_boolf_param {
    /**
     * \brief Функция чтения значения.
     */
    usd_get_bool_parameter_fp get;
    /**
     * \brief Функция записи значения.
     */
    usd_set_bool_parameter_fp set;
    /**
     * \brief Связанный объект.
     */
    void* object;
    /**
     * \brief Значение параметра.
     */
    uint8_t value;
    /**
     * \brief Дополнительный параметр.
     */
    size_t eparam;
};

/**
 * \brief Объединение допустимых типов для \c variant.
 * \ingroup variant_definitions
 */
typedef union {
    /**
     * \brief Тип <code>\link USD_VTYPE_DEC32 \endlink</code>.
     */
    usd_dec32_param_t dec32;
    /**
     * \brief Тип <code>\link USD_VTYPE_DEC32F \endlink</code>.
     */
    usd_dec32f_param_t dec32f;
    /**
     * \brief Тип <code>\link USD_VTYPE_DEC64 \endlink</code>.
     */
    usd_dec64_param_t dec64;
    /**
     * \brief Тип <code>\link USD_VTYPE_DEC64F \endlink</code>.
     */
    usd_dec64f_param_t dec64f;
    /**
     * \brief Тип <code>\link USD_VTYPE_BOOL \endlink</code>.
     */
    usd_bool_param_t bool;
    /**
     * \brief Тип <code>\link USD_VTYPE_BOOLF \endlink</code>.
     */
    usd_boolf_param_t boolf;
} usd_variant_t;

#endif // USD_VARIANT_H_INCLUDED
