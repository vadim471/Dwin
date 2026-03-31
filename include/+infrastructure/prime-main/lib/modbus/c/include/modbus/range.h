#ifndef MODBUS_RANGE_H_INCLUDED
#define MODBUS_RANGE_H_INCLUDED

/**
 * \file range.h
 * \brief Файл содержит функции для работы с диапазонами регистров.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"

#ifdef MODBUS_EXTEND_CALLBACKS
#include <stddef.h>
#endif // MODBUS_EXTEND_CALLBACKS

/**
 * \brief Структура обработчика диапазона регистров.
 */
struct mbs_ranged_handler {
    /**
     * \brief Первый регистр диапазона.
     */
    uint16_t first;
    /**
     * \brief Последний регистр диапазона.
     */
    uint16_t last;
    /**
     * \brief Указатель на пользовательский обработчик.
     */
    void* handler;
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t eparam;
    #endif // MODBUS_EXTEND_CALLBACKS
    /**
     * \brief Указатель на следующий объект в списке.
     */
    mbs_ranged_handler_tp next;
};

/**
 * \brief Структура диапазона регистров.
 */
struct mbs_range {
    /**
     * \brief Команда, для которой определён данный диапазон.
     */
    uint8_t command;
    /**
     * \brief Указатель на первый объект в списке.
     */
    mbs_ranged_handler_tp first;
    /**
     * \brief Указатель на следующий диапазон.
     */
    mbs_range_tp next;
};

/**
 * \brief Структура списка диапазонов регистров.
 */
struct mbs_ranges_list {
    /**
     * \brief Указатель на первый диапазон в списке.
     */
    mbs_range_tp first;
};

/**
 * \brief Инициализация обработчика \c mbs_ranged_handler.
 * \ingroup private_functions
 * \param[in] request Указатель на обработчик \c mbs_ranged_handler.
 */
void mbs_init_ranged_handler(mbs_ranged_handler_tp handler);

/**
 * \brief Освобождение памяти, занятой структурой обработчика \c mbs_ranged_handler.
 * \ingroup private_functions
 * \param[in] request Указатель на обработчик \c mbs_ranged_handler.
 */
void mbs_free_ranged_handler(mbs_ranged_handler_tp handler);

/**
 * \brief Инициализация диапазона регистров \c mbs_range.
 * \ingroup private_functions
 * \param[in] range Указатель на диапазон регистров \c mbs_range.
 * \param[in] command Команда.
 */
void mbs_init_range(mbs_range_tp range, uint8_t command);

/**
 * \brief Добавление обработчика в конец списка.
 * \ingroup private_functions
 * \param[in] range Указатель на диапазон регистров \c mbs_range.
 * \param[in] handler Указатель на обработчик \c mbs_ranged_handler.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_push_ranged_handler(mbs_range_tp range, mbs_ranged_handler_tp handler);

/**
 * \brief Удаление указанного обработчика из списка.
 * \ingroup private_functions
 * \param[in] range Указатель на диапазон регистров \c mbs_range.
 * \param[in] handler Указатель на обработчик \c mbs_ranged_handler.
 * \return Указатель на следующую задачу в списке, если она есть.
 */
mbs_ranged_handler_tp mbs_erase_ranged_handler(mbs_range_tp range, mbs_ranged_handler_tp handler);

/**
 * \brief Проверка диапазона регистров на пустоту.
 * \ingroup private_functions
 * \param[in] range Указатель на диапазон регистров \c mbs_range.
 * \return Если список пуст - 0 (ноль), если нет - 1 (единицу).
 */
uint8_t mbs_range_is_empty(mbs_range_tp range);

/**
 * \brief Очистка диапазона регистров.
 * \ingroup private_functions
 * \param[in] range Указатель на диапазон регистров \c mbs_range.
 */
void mbs_clear_range(mbs_range_tp range);

/**
 * \brief Удаление диапазона регистров.
 * \ingroup private_functions
 * \param[in] range Указатель на диапазон регистров \c mbs_range.
 */
void mbs_free_range(mbs_range_tp range);

/**
 * \brief Инициализация списка диапазонов \c mbs_ranges_list.
 * \ingroup private_functions
 * \param[in] list Указатель на список диапазонов \c mbs_ranges_list.
 */
void mbs_init_ranges_list(mbs_ranges_list_tp list);

/**
 * \brief Добавление диапазона в конец списка.
 * \ingroup private_functions
 * \param[in] list Указатель на список диапазонов \c mbs_ranges_list.
 * \param[in] range Указатель на диапазон регистров \c mbs_range.
 * \return Результат выполнения операции <code>\link mbs_error_code_t \endlink</code>.
 */
mbs_error_code_t mbs_push_range(mbs_ranges_list_tp list, mbs_range_tp range);

/**
 * \brief Найти диапазон регистров по команде.
 * \ingroup private_functions
 * \param[in] list Указатель на список диапазонов \c mbs_ranges_list.
 * \param[in] command Команда.
 * \return Указатель на диапазон.
 */
mbs_range_tp mbs_find_range(mbs_ranges_list_tp list, mbs_byte_t command);

/**
 * \brief Найти или создать диапазон регистров.
 * \ingroup private_functions
 * \param[in] list Указатель на список диапазонов \c mbs_ranges_list.
 * \param[in] command Команда.
 * \param[in] error Указатель на переменную, куда будет записан код ошибки.
 * \return Указатель на диапазон.
 */
mbs_range_tp mbs_get_or_create_range(mbs_ranges_list_tp list, mbs_byte_t command, mbs_error_code_t* error);

/**
 * \brief Удаление указанного диапазона из списка.
 * \ingroup private_functions
 * \param[in] list Указатель на список диапазонов \c mbs_ranges_list.
 * \param[in] range Указатель на диапазон регистров \c mbs_range.
 * \return Указатель на следующую задачу в списке, если она есть.
 */
mbs_range_tp mbs_erase_range(mbs_ranges_list_tp list, mbs_range_tp range);

/**
 * \brief Проверка списка диапазонов на пустоту.
 * \ingroup private_functions
 * \param[in] list Указатель на список диапазонов \c mbs_ranges_list.
 * \return Если список пуст - 0 (ноль), если нет - 1 (единицу).
 */
uint8_t mbs_ranges_list_is_empty(mbs_ranges_list_tp list);

/**
 * \brief Очистка списка диапазонов.
 * \ingroup private_functions
 * \param[in] list Указатель на список диапазонов \c mbs_ranges_list.
 */
void mbs_clear_ranges_list(mbs_ranges_list_tp list);

#endif // MODBUS_RANGE_H_INCLUDED
