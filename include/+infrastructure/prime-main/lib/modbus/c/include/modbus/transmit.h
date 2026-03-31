#ifndef MODBUS_TRANSMIT_H_INCLUDED
#define MODBUS_TRANSMIT_H_INCLUDED

/**
 * \file transmit.h
 * \brief Файл содержит функции для приёма/передачи пакетов.
 *
 * Все функции являются внутренними и не должны вызываться пользователем.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "protocol.h"

/**
 * \brief Перечисление статусов приёма/передачи данных.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Нет данных для передачи.
     */
    MBS_TRANSMIT_IDLE = 0,
    /**
     * \brief Приём или передача завершены.
     */
    MBS_TRANSMIT_OK,
    /**
     * \brief Были переданы не все данные.
     */
    MBS_TRANSMIT_PENDING,
    /**
     * \brief Возникла ошибка во время приёма/передачи.
     */
    MBS_TRANSMIT_ERROR
} mbs_transmit_status_t;

/**
 * \brief Структура уровня приёма/передачи пакетов.
 */
struct mbs_buffer {
    /**
     * \brief Указатель на функцию проверки размера кадра.
     */
    mbs_get_pdu_length_fp get_pdu_length;
    /**
     * \brief Указатель на функцию проверки кадра.
     */
    mbs_check_frame_fp check_frame;
    /**
     * \brief Буффер приёма/передачи.
     *
     * Имеет фиксированный размер <code>\link MODBUS_BUFFER_SIZE \endlink</code>.
     */
    mbs_byte_t data[MODBUS_BUFFER_SIZE];
    /**
     * \brief Текущий размер буфера данных.
     */
    mbs_size_t length;
    /**
     * \brief Количество переданных байт данных.
     */
    mbs_size_t complete;
    /**
     * \brief Время начала приёма или отправки.
     */
    mbs_time_t time;
};

/**
 * \brief Инициализация буфера приёма/передачи.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c mbs_buffer.
 * \param[in] get_pdu_length Указатель на функцию проверки размера PDU.
 * \param[in] check_frame Указатель на функцию проверки кадра.
 */
void mbs_init_buffer(mbs_buffer_tp buffer, mbs_get_pdu_length_fp get_pdu_length, mbs_check_frame_fp check_frame);

/**
 * \brief Очистка буфера приёма/передачи.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c mbs_buffer.
 */
void mbs_clear_buffer(mbs_buffer_tp buffer);

/**
 * \brief Сброс счётчика переданных данных.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c mbs_buffer.
 */
void mbs_flush_buffer(mbs_buffer_tp buffer);

/**
 * \brief Приём полного сообщения.
 *
 * Возвращает результат операции и код ошибки, если был передан указатель.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c mbs_buffer.
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c mbs_endpoint.
 * \param[in] time Текущее время в системе.
 * \param[out] error Указатель на переменную, куда будет записан код ошибки. Опционально.
 * \return Результат выполнения операции <code>\link mbs_transmit_status_t \endlink</code>.
 */
mbs_transmit_status_t mbs_receive(mbs_buffer_tp buffer, mbs_endpoint_tp endpoint, mbs_time_t time, mbs_error_code_t* error);

/**
 * \brief Отправка сообщения.
 *
 * Возвращает результат операции и код ошибки, если был передан указатель.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c mbs_buffer.
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c mbs_endpoint.
 * \param[in] time Текущее время в системе.
 * \param[out] error Указатель на переменную, куда будет записан код ошибки. Опционально.
 * \return Результат выполнения операции <code>\link mbs_transmit_status_t \endlink</code>.
 */
mbs_transmit_status_t mbs_transmit(mbs_buffer_tp buffer, mbs_endpoint_tp endpoint, mbs_time_t time, mbs_error_code_t* error);

/**
 * \brief Есть ли в буфере непереданная информация.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c mbs_buffer.
 * \return 1 - есть, 0 - нет.
 */
uint8_t mbs_transmit_is_ready(mbs_buffer_tp buffer);

/**
 * \brief Копирование и удаление данных из буфера приёма.
 *
 * Оставшиеся в буфере данные смещаются в начало. Буфер для выходных данных должен иметь подходящий размер.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c mbs_buffer.
 * \param[out] data Буфер для записи данных. Опционально.
 * \param[out] size Количество читаемых байт данных.
 * \return Количество принятых байт данных.
 */
mbs_size_t mbs_receive_bytes(mbs_buffer_tp buffer, mbs_byte_t* data, mbs_size_t size);

#endif // MODBUS_TRANSMIT_H_INCLUDED
