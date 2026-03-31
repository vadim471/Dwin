#ifndef MODBUS_PROTOCOL_H_INCLUDED
#define MODBUS_PROTOCOL_H_INCLUDED

/**
 * \file protocol.h
 * \brief Файл содержит функции для разбора и генерации пакетов.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"

/**
 * \brief Перечисление статусов разбора кадра.
 * \ingroup enum_definitions
 */
typedef enum {
    /**
     * \brief Разбор кадра завершён успешно.
     */
    MBS_PARSE_OK = 0,
    /**
     * \brief Кадр не имеет корректного окончания.
     */
    MBS_PARSE_NOT_END,
    /**
     * \brief Кадр имеет недопустимую структуру.
     */
    MBS_PARSE_ERROR
} mbs_parse_status_t;

/**
 * \brief Тип функции проверки размера PDU.
 * \ingroup extentions
 * \param[in] buffer Указатель на структуру приёма/передачи кадров \c mbs_buffer.
 * \return Предполагаемый размер кадра.
 */
typedef mbs_size_t(*mbs_get_pdu_length_fp)(mbs_buffer_tp buffer);

/**
 * \brief Тип функции проверки структуры кадра.
 * \ingroup extentions
 * \param[in] buffer Указатель на структуру приёма/передачи кадров \c mbs_buffer.
 * \param[in] error Указатель на переменную, куда будет записан код ошибки.
 * \return Результат выполнения операции <code>\link mbs_parse_status_t \endlink</code>.
 */
typedef mbs_parse_status_t(*mbs_check_frame_fp)(mbs_buffer_tp buffer, mbs_error_code_t* error);

/**
 * \brief Тип функции извлечения кадра из буфера.
 * \ingroup extentions
 * \param[in] buffer Указатель на структуру приёма/передачи кадров \c mbs_buffer.
 * \param[in] error Указатель на переменную, куда будет записан код ошибки.
 * \return Указатель на кадр в случае успеха или \c NULL в случае ошибки.
 */
typedef mbs_frame_tp(*mbs_extract_frame_fp)(mbs_buffer_tp buffer, mbs_error_code_t* error);

/**
 * \brief Тип функции записи кадра в буфер.
 * \ingroup extentions
 * \param[in] buffer Указатель на структуру приёма/передачи кадров \c mbs_buffer.
 * \param[in] frame Указатель на кадр \c mbs_frame.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
typedef mbs_error_code_t(*mbs_serialize_frame_fp)(mbs_buffer_tp buffer, mbs_frame_tp frame);

/**
 * \brief Проверка размера PDU для ведущего устройства.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи кадров \c mbs_buffer.
 * \return Предполагаемый размер кадра.
 */
mbs_size_t mbs_get_pdu_length_master(mbs_buffer_tp buffer);

/**
 * \brief Проверка размера PDU для подчинённого устройства.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи кадров \c mbs_buffer.
 * \return Предполагаемый размер кадра.
 */
mbs_size_t mbs_get_pdu_length_slave(mbs_buffer_tp buffer);

/**
 * \brief Запись запроса в буфер.
 * \ingroup private_functions
 * \param[in] master Указатель на структуру ведущего устройства \c mbs_master.
 * \param[in] buffer Указатель на структуру приёма/передачи кадров \c mbs_buffer.
 * \param[in] request Указатель на структуру запроса \c mbs_request.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_bufferize_request(mbs_master_tp master, mbs_buffer_tp buffer, mbs_request_tp request);

/**
 * \brief Вызов обработчика результата.
 * \ingroup private_functions
 * \param[in] master Указатель на структуру ведущего устройства \c mbs_master.
 * \param[in] buffer Указатель на структуру приёма/передачи кадров \c mbs_buffer.
 * \param[in] request Указатель на структуру запроса \c mbs_request.
 * \param[in] frame Указатель на кадр ответа \c mbs_frame.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_call_on_result(mbs_master_tp master, mbs_device_tp device, mbs_request_tp request, mbs_frame_tp frame);

/**
 * \brief Вызов обработчика запроса.
 * \ingroup private_functions
 * \param[in] slave Указатель на структуру подчинённого устройства \c mbs_slave.
 * \param[in] range Указатель на диапазон регистров \c mbs_range.
 * \param[in] request Указатель на кадр запроса \c mbs_frame.
 * \param[in] response Указатель на кадр ответа \c mbs_frame.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_call_on_request(mbs_slave_tp slave, mbs_range_tp range, mbs_frame_tp request, mbs_frame_tp response);

/**
 * \brief Опрос устройства.
 * \ingroup private_functions
 * \param[in] master Указатель на структуру ведущего узла \c mbs_master.
 * \param[in] device Указатель на структуру устройства \c mbs_device.
 * \param[in] task Указатель на задачу \c mbs_task.
 * \param[in] error Код ошибки <code>\link mbs_error_t \endlink</code>.
 */
void mbs_report_task_error(mbs_master_tp master, mbs_device_tp device, mbs_task_tp task, mbs_byte_t error);

#endif // MODBUS_PROTOCOL_H_INCLUDED
