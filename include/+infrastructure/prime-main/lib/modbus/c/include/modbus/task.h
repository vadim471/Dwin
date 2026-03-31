#ifndef MODBUS_TASK_H_INCLUDED
#define MODBUS_TASK_H_INCLUDED

/**
 * \file task.h
 * \brief Файл содержит функции для создания задач.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "list.h"

/**
 * \brief Добавить задачу на чтение дискретных выводов (0x01).
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] repeat_check Функция проверки состояния задачи.
 * \param[in] repeat_reset Функция сброса состояния задачи.
 * \param[in] repeat_object Указатель на структуру задачи.
 * \param[in] priority Приоритет задачи (0-255).
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_read_coils(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_read_coils_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на чтение дискретных выводов (0x01).
 *
 * Задача выполнится один раз, затем будет удалена.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_read_coils_once(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_master_read_coils_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на чтение дискретных выводов (0x01).
 *
 * Задача будет выполняться циклически с задержкой \c time.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] time Период автоповтора.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_read_coils_loop(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_time_t time,
    mbs_master_read_coils_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на чтение дискретных вводов (0x02).
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого ввода.
 * \param[in] count Количество вводов.
 * \param[in] repeat_check Функция проверки состояния задачи.
 * \param[in] repeat_reset Функция сброса состояния задачи.
 * \param[in] repeat_object Указатель на структуру задачи.
 * \param[in] priority Приоритет задачи (0-255).
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_read_discrete_inputs(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_read_discrete_inputs_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на чтение дискретных вводов (0x02).
 *
 * Задача выполнится один раз, затем будет удалена.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого ввода.
 * \param[in] count Количество вводов.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_read_discrete_inputs_once(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_master_read_discrete_inputs_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на чтение дискретных вводов (0x02).
 *
 * Задача будет выполняться циклически с задержкой \c time.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого ввода.
 * \param[in] count Количество вводов.
 * \param[in] time Период автоповтора.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_read_discrete_inputs_loop(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_time_t time,
    mbs_master_read_discrete_inputs_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на чтение аналоговых выводов (0x03).
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] repeat_check Функция проверки состояния задачи.
 * \param[in] repeat_reset Функция сброса состояния задачи.
 * \param[in] repeat_object Указатель на структуру задачи.
 * \param[in] priority Приоритет задачи (0-255).
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_read_holding_registers(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_read_holding_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на чтение аналоговых выводов (0x03).
 *
 * Задача выполнится один раз, затем будет удалена.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_read_holding_registers_once(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_master_read_holding_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на чтение аналоговых выводов (0x03).
 *
 * Задача будет выполняться циклически с задержкой \c time.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] time Период автоповтора.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_read_holding_registers_loop(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_time_t time,
    mbs_master_read_holding_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на чтение аналоговых вводов (0x04).
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого ввода.
 * \param[in] count Количество вводов.
 * \param[in] repeat_check Функция проверки состояния задачи.
 * \param[in] repeat_reset Функция сброса состояния задачи.
 * \param[in] repeat_object Указатель на структуру задачи.
 * \param[in] priority Приоритет задачи (0-255).
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_read_input_registers(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_read_input_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на чтение аналоговых вводов (0x04).
 *
 * Задача выполнится один раз, затем будет удалена.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого ввода.
 * \param[in] count Количество вводов.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_read_input_registers_once(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_master_read_input_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на чтение аналоговых вводов (0x04).
 *
 * Задача будет выполняться циклически с задержкой \c time.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого ввода.
 * \param[in] count Количество вводов.
 * \param[in] time Период автоповтора.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_read_input_registers_loop(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_time_t time,
    mbs_master_read_input_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на чтение аналоговых вводов (0x04).
 *
 * Задача будет выполняться по внешнему условию.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого ввода.
 * \param[in] count Количество вводов.
 * \param[in] trigger Указатель на отслеживаемую переменную.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_read_input_registers_manual(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    uint8_t* trigger,
    mbs_master_read_input_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на запись дискретного вывода (0x05).
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] repeat_check Функция проверки состояния задачи.
 * \param[in] repeat_reset Функция сброса состояния задачи.
 * \param[in] repeat_object Указатель на структуру задачи.
 * \param[in] priority Приоритет задачи (0-255).
 * \param[in] on_request Функция записи значения.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_write_single_coil(
    mbs_device_tp device,
    uint16_t address,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_write_single_coil_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на запись дискретного вывода (0x05).
 *
 * Задача выполнится один раз, затем будет удалена.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] on_request Функция записи значения.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_write_single_coil_once(
    mbs_device_tp device,
    uint16_t address,
    mbs_master_write_single_coil_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на запись дискретного вывода (0x05).
 *
 * Задача будет выполняться циклически с задержкой \c time.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] time Период автоповтора.
 * \param[in] on_request Функция записи значения.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_write_single_coil_loop(
    mbs_device_tp device,
    uint16_t address,
    mbs_time_t time,
    mbs_master_write_single_coil_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на запись аналогового вывода (0x06).
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] repeat_check Функция проверки состояния задачи.
 * \param[in] repeat_reset Функция сброса состояния задачи.
 * \param[in] repeat_object Указатель на структуру задачи.
 * \param[in] priority Приоритет задачи (0-255).
 * \param[in] on_request Функция записи значения.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_write_single_register(
    mbs_device_tp device,
    uint16_t address,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_write_single_register_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на запись аналогового вывода (0x06).
 *
 * Задача выполнится один раз, затем будет удалена.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] on_request Функция записи значения.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_write_single_register_once(
    mbs_device_tp device,
    uint16_t address,
    mbs_master_write_single_register_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на запись аналогового вывода (0x06).
 *
 * Задача будет выполняться циклически с задержкой \c time.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] time Период автоповтора.
 * \param[in] on_request Функция записи значения.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_write_single_register_loop(
    mbs_device_tp device,
    uint16_t address,
    mbs_time_t time,
    mbs_master_write_single_register_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на запись аналогового вывода (0x06).
 *
 * Задача будет выполняться по внешнему условию.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] trigger Указатель на отслеживаемую переменную.
 * \param[in] on_request Функция записи значения.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_write_single_register_manual(
    mbs_device_tp device,
    uint16_t address,
    uint8_t* trigger,
    mbs_master_write_single_register_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на запись дискретных выводов (0x0F).
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] repeat_check Функция проверки состояния задачи.
 * \param[in] repeat_reset Функция сброса состояния задачи.
 * \param[in] repeat_object Указатель на структуру задачи.
 * \param[in] priority Приоритет задачи (0-255).
 * \param[in] on_request Функция записи значений.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_write_multiple_coils(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_write_multiple_coils_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на запись дискретных выводов (0x0F).
 *
 * Задача выполнится один раз, затем будет удалена.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] on_request Функция записи значений.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_write_multiple_coils_once(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_master_write_multiple_coils_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на запись дискретных выводов (0x0F).
 *
 * Задача будет выполняться циклически с задержкой \c time.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] time Период автоповтора.
 * \param[in] on_request Функция записи значений.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_write_multiple_coils_loop(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_time_t time,
    mbs_master_write_multiple_coils_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на запись аналоговых выводов (0x10).
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] repeat_check Функция проверки состояния задачи.
 * \param[in] repeat_reset Функция сброса состояния задачи.
 * \param[in] repeat_object Указатель на структуру задачи.
 * \param[in] priority Приоритет задачи (0-255).
 * \param[in] on_request Функция записи значений.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_write_multiple_registers(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_write_multiple_registers_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на запись аналоговых выводов (0x10).
 *
 * Задача выполнится один раз, затем будет удалена.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] on_request Функция записи значений.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_write_multiple_registers_once(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_master_write_multiple_registers_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Добавить задачу на запись аналоговых выводов (0x10).
 *
 * Задача будет выполняться циклически с задержкой \c time.
 * \ingroup master_functions
 * \param[in] device Указатель на структуру ведомого устройства \c mbs_device.
 * \param[in] address Адрес первого вывода.
 * \param[in] count Количество выводов.
 * \param[in] time Период автоповтора.
 * \param[in] on_request Функция записи значений.
 * \param[in] on_response Обработчик результата операции.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_write_multiple_registers_loop(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_time_t time,
    mbs_master_write_multiple_registers_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

#endif // MODBUS_TASK_H_INCLUDED
