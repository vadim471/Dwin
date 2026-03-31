#ifndef MODBUS_DEVICE_H_INCLUDED
#define MODBUS_DEVICE_H_INCLUDED

/**
 * \file device.h
 * \brief Файл содержит функции для работы с подчинённым устройством.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "callback.h"
#include "list.h"

/**
 * \brief Структура ведомого устройства.
 */
struct mbs_device {
    /**
     * \brief Сетевой адрес устройства.
     */
    uint8_t address;
    /**
     * \brief Список задач.
     */
    mbs_tasks_list_t tasks;
};

/**
 * \brief Инициализация структуры устройства.
 * \ingroup public_functions
 * \param[in] device Указатель на структуру устройства \c mbs_device.
 * \param[in] address Сетевой адрес устройства.
 */
void mbs_init_device(mbs_device_tp device, uint8_t address);

/**
 * \brief Освобождение памяти, занятой структурой устройства.
 * \ingroup private_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру устройства \c mbs_device.
 */
void mbs_free_device(mbs_master_tp master, mbs_device_tp device);

/**
 * \brief Отмена всех задач.
 * \ingroup private_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру устройства \c mbs_device.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_cancel_tasks(mbs_master_tp master, mbs_device_tp device);

/**
 * \brief Отмена всех задач, удовлетворяющих условию.
 * \ingroup private_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру устройства \c mbs_device.
 * \param[in] condition Указатель на функцию проверки условия.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_cancel_tasks_conditional(
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_task_condition_fp condition
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t condition_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
);

/**
 * \brief Активация всех задач.
 * \ingroup private_functions
 * \param[in] device Указатель на структуру устройства \c mbs_device.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_enable_tasks(mbs_device_tp device);

/**
 * \brief Сброс или удаление задачи (в случае ошибки).
 * \ingroup private_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру устройства \c mbs_device.
 * \param[in] task Указатель на задачу \c mbs_task.
 * \param[in] time Текущее время в системе.
 */
void mbs_reset_or_erase_task(mbs_master_tp master, mbs_device_tp device, mbs_task_tp task, mbs_time_t time);

/**
 * \brief Опрос устройства.
 * \ingroup private_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру устройства \c mbs_device.
 * \param[in] time Текущее время в системе.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_poll_device(mbs_master_tp master, mbs_device_tp device, mbs_time_t time);

#endif // MODBUS_DEVICE_H_INCLUDED
