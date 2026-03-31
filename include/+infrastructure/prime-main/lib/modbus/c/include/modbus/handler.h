#ifndef MODBUS_HANDLER_H_INCLUDED
#define MODBUS_HANDLER_H_INCLUDED

/**
 * \file handler.h
 * \brief Файл содержит функции для создания обработчиков команд.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "callback.h"

/**
 * \brief Добавить обработчик чтения дискретных выводов (0x01).
 * \ingroup slave_functions
 * \param[in] slave Указатель на структуру ведомого устройства \c mbs_slave.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] on_request Обработчик запроса.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_on_read_coils(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    mbs_slave_read_coils_fp on_request
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_request_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить обработчик чтения дискретных вводов (0x02).
 * \ingroup slave_functions
 * \param[in] slave Указатель на структуру ведомого устройства \c mbs_slave.
 * \param[in] address Адрес первого ввода.
 * \param[in] count Количество вводов.
 * \param[in] on_request Обработчик запроса.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_on_read_discrete_inputs(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    mbs_slave_read_discrete_inputs_fp on_request
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_request_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить обработчик чтения аналоговых выводов (0x03).
 * \ingroup slave_functions
 * \param[in] slave Указатель на структуру ведомого устройства \c mbs_slave.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] on_request Обработчик запроса.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_on_read_holding_registers(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    mbs_slave_read_holding_registers_fp on_request
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_request_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить обработчик чтения аналоговых вводов (0x04).
 * \ingroup slave_functions
 * \param[in] slave Указатель на структуру ведомого устройства \c mbs_slave.
 * \param[in] address Адрес первого ввода.
 * \param[in] count Количество вводов.
 * \param[in] on_request Обработчик запроса.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_on_read_input_registers(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    mbs_slave_read_input_registers_fp on_request
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_request_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить обработчик записи дискретных выводов (0x0F).
 * \ingroup slave_functions
 * \param[in] slave Указатель на структуру ведомого устройства \c mbs_slave.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] on_request Функция записи значений.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_on_write_multiple_coils(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    mbs_slave_write_multiple_coils_fp on_request
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_request_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить обработчик записи аналоговых выводов (0x10).
 * \ingroup slave_functions
 * \param[in] slave Указатель на структуру ведомого устройства \c mbs_slave.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] on_request Функция записи значений.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_on_write_multiple_registers(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    mbs_slave_write_multiple_registers_fp on_request
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_request_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

#endif // MODBUS_HANDLER_H_INCLUDED
