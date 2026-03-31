#ifndef ITP_TUNNEL_H_INCLUDED
#define ITP_TUNNEL_H_INCLUDED

/**
 * \file tunnel.h
 * \brief Файл содержит определение структур тоннеля для передачи данных.
 */

#include "endpoint.h"

/**
 * \defgroup tunnel_definitions Тоннель передачи данных
 * \brief Список структур и функций, используемых для замыкания двух интерфейсов передачи данных.
 */

struct itp_tunnel_endpoint;

/**
 * \brief Тип структуры конечной точки тоннеля передачи данных <code>\link itp_tunnel_endpoint \endlink</code>.
 * \ingroup tunnel_definitions
 */
typedef struct itp_tunnel_endpoint itp_tunnel_endpoint_t;

/**
 * \brief Указатель на структуру конечной точки тоннеля передачи данных <code>\link itp_tunnel_endpoint \endlink</code>.
 * \ingroup tunnel_definitions
 */
typedef itp_tunnel_endpoint_t* itp_tunnel_endpoint_tp;

/**
 * \brief Структура конечной точки тоннеля передачи данных.
 * \ingroup tunnel_definitions
 */
struct itp_tunnel_endpoint {
    /**
     * \brief Промежуточный буфер для хранения принимаемых данных.
     */
    itp_byte_t data[ITP_TUNNEL_BUFFER_SIZE];
    /**
     * \brief Длина данных в буфере.
     */
    itp_size_t length;
    /**
     * \brief Указатель на связанную конечную точку.
     */
    itp_tunnel_endpoint_tp remote;
};

/**
 * \brief Инициализация конечной точки тоннеля передачи данных.
 *
 * Вызывается автоматически функцией <code>\link itp_init_tunnel \endlink</code>.
 * \ingroup tunnel_definitions
 * \param[in] endpoint Указатель на инициализируемую конечную точку.
 * \param[in] remote Указатель на замыкающую конечную точку.
 */
itp_error_code_t itp_init_tunnel_endpoint(itp_tunnel_endpoint_tp endpoint, itp_tunnel_endpoint_tp remote);

struct itp_tunnel;

/**
 * \brief Тип структуры тоннеля передачи данных <code>\link itp_tunnel \endlink</code>.
 * \ingroup tunnel_definitions
 */
typedef struct itp_tunnel itp_tunnel_t;

/**
 * \brief Указатель на структуру тоннеля передачи данных <code>\link itp_tunnel \endlink</code>.
 * \ingroup tunnel_definitions
 */
typedef itp_tunnel_t* itp_tunnel_tp;

/**
 * \brief Структура тоннеля передачи данных.
 * \ingroup tunnel_definitions
 */
struct itp_tunnel {
    /**
     * \brief Массив конечных точек.
     */
    itp_tunnel_endpoint_t ep[2];
};

/**
 * \brief Соединение двух интерфейсов передачи данных тоннелем.
 * \ingroup tunnel_definitions
 * \param[in] tunnel Указатель на структуру тоннеля передачи данных \c itp_tunnel.
 * \param[in] ep0 Указатель на первый интерфейс \c itp_endpoint.
 * \param[in] ep1 Указатель на второй интерфейс \c itp_endpoint.
 */
itp_error_code_t itp_init_tunnel(itp_tunnel_tp tunnel, itp_endpoint_tp ep0, itp_endpoint_tp ep1);

/**
 * \brief Отправка данных через тоннель.
 * \ingroup tunnel_definitions
 * \param[in] object Указатель на конечную точку тоннеля \c itp_tunnel_endpoint.
 * \param[in] data Буфер с отправляемыми данными.
 * \param[in] length Размер буфера данных.
 */
itp_size_t itp_tunnel_write_data(void* object, const itp_byte_t* data, itp_size_t length);

/**
 * \brief Чтение данных из промежуточного буфера.
 * \ingroup tunnel_definitions
 * \param[in] object Указатель на конечную точку тоннеля \c itp_tunnel_endpoint.
 * \param[in] data Буфер для принимаемых данных.
 * \param[in] size Размер буфера для данных.
 */
itp_size_t itp_tunnel_read_data(void* object, itp_byte_t* data, itp_size_t size);

#endif // ITP_TUNNEL_H_INCLUDED
