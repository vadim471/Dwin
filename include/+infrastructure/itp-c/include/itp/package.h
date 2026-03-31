#ifndef ITP_PACKAGE_H_INCLUDED
#define ITP_PACKAGE_H_INCLUDED

/**
 * \file package.h
 * \brief Файл содержит функции для работы со структурой пакета.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"

/**
 * \brief Структура пакета (упакованного кадра).
 */
struct itp_package {
    /**
     * \brief Адрес получателя.
     */
    uint8_t address;
    /**
     * \brief Наличие контрольной суммы в пакете.
     */
    uint8_t use_checksum;
    /**
     * \brief Статус запроса.
     */
    uint8_t status;
    /**
     * \brief Указатель на блок данных.
     */
    itp_byte_t* data;
    /**
     * \brief Размер блока данных.
     */
    itp_size_t size;
    /**
     * \brief Указатель на следующий пакет в очереди.
     */
    itp_package_tp next;
};

/**
 * \brief Инициализация структуры пакета \c itp_package.
 * \ingroup private_functions
 * \param[in] package Указатель на структуру пакета \c itp_package.
 */
void itp_init_package(itp_package_tp package);

/**
 * \brief Выделение памяти для данных.
 * \ingroup private_functions
 * \param[in] package Указатель на структуру пакета \c itp_package.
 * \param[in] size Размер блока данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_prepare_package(itp_package_tp package, itp_size_t size);

/**
 * \brief Освободить буфер пакета \c itp_package.
 * \ingroup private_functions
 * \param[in] frame Указатель на пакет \c itp_package.
 */
void itp_dispose_package(itp_package_tp package);

/**
 * \brief Освобождение памяти, занятой пакетом \c itp_package.
 * \ingroup private_functions
 * \param[in] package Указатель на структуру пакета \c itp_package.
 */
void itp_free_package(itp_package_tp package);

/**
 * \brief Упаковка результата операции (ACK/NAK/DAT).
 * \ingroup private_functions
 * \param[in] from Адрес отправителя.
 * \param[in] to Адрес получателя.
 * \param[in] command Команда.
 * \param[in] order Номер пакета.
 * \param[in] status Статус выполнения.
 * \param[in] error Код ошибки <code>\link itp_error_t \endlink</code>.
 * \param[in] result Указатель на переменную, куда будет записан код ошибки <code>\link itp_error_code_t \endlink</code> в случае её возникновения.
 * \return Указатель на структуру пакета \c itp_package.
 */
itp_package_tp itp_serialize_result(uint8_t from, uint8_t to, uint16_t command, uint16_t order, uint8_t status, uint16_t error, itp_error_code_t* result);

#endif // ITP_PACKAGE_H_INCLUDED
