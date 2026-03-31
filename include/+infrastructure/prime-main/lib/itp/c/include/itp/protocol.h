#ifndef ITP_PROTOCOL_H_INCLUDE
#define ITP_PROTOCOL_H_INCLUDE

/**
 * \file protocol.h
 * \brief Файл содержит функции для разбора и генерации пакетов.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"

/**
 * \brief Перечисление статусов разбора пакета.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Разбор пакета завершён успешно.
     */
    ITP_PARSE_OK = 0,
    /**
     * \brief Пакет не имеет корректного окончания.
     */
    ITP_PARSE_NOT_END,
    /**
     * \brief Пакет имеет недопустимую структуру.
     */
    ITP_PARSE_ERROR
} itp_parse_status_t;

/**
 * \brief Проверка структуры пакета.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 * \param[in] expected_size Указатель на переменную, куда будет записан ожидаемый размер пакета.
 * \return Результат выполнения операции <code>\link itp_parse_status_t \endlink</code>.
 */
itp_parse_status_t itp_check_package(itp_buffer_tp buffer, itp_size_t* expected_size);

/**
 * \brief Попытка найти заголовок пакета в буфере.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 * \return Результат выполнения операции <code>\link itp_parse_status_t \endlink</code>.
 */
itp_parse_status_t itp_repair_offset(itp_buffer_tp buffer);

/**
 * \brief Извлечение пакета из буфера и его частичный разбор.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 * \param[out] error Указатель на переменную, куда будет записан код ошибки. Опционально.
 * \return Указатель на пакет с данными в случае успеха или \c NULL в случае ошибки.
 */
itp_package_tp itp_extract_package(
#ifdef ITP_TRACE_BUFFERS
uint8_t root_address,
#endif // ITP_TRACE_BUFFERS
itp_buffer_tp buffer, itp_error_code_t* error);

/**
 * \brief Запись пакета в буфер.
 * \warning Функция не обеспечивает запись контрольной суммы и не должна вызываться при передаче данных.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 * \param[in] package Указатель на пакет с данными \c itp_package.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_write_package(itp_buffer_tp buffer, itp_package_tp package);

/**
 * \brief Разбор пакета.
 * \ingroup private_functions
 * \param[in] package Указатель на пакет с данными \c itp_package.
 * \param[out] error Указатель на переменную, куда будет записан код ошибки. Опционально.
 * \return Указатель на кадр в случае успеха или \c NULL в случае ошибки.
 */
itp_frame_tp itp_parse_package(itp_package_tp package, itp_error_code_t* error);

/**
 * \brief Формирование пакета из кадра.
 * \ingroup private_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[out] error Указатель на переменную, куда будет записан код ошибки. Опционально.
 * \return Указатель на созданный пакет \c itp_package в случае успеха.
 */
itp_package_tp itp_serialize_frame(itp_frame_tp frame, itp_error_code_t* error);

/**
 * \brief Получение версии протокола из пакета.
 * \ingroup private_functions
 * \param[in] package Указатель на пакет с данными \c itp_package.
 * \param[out] error Указатель на переменную, куда будет записан код ошибки. Опционально.
 * \return Команда пакета или 0 (ноль) в случае ошибки.
 */
uint8_t itp_get_package_version(itp_package_tp package, itp_error_code_t* error);

/**
 * \brief Получение адреса отправителя из пакета.
 * \ingroup private_functions
 * \param[in] package Указатель на пакет с данными \c itp_package.
 * \param[out] error Указатель на переменную, куда будет записан код ошибки. Опционально.
 * \return Команда пакета или 0 (ноль) в случае ошибки.
 */
uint8_t itp_get_package_from(itp_package_tp package, itp_error_code_t* error);

/**
 * \brief Получение адреса получателя из пакета.
 * \ingroup private_functions
 * \param[in] package Указатель на пакет с данными \c itp_package.
 * \param[out] error Указатель на переменную, куда будет записан код ошибки. Опционально.
 * \return Команда пакета или 0 (ноль) в случае ошибки.
 */
uint8_t itp_get_package_to(itp_package_tp package, itp_error_code_t* error);

/**
 * \brief Получение команды из пакета.
 * \ingroup private_functions
 * \param[in] package Указатель на пакет с данными \c itp_package.
 * \param[out] error Указатель на переменную, куда будет записан код ошибки. Опционально.
 * \return Команда пакета или 0 (ноль) в случае ошибки.
 */
uint16_t itp_get_package_command(itp_package_tp package, itp_error_code_t* error);

/**
 * \brief Получение порядкового номера пакета.
 * \ingroup private_functions
 * \param[in] package Указатель на пакет с данными \c itp_package.
 * \param[out] error Указатель на переменную, куда будет записан код ошибки. Опционально.
 * \return Порядковый номер пакета.
 */
uint16_t itp_get_package_order(itp_package_tp package, itp_error_code_t* error);

/**
 * \brief Дописывание контрольной суммы в буфер приёма/передачи.
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 */
void itp_append_checksum(itp_buffer_tp buffer);

/**
 * \brief Удаление контрольной суммы из буфер приёма/передачи.
 * \param[in] buffer Указатель на структуру приёма/передачи пакетов \c itp_buffer.
 */
void itp_remove_checksum(itp_buffer_tp buffer);

#endif // ITP_PROTOCOL_H_INCLUDE
