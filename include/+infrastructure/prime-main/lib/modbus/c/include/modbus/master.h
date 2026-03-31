#ifndef MODBUS_MASTER_H_INCLUDED
#define MODBUS_MASTER_H_INCLUDED

/**
 * \file master.h
 * \brief Файл содержит функции для работы с ведущим устройством.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "symbol.h"
#include "device.h"
#include "endpoint.h"
#include "transmit.h"
#include "queue.h"
#include "task.h"

/**
 * \brief Структура ведущего устройства.
 */
struct mbs_master {
    /**
     * \brief Массив указателей на подчинённые устройства.
     */
    mbs_device_tp* devices;
    /**
     * \brief Размер массива подчинённых устройств.
     */
    uint8_t devices_size;
    /**
     * \brief Количество подчинённых устройств в массиве.
     */
    uint8_t devices_count;
    /**
     * \brief Состояние линии передачи данных.
     */
    mbs_line_status_t line;
    /**
     * \brief Указатель на интерфейс передачи данных.
     */
    mbs_endpoint_tp endpoint;
    /**
     * \brief Очередь запросов, ожидающих исполнения.
     */
    mbs_requests_queue_t queue;
    /**
     * \brief Функция извлечения кадра из буфера.
     */
    mbs_extract_frame_fp extract;
    /**
     * \brief Функция упаковки кадра в буфер.
     */
    mbs_serialize_frame_fp serialize;
    /**
     * \brief Буфер для принимаемых данных.
     */
    mbs_buffer_t buffer;
    /**
     * \brief Время последнего отправленного запроса.
     */
    mbs_time_t time;
    /**
     * \brief Время последнего сообщения о таймауте.
     */
    mbs_time_t debug_time;
};

/**
 * \brief Инициализация структуры ведущего узла RTU.
 * \ingroup public_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c mbs_endpoint.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_init_master_rtu(mbs_master_tp master, mbs_endpoint_tp endpoint);

/**
 * \brief Увеличить размер массива устройств.
 *
 * При добавлении устройств указатели будут записываться в заранее выделенную память.
 * \ingroup public_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] size Количество дополнительных слотов для устройств.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_expand_master(mbs_master_tp master, uint8_t size);

/**
 * \brief Создать новое устройство и добавить в структуру ведущего узла.
 *
 * Память для структуры устройства выделяется динамически путём вызова функции <code>\link mbs_malloc \endlink</code>.
 * Если память для массива не была выделена заранее, то выделяется аналогично.
 * \ingroup public_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] address Сетевой адрес устройства.
 * \param[out] device Указатель на память, куда будет записан возвращаемый указатель на \c mbs_device.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_create_device(mbs_master_tp master, uint8_t address, mbs_device_tp* device);

/**
 * \brief Добавить существующее устройство в структуру ведущего узла.
 *
 * Если память для массива не была выделена заранее, то происходит вызов функции <code>\link mbs_malloc \endlink</code>.
 * \ingroup public_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру устройств \c mbs_device.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_add_device(mbs_master_tp master, mbs_device_tp device);

/**
 * \brief Поиск устройства с указанным сетевым адресом.
 * \ingroup public_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] address Сетевой адрес устройства.
 * \return Указатель на структуру устройства \c mbs_device.
 */
mbs_device_tp mbs_get_device(mbs_master_tp master, uint8_t address);

/**
 * \brief Освобождение памяти, занятой структурой ведущего узла.
 *
 * После этой операции структура может быть использована повторно.
 * \ingroup public_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 */
void mbs_dispose_master(mbs_master_tp master);

/**
 * \brief Освобождение памяти, занятой структурой ведущего узла.
 * \ingroup public_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 */
void mbs_free_master(mbs_master_tp master);

/**
 * \brief Опросить ведущий узел.
 * \ingroup public_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] time Текущее время в системе (учитывается только дельта времени).
 * \return Время до ближайшего опроса устройств.
 */
void mbs_poll_master(mbs_master_tp master, mbs_time_t time);

#endif // MODBUS_MASTER_H_INCLUDED
