#ifndef ITP_ENDPOINT_H_INCLUDED
#define ITP_ENDPOINT_H_INCLUDED

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
typedef itp_size_t(*itp_ep_read_fp)(void* object, itp_byte_t* data, itp_size_t size);

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
typedef itp_size_t(*itp_ep_write_fp)(void* object, const itp_byte_t* data, itp_size_t length);

/**
 * \brief Структура интерфейса передачи данных.
 */
struct itp_endpoint {
    /**
     * \brief Указатель на функцию чтения данных.
     */
    itp_ep_read_fp read;
    /**
     * \brief Указатель на функцию записи данных.
     */
    itp_ep_write_fp write;
    /**
     * \brief Флаг использования контрольной суммы при передаче данных.
     *
     * 0 - выключена, 1 - включена. По умолчанию проверка контрольной суммы выключена.
     */
    uint8_t use_checksum;
    /**
     * \brief Указатель на пользовательский объект.
     */
    void* object;
};

/**
 * \brief Инициализация структуры интерфейса передачи данных.
 * \ingroup public_functions
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c itp_endpoint.
 * \param[in] read Указатель на функцию чтения данных.
 * \param[in] write Указатель на функцию записи данных.
 * \param[in] object Указатель на пользовательский объект.
 */
itp_error_code_t itp_init_endpoint(itp_endpoint_tp endpoint, itp_ep_read_fp read, itp_ep_write_fp write, void* object);

/**
 * \brief Приём данных через физический интерфейс.
 * \ingroup private_functions
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c itp_endpoint.
 * \param[out] data Указатель на буфер, в который производится чтение.
 * \param[in] size Размер буфера данных.
 * \return Количество прочитанных байт данных.
 */
itp_size_t itp_read_data(itp_endpoint_tp endpoint, itp_byte_t* data, itp_size_t size);

/**
 * \brief Отправка данных через физический интерфейс.
 * \ingroup private_functions
 * \param[in] endpoint Указатель на структуру интерфейса передачи данных \c itp_endpoint.
 * \param[in] data Указатель на буфер, содержащий передаваемые данные.
 * \param[in] length Количество байт, ожидающих записи.
 * \return Количество переданных байт данных.
 */
itp_size_t itp_write_data(itp_endpoint_tp endpoint, const itp_byte_t* data, itp_size_t length);

#endif // ITP_ENDPOINT_H_INCLUDED
