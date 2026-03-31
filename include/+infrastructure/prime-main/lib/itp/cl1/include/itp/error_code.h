#ifndef ITP_CL1_ERROR_CODE_H_INCLUDED
#define ITP_CL1_ERROR_CODE_H_INCLUDED

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
    CL1_ERR_INTERNAL = 0x0101,
    /**
     * \brief \c <b>0x0102</b> Ошибка при инициализации интерфейса передачи данных.
     */
    CL1_ERR_ENDPOINT_INIT_FAILED = 0x0102,
    /**
     * \brief \c <b>0x0103</b> Неизвестный тип устройства.
     */
    CL1_ERR_UNKNOWN_DEVICE_TYPE = 0x0103,
    /**
     * \brief \c <b>0x0104</b> Неверный адрес устройства.
     */
    CL1_ERR_WRONG_ADDRESS = 0x0104,
    /**
     * \brief \c <b>0x0105</b> Неверное состояние устройства.
     */
    CL1_ERR_WRONG_STATE = 0x0105,
    /**
     * \brief \c <b>0x0106</b> Команда не поддерживается устройством.
     */
    CL1_ERR_NOT_IMPLEMENTED = 0x0106,
    /**
     * \brief \c <b>0x0107</b> Ошибка в конфигурации устройства.
     */
    CL1_ERR_BAD_CONFIGURATION = 0x0107,

    /**
     * \brief \c <b>0x0201</b> Ошибка при работе с ТРК.
     */
    CL1_ERR_DSP_INTERNAL = 0x0201,
    /**
     * \brief \c <b>0x0202</b> Неверный тип налива.
     */
    CL1_ERR_DSP_WRONG_FUELLING_TYPE = 0x0202,
    /**
     * \brief \c <b>0x0203</b> Неверный идентификатор продукта (или номер рукава).
     */
    CL1_ERR_DSP_WRONG_PRODUCT = 0x0203,
    /**
     * \brief \c <b>0x0204</b> Задан неверный параметр.
     */
    CL1_ERR_DSP_WRONG_PARAMETER = 0x0204,

    /**
     * \brief \c <b>0x0301</b> Ошибка при работе с универсальным датчиком.
     */
    CL1_ERR_USD_INTERNAL = 0x0301,
    /**
     * \brief \c <b>0x0302</b> Неверный тип датчика.
     */
    CL1_ERR_USD_WRONG_TYPE = 0x0302,
    /**
     * \brief \c <b>0x0303</b> Параметр не найден.
     */
    CL1_ERR_USD_PARAMETER_NOT_FOUND = 0x0303,

    /**
     * \brief \c <b>0x0401</b> Ошибка при работе с ценовым табло.
     */
    CL1_ERR_LED_INTERNAL = 0x0401

#ifdef ITP_COMPILE_CXX
} cl1_error_t;
}
#else // ITP_COMPILE_CXX
} itp_cl1_error_t;
#endif // ITP_COMPILE_CXX

#endif // ITP_CL1_ERROR_CODE_H_INCLUDED
