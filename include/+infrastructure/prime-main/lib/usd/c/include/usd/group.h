#ifndef USD_GROUP_H_INCLUDED
#define USD_GROUP_H_INCLUDED

/**
 * \file group.h
 * \brief Файл содержит описание группы универсальных устройств.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"

struct usd_device_item;

/**
 * \brief Тип структуры элемента группы устройств.
 */
typedef struct usd_device_item usd_device_item_t;

/**
 * \brief Указатель на структуру элемента группы устройств.
 */
typedef usd_device_item_t* usd_device_item_tp;

/**
 * \brief Структура элемента группы устройств.
 */
struct usd_device_item {
    /**
     * \brief Указатель на структуру устройства \c usd_device.
     */
    usd_device_tp device;
    /**
     * \brief Указатель на следующий элемент списка.
     */
    usd_device_item_tp next;
    /**
     * \brief Признак слабого связывания (0 - устройство удалится вместе со списком, 1 - нет).
     */
    uint8_t weak_ptr;
};

/**
 * \brief Структура группы устройств.
 */
struct usd_device_group {
    /**
     * \brief Указатель на первый элемент списка.
     */
    usd_device_item_tp first;
};

/**
 * \brief Тип структуры группы устройств.
 */
typedef struct usd_device_group usd_device_group_t;

/**
 * \brief Указатель на структуру группы устройств.
 */
typedef usd_device_group_t* usd_device_group_tp;

/**
 * \brief Инициализация элемента группы устройств \c usd_device_item.
 * \ingroup group_functions
 * \param[in] item Указатель на узел \c usd_device_item.
 * \param[in] device Указатель на устройство \c usd_device.
 * \param[in] weak_ptr Признак слабого связывания.
 */
void usd_init_device_item(usd_device_item_tp item, usd_device_tp device, uint8_t weak_ptr);

/**
 * \brief Инициализация группы устройств \c usd_device_group.
 * \ingroup group_functions
 * \param[in] group Указатель на группу устройств \c usd_device_group.
 */
void usd_init_device_group(usd_device_group_tp group);

/**
 * \brief Добавление устройства в группу.
 * \ingroup group_functions
 * \param[in] group Указатель на группу устройств \c usd_device_group.
 * \param[in] device Указатель на устройство \c usd_device.
 * \param[in] weak_ptr Признак слабого связывания.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_push_device(usd_device_group_tp group, usd_device_tp device, uint8_t weak_ptr);

/**
 * \brief Удаление указанного устройства из группы.
 *
 * Память, занятая структурой устройства не освобождается.
 * \ingroup group_functions
 * \param[in] group Указатель на группу устройств \c usd_device_group.
 * \param[in] device Указатель на устройство \c usd_device.
 * \return Указатель на следующий элемент группы, если он есть.
 */
usd_device_item_tp usd_erase_device(usd_device_group_tp group, usd_device_tp device);

/**
 * \brief Проверка существования указанного устройства.
 * \ingroup group_functions
 * \param[in] group Указатель на группу устройств \c usd_device_group.
 * \param[in] device Указатель на устройство \c usd_device.
 * \return 1 - устройство есть в списке, 0 - нет.
 */
uint8_t usd_device_is_exist(usd_device_group_tp group, usd_device_tp device);

/**
 * \brief Поиск устройства по его идентификатору.
 * \ingroup group_functions
 * \param[in] group Указатель на группу устройств \c usd_device_group.
 * \param[in] id Идентификатор искомого устройства.
 * \return Указатель на структуру универсального устройства или \c NULL.
 */
usd_device_tp usd_find_device_by_id(usd_device_group_tp group, uint16_t id);

/**
 * \brief Поиск устройства по идентификатору параметра.
 * \ingroup group_functions
 * \param[in] group Указатель на группу устройств \c usd_device_group.
 * \param[in] key Идентификатор искомого параметра.
 * \return Указатель на структуру универсального устройства или \c NULL.
 */
usd_device_tp usd_find_device_by_parameter(usd_device_group_tp group, uint8_t key);

/**
 * \brief Проверка группы на пустоту.
 * \ingroup group_functions
 * \param[in] group Указатель на группу устройств \c usd_device_group.
 * \return Если группа пуста - 0 (ноль), если нет - 1 (единицу).
 */
uint8_t usd_group_is_empty(usd_device_group_tp group);

/**
 * \brief Очистка группы устройств.
 * \ingroup group_functions
 * \param[in] group Указатель на группу устройств \c usd_device_group.
 */
void usd_clear_group(usd_device_group_tp group);

/**
 * \brief Очистка памяти, занятой группой устройств.
 * \ingroup group_functions
 * \param[in] group Указатель на группу устройств \c usd_device_group.
 */
void usd_free_group(usd_device_group_tp group);

#endif // USD_GROUP_H_INCLUDED
