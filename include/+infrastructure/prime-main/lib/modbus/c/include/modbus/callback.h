#ifndef MODBUS_CALLBACK_H_INCLUDED
#define MODBUS_CALLBACK_H_INCLUDED

/**
 * \file callback.h
 * \brief Файл содержит типы функций обратной связи.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"

#ifdef MODBUS_EXTEND_CALLBACKS
#include <stddef.h>
#endif // MODBUS_EXTEND_CALLBACKS

#ifdef MODBUS_EXTEND_CALLBACKS
void mbs_free_request_eparam(size_t address);
void mbs_free_response_eparam(size_t address);
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию обработки результата записи.
 * \ingroup callback_definitions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] error Код ошибки \c Modbus.
 */
typedef void(*mbs_master_result_fp)(
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_byte_t error
);
#ifdef MODBUS_EXTEND_CALLBACKS
typedef void(mbs_master_result_f)(mbs_master_tp, mbs_device_tp, mbs_byte_t);
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию чтения дискретных выводов (0x01).
 * \ingroup callback_definitions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] error Код ошибки \c Modbus.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] coils Указатель на массив значений.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
typedef mbs_error_code_t(*mbs_master_read_coils_fp)(
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_byte_t error,
    uint16_t address,
    uint16_t count,
    const uint8_t* coils
);
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_error_code_t(mbs_master_read_coils_f)(mbs_master_tp, mbs_device_tp, mbs_byte_t, uint16_t, uint16_t, const uint8_t*);
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию чтения дискретных вводов (0x02).
 *
 * См. <code>\link mbs_master_read_coils_fp \endlink</code>.
 * \ingroup callback_definitions
 */
typedef mbs_master_read_coils_fp mbs_master_read_discrete_inputs_fp;
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_master_read_coils_f mbs_master_read_discrete_inputs_f;
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию чтения аналоговых выводов (0x03).
 * \ingroup callback_definitions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] error Код ошибки \c Modbus.
 * \param[in] address Адрес первого регистра.
 * \param[in] count Количество регистров.
 * \param[in] registers Указатель на массив регистров.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
typedef mbs_error_code_t(*mbs_master_read_holding_registers_fp)(
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_byte_t error,
    uint16_t address,
    uint16_t count,
    const uint16_t* registers
);
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_error_code_t(mbs_master_read_holding_registers_f)(mbs_master_tp, mbs_device_tp, mbs_byte_t, uint16_t, uint16_t, const uint16_t*);
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию чтения аналоговых вводов (0x04).
 *
 * См. <code>\link mbs_master_read_holding_registers_fp \endlink</code>.
 * \ingroup callback_definitions
 */
typedef mbs_master_read_holding_registers_fp mbs_master_read_input_registers_fp;
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_master_read_holding_registers_f mbs_master_read_input_registers_f;
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию записи дискретного вывода (0x05).
 * \ingroup callback_definitions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес вывода.
 * \param[in] value Значение вывода (0 или 1).
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
typedef mbs_error_code_t(*mbs_master_write_single_coil_fp)(
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_tp master,
    mbs_device_tp device,
    uint16_t address,
    uint8_t* value
);
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_error_code_t(mbs_master_write_single_coil_f)(mbs_master_tp, mbs_device_tp, uint16_t, uint8_t*);
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию записи аналогового вывода (0x06).
 * \ingroup callback_definitions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес вывода.
 * \param[in] value Значение вывода.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
typedef mbs_error_code_t(*mbs_master_write_single_register_fp)(
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_tp master,
    mbs_device_tp device,
    uint16_t address,
    uint16_t* value
);
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_error_code_t(mbs_master_write_single_register_f)(mbs_master_tp, mbs_device_tp, uint16_t, uint16_t*);
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию записи дискретных выводов (0x0F).
 * \ingroup callback_definitions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] values Указатель на массив, куда следует записать значения 0 или 1.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
