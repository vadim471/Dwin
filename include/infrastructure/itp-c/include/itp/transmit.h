#ifndef ITP_TRANSMIT_H_INCLUDED
#define ITP_TRANSMIT_H_INCLUDED

/**
 * \file transmit.h
 * \brief Файл содержит функции для приёма/передачи пакетов.
 *
 * Все функции являются внутренними и не должны вызываться пользователем.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"

/**
 * \brief Перечисление статусов приёма/передачи данных.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Нет данных для передачи.
     */
    ITP_TRANSMIT_IDLE = 0,
    /**
     * \brief Приём или передача завершены.
     */
    ITP_TRANSMIT_OK,
    /**
     * \brief Были переданы не все данные.
     */
    ITP_TRANSMIT_PENDING,
    /**
     * \brief Возникла ошибка во время приёма/передачи.
     */
    ITP_TRANSMIT_ERROR
} itp_transmit_status_t;

/**
 * \brief Структура уровня приёма/передачи пакетов.
 */
struct itp_buffer {
    /**
     * \brief Буффер приёма/передачи.
     *
     * Имеет динамический размер от <code>\link ITP_MIN_BUFFER_SIZE \endlink</code> до <code>\link ITP_MAX_BUFFER_SIZE \endlink</code>.
     */
    itp_byte_t* data;
    /**
     * \brief Текущий размер буфера данных.
     */
    itp_size_t size;
    /**
     * \brief Количество байт данных в буфере.
     */
    itp_size_t length;
    /**
     * \brief Количество переданных байт данных.
     */
    itp_size_t complete;
    /**
     * \brief Время начала приёма или отправки.
     */
    itp_time_t time;
};

/**
 * \brief Инициализация буфера приёма/передачи.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_init_buffer(itp_buffer_tp buffer);

/**
 * \brief Очистка буфера приёма/передачи.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 */
void itp_clear_buffer(itp_buffer_tp buffer);

/**
 * \brief Сброс счётчика переданных данных.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 */
void itp_flush_buffer(itp_buffer_tp buffer);

/**
 * \brief Дамп буфера.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 */
void itp_dump_buffer(itp_buffer_tp buffer);

/**
 * \brief Удаление памяти, занятой полями буфера.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 */
void itp_dispose_buffer(itp_buffer_tp buffer);

/**
 * \brief Изменить размер буфера.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 * \param[in] size Новый размер буфера.
 * \param[in] preserve Необходимость сохранения данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_resize_buffer(itp_buffer_tp buffer, itp_size_t size, uint8_t preserve);

/**
 * \brief Приём полного сообщения.
 *
 * Возвращает результат операции и код ошибки, если был передан указатель.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c itp_endpoint.
 * \param[out] error Указатель на переменную, куда будет записан код ошибки. Опционально.
 * \return Результат выполнения операции <code>\link itp_transmit_status_t \endlink</code>.
 */
itp_transmit_status_t itp_receive(itp_buffer_tp buffer, itp_endpoint_tp endpoint, itp_error_code_t* error);

/**
 * \brief Копирование и удаление данных из буфера приёма.
 *
 * Оставшиеся в буфере данные смещаются в начало. Буфер для выходных данных должен иметь подходящий размер.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 * \param[out] data Буфер для записи данных. Опционально.
 * \param[out] size Количество читаемых байт данных.
 * \return Количество принятых байт данных.
 */
itp_size_t itp_receive_bytes(
#ifdef ITP_TRACE_BUFFERS
uint8_t root_address,
#endif // ITP_TRACE_BUFFERS
itp_buffer_tp buffer, itp_byte_t* data, itp_size_t size);

/**
 * \brief Есть ли в буфере непереданная информация.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 * \return 1 - есть, 0 - нет.
 */
uint8_t itp_transmit_is_ready(itp_buffer_tp buffer);

/**
 * \brief Отправка сообщения.
 *
 * Возвращает результат операции и код ошибки, если был передан указатель.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c itp_endpoint.
 * \param[out] error Указатель на переменную, куда будет записан код ошибки. Опционально.
 * \return Результат выполнения операции <code>\link itp_transmit_status_t \endlink</code>.
 */
itp_transmit_status_t itp_transmit(itp_buffer_tp buffer, itp_endpoint_tp endpoint, itp_error_code_t* error);

#endif // ITP_TRANSMIT_H_INCLUDED
