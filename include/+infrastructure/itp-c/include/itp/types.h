#ifndef ITP_TYPES_H_INCLUDED
#define ITP_TYPES_H_INCLUDED

/**
 * \file types.h
 * \brief Файл содержит предварительные объявления структур и указателей на них.
 */

struct itp_endpoint;

/**
 * \brief Тип структуры интерфейса передачи данных <code>\link itp_endpoint \endlink</code>.
 */
typedef struct itp_endpoint itp_endpoint_t;

/**
 * \brief Указатель на структуру интерфейса передачи данных <code>\link itp_endpoint \endlink</code>.
 */
typedef itp_endpoint_t* itp_endpoint_tp;

struct itp_buffer;

/**
 * \brief Тип структуры приёма/передачи пакетов <code>\link itp_buffer \endlink</code>.
 */
typedef struct itp_buffer itp_buffer_t;

/**
 * \brief Указатель на структуру приёма/передачи пакетов <code>\link itp_buffer \endlink</code>.
 */
typedef itp_buffer_t* itp_buffer_tp;

struct itp_package;

/**
 * \brief Тип структуры пакета <code>\link itp_package \endlink</code>.
 */
typedef struct itp_package itp_package_t;

/**
 * \brief Указатель на структуру пакета <code>\link itp_package \endlink</code>.
 */
typedef itp_package_t* itp_package_tp;

struct itp_frame;

/**
 * \brief Тип структуры кадра <code>\link itp_frame \endlink</code>.
 */
typedef struct itp_frame itp_frame_t;

/**
 * \brief Указатель на структуру кадра <code>\link itp_frame \endlink</code>.
 */
typedef itp_frame_t* itp_frame_tp;

struct itp_node;

/**
 * \brief Тип структуры узла сети <code>\link itp_node \endlink</code>.
 */
typedef struct itp_node itp_node_t;

/**
 * \brief Указатель на структуру узла сети <code>\link itp_node \endlink</code>.
 */
typedef itp_node_t* itp_node_tp;

struct itp_root;

/**
 * \brief Тип корневой структуры узла сети <code>\link itp_root \endlink</code>.
 */
typedef struct itp_root itp_root_t;

/**
 * \brief Указатель на корневую структуру узла сети <code>\link itp_root \endlink</code>.
 */
typedef itp_root_t* itp_root_tp;

#endif // ITP_TYPES_H_INCLUDED
