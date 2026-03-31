#ifndef MODBUS_RTU_H_INCLUDED
#define MODBUS_RTU_H_INCLUDED

/**
 * \file rtu.h
 * \brief Файл содержит функции для разбора и генерации пакетов Modbus RTU.
 */

#include "config.h"
#include "protocol.h"

/**
 * \brief Проверка структуры RTU кадра.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи кадров \c mbs_buffer.
 * \param[in] error Указатель на переменную, куда будет записан код ошибки.
 * \return Результат выполнения операции <code>\link mbs_parse_status_t \endlink</code>.
 */
mbs_parse_status_t mbs_check_rtu_frame(mbs_buffer_tp buffer, mbs_error_code_t* error);

/**
 * \brief Извлечение RTU кадра из буфера.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи кадров \c mbs_buffer.
 * \param[in] error Указатель на переменную, куда будет записан код ошибки.
 * \return Указатель на кадр в случае успеха или \c NULL в случае ошибки.
 */
mbs_frame_tp mbs_extract_rtu_frame(mbs_buffer_tp buffer, mbs_error_code_t* error);

/**
 * \brief Запись RTU кадра в буфер.
 * \ingroup private_functions
 * \param[in] buffer Указатель на структуру приёма/передачи кадров \c mbs_buffer.
 * \param[in] frame Указатель на кадр \c mbs_frame.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_serialize_rtu_frame(mbs_buffer_tp buffer, mbs_frame_tp frame);

#endif // MODBUS_RTU_H_INCLUDED
