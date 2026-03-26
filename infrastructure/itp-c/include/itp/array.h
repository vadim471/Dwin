#ifndef ITP_ARRAY_H_INCLUDED
#define ITP_ARRAY_H_INCLUDED

/**
 * \file array.h
 * \brief Файл содержит функции для работы с массивами узлов и адресов.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"

/**
 * \brief Структура массива узлов сети.
 */
struct itp_node_array {
    /**
     * \brief Список дочерних узлов сети.
     */
    itp_node_tp* data;
    /**
     * \brief Размер массива дочерних узлов сети.
     */
    uint8_t size;
    /**
     * \brief Количество дочерних узлов сети.
     */
    uint8_t count;
};

/**
 * \brief Тип структуры массива узлов сети <code>\link itp_node_array \endlink</code>.
 */
typedef struct itp_node_array itp_node_array_t;

/**
 * \brief Указатель на структуру массива узлов сети <code>\link itp_node_array \endlink</code>.
 */
typedef itp_node_array_t* itp_node_array_tp;

/**
 * \brief Инициализация массива узлов сети.
 * \ingroup private_functions
 * \param[in] array Указатель на структуру массива узлов \c itp_node_array.
 */
void itp_init_node_array(itp_node_array_tp array);

/**
 * \brief Увеличить размер массива узлов сети.
 *
 * При добавлении новых узлов указатели будут записываться в заранее выделенную память.
 * \ingroup private_functions
 * \param[in] array Указатель на структуру массива узлов \c itp_node_array.
 * \param[in] size Количество дополнительных слотов.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_expand_node_array(itp_node_array_tp array, uint8_t size);

/**
 * \brief Добавить узел сети в массив.
 *
 * Если память для массива не была выделена заранее, то происходит вызов функции <code>\link itp_malloc \endlink</code>.
 * \ingroup private_functions
 * \param[in] array Указатель на структуру массива узлов \c itp_node_array.
 * \param[in] node Указатель на структуру узла сети \c itp_node.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_add_node_to_array(itp_node_array_tp array, itp_node_tp node);

/**
 * \brief Очистка массива узлов сети.
 * \ingroup private_functions
 * \param[in] array Указатель на структуру массива узлов \c itp_node_array.
 */
void itp_clear_node_array(itp_node_array_tp array);

/**
 * \brief Структура массива адресов.
 */
struct itp_address_array {
    /**
     * \brief Список виртуальных адресов.
     */
    uint8_t* data;
    /**
     * \brief Размер массива адресов.
     */
    uint8_t size;
    /**
     * \brief Количество адресов.
     */
    uint8_t count;
};

/**
 * \brief Тип структуры массива адресов <code>\link itp_address_array \endlink</code>.
 */
typedef struct itp_address_array itp_address_array_t;

/**
 * \brief Указатель на структуру массива адресов <code>\link itp_address_array \endlink</code>.
 */
typedef itp_address_array_t* itp_address_array_tp;

/**
 * \brief Инициализация массива виртуальных адресов.
 * \ingroup private_functions
 * \param[in] array Указатель на структуру массива адресов \c itp_address_array.
 */
void itp_init_address_array(itp_address_array_tp array);

/**
 * \brief Увеличить размер массива адресов.
 *
 * При добавлении новых узлов указатели будут записываться в заранее выделенную память.
 * \ingroup private_functions
 * \param[in] array Указатель на структуру массива адресов \c itp_address_array.
 * \param[in] size Количество дополнительных слотов.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_expand_address_array(itp_address_array_tp array, uint8_t size);

/**
 * \brief Проверить, существует ли адрес в массиве.
 * \ingroup private_functions
 * \param[in] array Указатель на структуру массива адресов \c itp_address_array.
 * \param[in] address Виртуальный адрес узла сети.
 * \return 1 - элемент найден, 0 - нет.
 */
uint8_t itp_is_array_contains(itp_address_array_tp array, uint8_t address);

/**
 * \brief Добавить виртуальный адрес в массив.
 *
 * Если память для массива не была выделена заранее, то происходит вызов функции <code>\link itp_malloc \endlink</code>.
 * \ingroup private_functions
 * \param[in] array Указатель на структуру массива адресов \c itp_address_array.
 * \param[in] address Виртуальный адрес узла сети.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_add_address_to_array(itp_address_array_tp array, uint8_t address);

/**
 * \brief Удалить указанный адрес из массива.
 * \ingroup private_functions
 * \param[in] array Указатель на структуру массива адресов \c itp_address_array.
 * \param[in] address Виртуальный адрес узла сети.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_remove_address_from_array(itp_address_array_tp array, uint8_t address);

/**
 * \brief Очистка массива виртуальных адресов.
 * \ingroup private_functions
 * \param[in] array Указатель на структуру массива адресов \c itp_address_array.
 */
void itp_clear_address_array(itp_address_array_tp array);

#endif // ITP_ARRAY_H_INCLUDED
