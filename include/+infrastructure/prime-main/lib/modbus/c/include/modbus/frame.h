#ifndef MODBUS_FRAME_H_INCLUDED
#define MODBUS_FRAME_H_INCLUDED

/**
 * \file frame.h
 * \brief Файл содержит функции для работы с кадром.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"

/**
 * \brief Структура кадра.
 */
struct mbs_frame {
    /**
     * \brief Адрес устройства.
     */
    uint8_t slave;
    /**
     * \brief Команда.
     */
    uint8_t command;
    /**
     * \brief Код ошибки Modbus.
     */
    uint8_t error;
    /**
     * \brief Указатель на массив данных.
     */
    mbs_byte_t* data;
    /**
     * \brief Размер массива данных.
     */
    mbs_size_t size;
};

/**
 * \brief Инициализация структуры кадра \c mbs_frame.
 * \ingroup private_functions
 * \param[in] frame Указатель на кадр \c mbs_frame.
 */
void mbs_init_frame(mbs_frame_tp frame);

/**
 * \brief Выделение памяти и инициализация структуры кадра \c mbs_frame.
 * \ingroup private_functions
 * \param[in] command Тип команды <code>\link mbs_symbol_t \endlink</code>.
 * \return Указатель на кадр \c mbs_frame.
 */
mbs_frame_tp mbs_create_frame(void);

/**
 * \brief Освобождение динамически выделенной памяти для кадра \c mbs_frame.
 * \ingroup private_functions
 * \param[in] frame Указатель на кадр \c mbs_frame.
 */
void mbs_free_frame(mbs_frame_tp frame);

/**
 * \brief Подготовить буфер кадра \c mbs_frame.
 * \ingroup private_functions
 * \param[in] frame Указатель на кадр \c mbs_frame.
 * \param[in] size Размер буфера.
 */
mbs_error_code_t mbs_prepare_frame(mbs_frame_tp frame, mbs_size_t size);

/**
 * \brief Освободить буфер кадра \c mbs_frame.
 * \ingroup private_functions
 * \param[in] frame Указатель на кадр \c mbs_frame.
 */
void mbs_dispose_frame(mbs_frame_tp frame);

#endif // MODBUS_FRAME_H_INCLUDED
