#ifndef MODBUS_SLAVE_H_INCLUDED
#define MODBUS_SLAVE_H_INCLUDED

/**
 * \file slave.h
 * \brief Файл содержит функции для работы с подчинённым устройством.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "symbol.h"
#include "endpoint.h"
#include "transmit.h"
#include "handler.h"
#include "range.h"

/**
 * \brief Структура подчинённого устройства.
 */
struct mbs_slave {
    /**
     * \brief Идентификатор устройства.
     */
    uint8_t id;
    /**
     * \brief Состояние линии передачи данных.
     */
    mbs_line_status_t line;
    /**
     * \brief Указатель на интерфейс передачи данных.
     */
    mbs_endpoint_tp endpoint;
    /**
     * \brief Список диапазонов обработчиков команд.
     */
    mbs_ranges_list_t ranges;
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
};

/**
 * \brief Инициализация структуры подчинённого устройства RTU.
 * \ingroup public_functions
 * \param[in] slave Указатель на структуру подчинённого устройства \c mbs_slave.
 * \param[in] id Идентификатор устройства.
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c mbs_endpoint.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_init_slave_rtu(mbs_slave_tp slave, uint8_t id, mbs_endpoint_tp endpoint);

/**
 * \brief Освобождение памяти, занятой структурой подчинённого устройства.
 *
 * После этой операции структура может быть использована повторно.
 * \ingroup public_functions
 * \param[in] slave Указатель на структуру подчинённого устройства \c mbs_slave.
 */
void mbs_dispose_slave(mbs_slave_tp slave);

/**
 * \brief Освобождение памяти, занятой структурой подчинённого устройства.
 * \ingroup public_functions
 * \param[in] slave Указатель на структуру подчинённого устройства \c mbs_slave.
 */
void mbs_free_slave(mbs_slave_tp slave);

/**
 * \brief Опросить подчинённое устройство.
 * \ingroup public_functions
 * \param[in] slave Указатель на структуру подчинённого устройства \c mbs_slave.
 * \param[in] time Текущее время в системе (учитывается только дельта времени).
 * \return Время до ближайшего опроса устройств.
 */
void mbs_poll_slave(mbs_slave_tp slave, mbs_time_t time);

#endif // MODBUS_SLAVE_H_INCLUDED
