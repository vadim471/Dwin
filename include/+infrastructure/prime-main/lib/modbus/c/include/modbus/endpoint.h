#ifndef MODBUS_ENDPOINT_H_INCLUDED
#define MODBUS_ENDPOINT_H_INCLUDED

/**
 * \file endpoint.h
 * \brief Файл содержит функции для работы с интерфейсом передачи данных.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"

/**
 * \brief Указатель на функцию чтения данных.
 *
 * Функция неблокирующая, если данные отсутствуют - возвращает 0 (ноль).
 * \ingroup endpoint_definition
 * \param[in] object Указатель на пользовательский объект.
 * \param[in] data Указатель на буфер, в который производится чтение.
 * \param[in] size Размер буфера данных.
 * \return Количество прочитанных байт данных.
 */
typedef mbs_size_t(*mbs_ep_read_fp)(void* object, mbs_byte_t* data, mbs_size_t size);

/**
 * \brief Указатель на функцию записи данных.
 *
 * Необязательно выполнять запись всех данных за одну итерацию.
 * При повторном вызове будет передан указатель на новую позицию в буфере данных.
 *
 * Данные для записи могут отсутствовать, в этом случае количество записываемых байт будет равняться 0 (нулю).
 * \ingroup endpoint_definition
 * \param[in] object Указатель на пользовательский объект.
 * \param[in] data Указатель на буфер, содержащий передаваемые данные.
 * \param[in] length Количество байт, ожидающих записи.
 * \return Количество переданных байт данных.
 */
typedef mbs_size_t(*mbs_ep_write_fp)(void* object, const mbs_byte_t* data, mbs_size_t length);

/**
 * \brief Структура интерфейса передачи данных.
 */
struct mbs_endpoint {
    /**
     * \brief Указатель на функцию чтения данных.
     */
    mbs_ep_read_fp read;
    /**
     * \brief Указатель на функцию записи данных.
     */
    mbs_ep_write_fp write;
    /**
     * \brief Указатель на пользовательский объект.
     */
    void* object;
    #if (MODBUS_WAIT_SWITCHING > 0)
    /**
     * \brief Время последнего приёма данных.
     *
     * Поле необходимо для обеспечения задержек приёма/передачи.
     */
    mbs_time_t time;
    #endif // (MODBUS_WAIT_SWITCHING > 0)
};

/**
 * \brief Инициализация структуры интерфейса передачи данных.
 * \ingroup public_functions
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c mbs_endpoint.
 * \param[in] read Указатель на функцию чтения данных.
 * \param[in] write Указатель на функцию записи данных.
 * \param[in] object Указатель на пользовательский объект.
 */
mbs_error_code_t mbs_init_endpoint(mbs_endpoint_tp endpoint, mbs_ep_read_fp read, mbs_ep_write_fp write, void* object);

/**
 * \brief Приём данных через физический интерфейс.
 * \ingroup private_functions
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c mbs_endpoint.
 * \param[in] time Текущее время в системе.
 * \param[out] data Указатель на буфер, в который производится чтение.
 * \param[in] size Размер буфера данных.
 * \return Количество прочитанных байт данных.
 */
mbs_size_t mbs_read_data(
    mbs_endpoint_tp endpoint,
    #if (MODBUS_WAIT_SWITCHING > 0)
    mbs_time_t time,
    #endif // (MODBUS_WAIT_SWITCHING > 0)
    mbs_byte_t* data,
    mbs_size_t size
);

/**
 * \brief Отправка данных через физический интерфейс.
 * \ingroup private_functions
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c mbs_endpoint.
 * \param[in] time Текущее время в системе.
 * \param[in] data Указатель на буфер, содержащий передаваемые данные.
 * \param[in] length Количество байт, ожидающих записи.
 * \return Количество переданных байт данных.
 */
mbs_size_t mbs_write_data(
    mbs_endpoint_tp endpoint,
    #if (MODBUS_WAIT_SWITCHING > 0)
    mbs_time_t time,
    #endif // (MODBUS_WAIT_SWITCHING > 0)
    mbs_byte_t* data,
    mbs_size_t length
);

#endif // MODBUS_ENDPOINT_H_INCLUDED