typedef mbs_error_code_t(*mbs_master_write_multiple_coils_fp)(
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_tp master,
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    uint8_t* values
);
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_error_code_t(mbs_master_write_multiple_coils_f)(mbs_master_tp, mbs_device_tp, uint16_t, uint16_t, uint8_t*);
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию записи аналоговых выводов (0x10).
 * \ingroup callback_definitions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] values Указатель на массив, куда следует записать значения.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
typedef mbs_error_code_t(*mbs_master_write_multiple_registers_fp)(
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_tp master,
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    uint16_t* values
);
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_error_code_t(mbs_master_write_multiple_registers_f)(mbs_master_tp, mbs_device_tp, uint16_t, uint16_t, uint16_t*);
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию чтения дискретных выводов (0x01).
 * \ingroup callback_definitions
 * \param[in] slave Указатель на структуру подчинённого устройства \c mbs_slave.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] coils Указатель на массив для записи значений.
 * \return Код ошибки \c Modbus.
 */
typedef mbs_byte_t(*mbs_slave_read_coils_fp)(
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    uint8_t* coils
);
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_byte_t(mbs_slave_read_coils_f)(mbs_slave_tp, uint16_t, uint16_t, uint8_t*);
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию чтения дискретных вводов (0x02).
 *
 * См. <code>\link mbs_slave_read_coils_fp \endlink</code>.
 * \ingroup callback_definitions
 */
typedef mbs_slave_read_coils_fp mbs_slave_read_discrete_inputs_fp;
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_slave_read_coils_f mbs_slave_read_discrete_inputs_f;
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию чтения аналоговых выводов (0x03).
 * \ingroup callback_definitions
 * \param[in] slave Указатель на структуру подчинённого устройства \c mbs_slave.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] registers Указатель на массив для записи значений регистров.
 * \return Код ошибки \c Modbus.
 */
typedef mbs_byte_t(*mbs_slave_read_holding_registers_fp)(
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    uint16_t* registers
);
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_byte_t(mbs_slave_read_holding_registers_f)(mbs_slave_tp, uint16_t, uint16_t, uint16_t*);
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию чтения аналоговых вводов (0x04).
 *
 * См. <code>\link mbs_slave_read_holding_registers_fp \endlink</code>.
 * \ingroup callback_definitions
 */
typedef mbs_slave_read_holding_registers_fp mbs_slave_read_input_registers_fp;
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_slave_read_holding_registers_f mbs_slave_read_input_registers_f;
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию записи дискретных выводов (0x0F).
 * \ingroup callback_definitions
 * \param[in] slave Указатель на структуру подчинённого устройства \c mbs_slave.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] values Указатель на массив значений (0 или 1).
 * \return Код ошибки \c Modbus.
 */
typedef mbs_byte_t(*mbs_slave_write_multiple_coils_fp)(
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    const uint8_t* values
);
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_byte_t(mbs_slave_write_multiple_coils_f)(mbs_slave_tp, uint16_t, uint16_t, const uint8_t*);
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию записи аналоговых выводов (0x10).
 * \ingroup callback_definitions
 * \param[in] slave Указатель на структуру подчинённого устройства \c mbs_slave.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] values Указатель на массив, куда следует записать значения.
 * \return Код ошибки \c Modbus.
 */
typedef mbs_byte_t(*mbs_slave_write_multiple_registers_fp)(
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    const uint16_t* values
);
#ifdef MODBUS_EXTEND_CALLBACKS
typedef mbs_byte_t(mbs_slave_write_multiple_registers_f)(mbs_slave_tp, uint16_t, uint16_t, const uint16_t*);
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Указатель на функцию сравнения задач.
 * \ingroup callback_definitions
 * \param[in] task Указатель на задачу \c mbs_task.
 * \return Если задача удовлетворяет условию, то 1, иначе 0.
 */
typedef uint8_t(*mbs_task_condition_fp)(
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_task_tp task
);
#ifdef MODBUS_EXTEND_CALLBACKS
typedef uint8_t(mbs_task_condition_fp_f)(mbs_task_tp);
#endif // MODBUS_EXTEND_CALLBACKS

#endif // MODBUS_CALLBACK_H_INCLUDED
