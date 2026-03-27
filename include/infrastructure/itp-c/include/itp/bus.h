#ifndef ITP_BUS_H_INCLUDED
#define ITP_BUS_H_INCLUDED

/**
 * \file bus.h
 * \brief Файл содержит определение структур виртуальной шины данных.
 */

/**
 * \defgroup bus_definitions Виртуальная шина данных
 * \brief Список структур и функций, используемых для замыкания нескольких интерфейсов передачи данных.
 */

#include <itp/endpoint.h>

struct itp_bus;

/**
 * \brief Тип структуры шины данных <code>\link itp_bus \endlink</code>.
 * \ingroup bus_definitions
 */
typedef struct itp_bus itp_bus_t;

/**
 * \brief Указатель на структуру шины данных <code>\link itp_bus \endlink</code>.
 * \ingroup bus_definitions
 */
typedef itp_bus_t* itp_bus_tp;

struct itp_bus_endpoint;

/**
 * \brief Тип структуры конечной точки шины данных <code>\link itp_bus_endpoint \endlink</code>.
 * \ingroup bus_definitions
 */
typedef struct itp_bus_endpoint itp_bus_endpoint_t;

/**
 * \brief Указатель на структуру конечной точки шины данных <code>\link itp_bus_endpoint \endlink</code>.
 * \ingroup bus_definitions
 */
typedef itp_bus_endpoint_t* itp_bus_endpoint_tp;

/**
 * \brief Структура конечной точки шины данных.
 * \ingroup bus_definitions
 */
struct itp_bus_endpoint {
    /**
     * \brief Промежуточный буфер для хранения принимаемых данных.
     */
    itp_byte_t data[ITP_BUS_BUFFER_SIZE];
    /**
     * \brief Длина данных в буфере.
     */
    itp_size_t length;
    /**
     * \brief Указатель на структуру шины данных.
     */
    itp_bus_tp bus;
};

/**
 * \brief Инициализация конечной точки шины данных.
 *
 * Вызывается автоматически функцией <code>\link itp_join_bus \endlink</code>.
 * \ingroup bus_definitions
 * \param[in] endpoint Указатель на инициализируемую конечную точку.
 * \param[in] bus Указатель на структуру шины данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_init_bus_endpoint(itp_bus_endpoint_tp endpoint, itp_bus_tp bus);

/**
 * \brief Структура шины данных.
 * \ingroup bus_definitions
 */
struct itp_bus {
    /**
     * \brief Список конечных точек.
     */
    itp_bus_endpoint_tp* data;
    /**
     * \brief Размер массива конечных точек.
     */
    uint8_t size;
    /**
     * \brief Количество конечных точек.
     */
    uint8_t count;
    /**
     * \brief Передавать данные также в отправляющий интерфейс (1 - да, 0 - нет).
     */
    uint8_t echo;
};

/**
 * \brief Инициализация шины данных.
 * \ingroup bus_definitions
 * \param[in] bus Указатель на структуру шины данных \c itp_bus.
 * \param[in] echo Передавать данные также в отправляющий интерфейс (1 - да, 0 - нет).
 */
itp_error_code_t itp_init_bus(itp_bus_tp bus, uint8_t echo);

/**
 * \brief Увеличить количество слотов на шины данных.
 *
 * При регистрации новых интерфейсов указатели будут записываться в заранее выделенную память.
 * \ingroup bus_definitions
 * \param[in] bus Указатель на структуру шины данных \c itp_bus.
 * \param[in] size Количество дополнительных слотов.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_expand_bus(itp_bus_tp bus, uint8_t size);

/**
 * \brief Регистрация интерфейса передачи данных на шине.
 * \ingroup bus_definitions
 * \param[in] bus Указатель на структуру шины данных \c itp_bus.
 * \param[in] ep Указатель на интерфейс передачи данных \c itp_endpoint.
 */
itp_error_code_t itp_join_bus(itp_bus_tp bus, itp_endpoint_tp ep);

/**
 * \brief Отключение всех зарегистрированных интерфейсов передачи данных.
 * \ingroup bus_definitions
 * \param[in] bus Указатель на структуру шины данных \c itp_bus.
 */
void itp_clear_bus(itp_bus_tp bus);

/**
 * \brief Отправка данных по шине.
 * \ingroup bus_definitions
 * \param[in] object Указатель на конечную точку шины \c itp_bus_endpoint.
 * \param[in] data Буфер с отправляемыми данными.
 * \param[in] length Размер буфера данных.
 */
itp_size_t itp_bus_write_data(void* object, const itp_byte_t* data, itp_size_t length);

/**
 * \brief Чтение данных из промежуточного буфера.
 * \ingroup bus_definitions
 * \param[in] object Указатель на конечную точку шины \c itp_bus_endpoint.
 * \param[in] data Буфер для принимаемых данных.
 * \param[in] size Размер буфера для данных.
 */
itp_size_t itp_bus_read_data(void* object, itp_byte_t* data, itp_size_t size);

#endif // ITP_BUS_H_INCLUDED
