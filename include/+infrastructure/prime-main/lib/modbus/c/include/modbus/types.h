#ifndef MODBUS_TYPES_H_INCLUDED
#define MODBUS_TYPES_H_INCLUDED

/**
 * \file types.h
 * \brief Файл содержит предварительные объявления структур и указателей на них.
 */

struct mbs_endpoint;

/**
 * \brief Тип структуры интерфейса передачи данных <code>\link mbs_endpoint \endlink</code>.
 */
typedef struct mbs_endpoint mbs_endpoint_t;

/**
 * \brief Указатель на структуру интерфейса передачи данных <code>\link mbs_endpoint \endlink</code>.
 */
typedef mbs_endpoint_t* mbs_endpoint_tp;

struct mbs_master;

/**
 * \brief Тип структуры ведущего устройства <code>\link mbs_master \endlink</code>.
 */
typedef struct mbs_master mbs_master_t;

/**
 * \brief Указатель на структуру ведущего устройства <code>\link mbs_master \endlink</code>.
 */
typedef mbs_master_t* mbs_master_tp;

struct mbs_slave;

/**
 * \brief Тип структуры подчинённого устройства <code>\link mbs_slave \endlink</code>.
 */
typedef struct mbs_slave mbs_slave_t;

/**
 * \brief Указатель на структуру подчинённого устройства <code>\link mbs_slave \endlink</code>.
 */
typedef mbs_slave_t* mbs_slave_tp;

struct mbs_device;

/**
 * \brief Тип структуры ведомого устройства <code>\link mbs_device \endlink</code>.
 */
typedef struct mbs_device mbs_device_t;

/**
 * \brief Указатель на структуру ведомого устройства <code>\link mbs_device \endlink</code>.
 */
typedef mbs_device_t* mbs_device_tp;

struct mbs_buffer;

/**
 * \brief Тип структуры приёма/передачи <code>\link mbs_buffer \endlink</code>.
 */
typedef struct mbs_buffer mbs_buffer_t;

/**
 * \brief Указатель на структуру приёма/передачи <code>\link mbs_buffer \endlink</code>.
 */
typedef mbs_buffer_t* mbs_buffer_tp;

struct mbs_frame;

/**
 * \brief Тип структуры кадра <code>\link mbs_frame \endlink</code>.
 */
typedef struct mbs_frame mbs_frame_t;

/**
 * \brief Указатель на структуру кадра <code>\link mbs_frame \endlink</code>.
 */
typedef mbs_frame_t* mbs_frame_tp;

struct mbs_task;

/**
 * \brief Тип структуры задачи <code>\link mbs_task \endlink</code>.
 */
typedef struct mbs_task mbs_task_t;

/**
 * \brief Указатель на структуру задачи <code>\link mbs_task \endlink</code>.
 */
typedef mbs_task_t* mbs_task_tp;

struct mbs_request;

/**
 * \brief Тип структуры запроса.
 */
typedef struct mbs_request mbs_request_t;

/**
 * \brief Указатель на структуру запроса.
 */
typedef mbs_request_t* mbs_request_tp;

struct mbs_ranged_handler;

/**
 * \brief Тип структуры обработчика диапазона регистров.
 */
typedef struct mbs_ranged_handler mbs_ranged_handler_t;

/**
 * \brief Указатель на структуру обработчика диапазона регистров.
 */
typedef mbs_ranged_handler_t* mbs_ranged_handler_tp;

struct mbs_range;

/**
 * \brief Тип структуры диапазона регистров.
 */
typedef struct mbs_range mbs_range_t;

/**
 * \brief Указатель на структуру диапазона регистров.
 */
typedef mbs_range_t* mbs_range_tp;

struct mbs_ranges_list;

/**
 * \brief Тип структуры списка диапазонов регистров.
 */
typedef struct mbs_ranges_list mbs_ranges_list_t;

/**
 * \brief Указатель на структуру списка диапазонов регистров.
 */
typedef mbs_ranges_list_t* mbs_ranges_list_tp;

#endif // MODBUS_TYPES_H_INCLUDED
