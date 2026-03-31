#ifndef MODBUS_REPEAT_H_INCLUDED
#define MODBUS_REPEAT_H_INCLUDED

/**
 * \file repeat.h
 * \brief Файл содержит функции автоповтора запросов.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "list.h"

/**
 * \brief Служебная структура для однократного повтора запроса.
 */
typedef struct {
    /**
     * \brief Если запрос готов к отправке - 1, иначе 0 (ноль).
     *
     * После сброса поля в 0 (ноль), оно уже не может менять значение.
     */
    uint8_t active;
} mbs_repeat_once_t;

/**
 * \brief Функция инициализации структуры однократного повтора запроса.
 * \ingroup private_functions
 * \param[in] object Указатель на служебную структуру \c mbs_repeat_once_t.
 * \return Результат выполнения операции <code>\link mbs_repeat_status_t \endlink</code>.
 */
void mbs_repeat_once_init(mbs_repeat_once_t* object);

/**
 * \brief Функция однократного повтора запроса.
 * \ingroup private_functions
 * \param[in] object Указатель на служебную структуру \c mbs_repeat_once_t.
 * \param[in] time Текущее время в системе.
 * \param[in] error Указатель на переменную, куда будет записан код ошибки.
 * \return Результат выполнения операции <code>\link mbs_repeat_status_t \endlink</code>.
 */
mbs_repeat_status_t mbs_repeat_once_check(
    void* object,
    mbs_time_t time,
    mbs_error_code_t* error
);

/**
 * \brief Функция сброса однократного повтора запроса.
 * \ingroup private_functions
 * \param[in] object Указатель на служебную структуру \c mbs_repeat_once_t.
 * \param[in] time Текущее время в системе.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_repeat_once_reset(
    void* object,
    mbs_time_t time
);

/**
 * \brief Служебная структура для повтора запроса по истечении времени.
 */
typedef struct {
    /**
     * \brief Время повтора запроса.
     */
    mbs_time_t timeout;
    /**
     * \brief Время последнего сброса.
     */
    mbs_time_t begin;
} mbs_repeat_cyclic_t;

/**
 * \brief Функция инициализации структуры повтора запроса по истечении времени.
 * \ingroup private_functions
 * \param[in] object Указатель на служебную структуру \c mbs_repeat_cyclic_t.
 * \param[in] timeout Время повтора запроса.
 */
void mbs_repeat_cyclic_init(mbs_repeat_cyclic_t* object, mbs_time_t timeout);

/**
 * \brief Функция повтора запроса по истечении времени.
 * \ingroup private_functions
 * \param[in] object Указатель на служебную структуру \c mbs_repeat_cyclic_t.
 * \param[in] time Текущее время в системе.
 * \param[in] error Указатель на переменную, куда будет записан код ошибки.
 * \return Результат выполнения операции <code>\link mbs_repeat_status_t \endlink</code>.
 */
mbs_repeat_status_t mbs_repeat_cyclic_check(
    void* object,
    mbs_time_t time,
    mbs_error_code_t* error
);

/**
 * \brief Функция сброса повтора по истечении времени.
 * \ingroup private_functions
 * \param[in] object Указатель на служебную структуру \c mbs_repeat_cyclic_t.
 * \param[in] time Текущее время в системе.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_repeat_cyclic_reset(
    void* object,
    mbs_time_t time
);

/**
 * \brief Служебная структура для ручного повтора запроса.
 */
typedef struct {
    /**
     * \brief Если запрос готов к отправке - 1, иначе 0 (ноль).
     *
     * После получения ответа автоматически сбросится в 0 (ноль).
     */
    uint8_t* trigger;
} mbs_repeat_manual_t;

/**
 * \brief Функция инициализации структуры ручного повтора запроса.
 * \ingroup private_functions
 * \param[in] object Указатель на служебную структуру \c mbs_repeat_manual_t.
 * \param[in] trigger Указатель на отслеживаемую переменную.
 * \return Результат выполнения операции <code>\link mbs_repeat_status_t \endlink</code>.
 */
void mbs_repeat_manual_init(mbs_repeat_manual_t* object, uint8_t* trigger);

/**
 * \brief Функция ручного повтора запроса.
 * \ingroup private_functions
 * \param[in] object Указатель на служебную структуру \c mbs_repeat_manual_t.
 * \param[in] time Текущее время в системе.
 * \param[in] error Указатель на переменную, куда будет записан код ошибки.
 * \return Результат выполнения операции <code>\link mbs_repeat_status_t \endlink</code>.
 */
mbs_repeat_status_t mbs_repeat_manual_check(
    void* object,
    mbs_time_t time,
    mbs_error_code_t* error
);

/**
 * \brief Функция сброса ручного повтора запроса.
 * \ingroup private_functions
 * \param[in] object Указатель на служебную структуру \c mbs_repeat_manual_t.
 * \param[in] time Текущее время в системе.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_repeat_manual_reset(
    void* object,
    mbs_time_t time
);

#endif // MODBUS_REPEAT_H_INCLUDED
