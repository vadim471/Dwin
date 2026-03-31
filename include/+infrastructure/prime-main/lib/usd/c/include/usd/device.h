#ifndef USD_DEVICE_H_INCLUDED
#define USD_DEVICE_H_INCLUDED

/**
 * \file device.h
 * \brief Файл содержит описание универсального устройства.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "variant.h"

/**
 * \brief Структура универсального устройства.
 */
struct usd_device {
    /**
     * \brief Идентификатор устройства.
     */
    uint16_t id;
    /**
     * \brief Массив параметров.
     */
    usd_parameter_tp* data;
    /**
     * \brief Размер массива параметров.
     */
    uint8_t size;
    /**
     * \brief Количество параметров в массиве.
     */
    uint8_t count;
};

/**
 * \brief Инициализация структуры универсального устройства.
 * \ingroup device_functions
 * \param[in] object Указатель на структуру универсального устройства \c usd_device.
 * \param[in] id Идентификатор устройства.
 */
void usd_init_device(usd_device_tp object, uint16_t id);

/**
 * \brief Увеличить размер массива параметров.
 *
 * При добавлении новых параметров указатели будут записываться в заранее выделенную память.
 * \ingroup device_functions
 * \param[in] object Указатель на структуру универсального устройства \c usd_device.
 * \param[in] size Количество дополнительных слотов.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_expand_device(usd_device_tp object, uint8_t size);

/**
 * \brief Добавить параметр в массив.
 *
 * Если память для массива не была выделена заранее, то происходит вызов функции <code>\link usd_malloc \endlink</code>.
 * \ingroup device_functions
 * \param[in] object Указатель на структуру универсального устройства \c usd_device.
 * \param[in] parameter Указатель на структуру универсального параметра \c usd_parameter.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_add_parameter(usd_device_tp object, usd_parameter_tp parameter);

/**
 * \brief Очистка списка параметров универсального устройства.
 * \ingroup device_functions
 * \param[in] object Указатель на структуру универсального устройства \c usd_device.
 */
void usd_clear_device(usd_device_tp object);

/**
 * \brief Очистка памяти, занимаемой полями структуры универсального устройства.
 * \ingroup device_functions
 * \param[in] object Указатель на структуру универсального устройства \c usd_device.
 */
void usd_dispose_device(usd_device_tp object);

/**
 * \brief Очистка памяти, занимаемой структурой универсального устройства.
 * \ingroup device_functions
 * \param[in] object Указатель на структуру универсального устройства \c usd_device.
 */
void usd_free_device(usd_device_tp object);

/**
 * \brief Создать параметр типа \c dec32.
 * \ingroup device_functions
 * \param[in] device Указатель на структуру универсального устройства \c usd_device.
 * \param[in] key Идентификатор параметра.
 * \param[in] read_only Доступ только для чтения.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_create_dec32_parameter(usd_device_tp device, uint8_t key, uint8_t read_only);

/**
 * \brief Создать параметр типа \c dec32 для функции.
 * \ingroup device_functions
 * \param[in] device Указатель на структуру универсального устройства \c usd_device.
 * \param[in] key Идентификатор параметра.
 * \param[in] object Связываемый объект.
 * \param[in] eparam Дополнительный параметр.
 * \param[in] read Указатель на функцию чтения значения.
 * \param[in] write Указатель на функцию записи значения (если допустимо).
 * \param[in] read_only Доступ только для чтения.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_bind_dec32_parameter(usd_device_tp device, uint8_t key, void* object, size_t eparam, usd_get_dec32_parameter_fp read, usd_set_dec32_parameter_fp write, uint8_t read_only);

/**
 * \brief Создать параметр типа \c dec64.
 * \ingroup device_functions
 * \param[in] device Указатель на структуру универсального устройства \c usd_device.
 * \param[in] key Идентификатор параметра.
 * \param[in] read_only Доступ только для чтения.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_create_dec64_parameter(usd_device_tp device, uint8_t key, uint8_t read_only);

/**
 * \brief Создать параметр типа \c dec64 для функции.
 * \ingroup device_functions
 * \param[in] device Указатель на структуру универсального устройства \c usd_device.
 * \param[in] key Идентификатор параметра.
 * \param[in] object Связываемый объект.
 * \param[in] eparam Дополнительный параметр.
 * \param[in] read Указатель на функцию чтения значения.
 * \param[in] write Указатель на функцию записи значения (если допустимо).
 * \param[in] read_only Доступ только для чтения.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_bind_dec64_parameter(usd_device_tp device, uint8_t key, void* object, size_t eparam, usd_get_dec64_parameter_fp read, usd_set_dec64_parameter_fp write, uint8_t read_only);

/**
 * \brief Создать параметр типа \c bool.
 * \ingroup device_functions
 * \param[in] device Указатель на структуру универсального устройства \c usd_device.
 * \param[in] key Идентификатор параметра.
 * \param[in] read_only Доступ только для чтения.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_create_bool_parameter(usd_device_tp device, uint8_t key, uint8_t read_only);

/**
 * \brief Создать параметр типа \c bool для функции.
 * \ingroup device_functions
 * \param[in] device Указатель на структуру универсального устройства \c usd_device.
 * \param[in] key Идентификатор параметра.
 * \param[in] object Связываемый объект.
 * \param[in] eparam Дополнительный параметр.
 * \param[in] read Указатель на функцию чтения значения.
 * \param[in] write Указатель на функцию записи значения (если допустимо).
 * \param[in] read_only Доступ только для чтения.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_bind_bool_parameter(usd_device_tp device, uint8_t key, void* object, size_t eparam, usd_get_bool_parameter_fp read, usd_set_bool_parameter_fp write, uint8_t read_only);

/**
 * \brief Проверить существование параметра в устройстве.
 * \ingroup device_functions
 * \param[in] device Указатель на структуру универсального устройства \c usd_device.
 * \param[in] key Идентификатор параметра.
 * \return Признак существования параметра (1 - существует, 0 - нет).
 */
uint8_t usd_is_has_parameter(usd_device_tp device, uint8_t key);

/**
 * \brief Поиск параметра по идентификатору.
 * \ingroup device_functions
 * \param[in] device Указатель на структуру универсального устройства \c usd_device.
 * \param[in] key Идентификатор параметра.
 * \return Указатель на структуру универсального параметра или \c NULL.
 */
usd_parameter_tp usd_find_parameter_by_key(usd_device_tp device, uint8_t key);

/**
 * \brief Получить значение параметра.
 * \ingroup device_functions
 * \param[in] device Указатель на структуру универсального устройства \c usd_device.
 * \param[in] key Идентификатор параметра.
 * \param[in] value Указатель на переменную, куда будет записана мантисса числа.
 * \param[in] exponent Указатель на переменную, куда будет записана экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_get_parameter(usd_device_tp device, uint8_t key, int32_t* value, uint8_t* exponent);

/**
 * \brief Получить значение параметра (64-разрядная версия).
 * \ingroup device_functions
 * \param[in] device Указатель на структуру универсального устройства \c usd_device.
 * \param[in] key Идентификатор параметра.
 * \param[in] value Указатель на переменную, куда будет записана мантисса числа.
 * \param[in] exponent Указатель на переменную, куда будет записана экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_get_parameter_64(usd_device_tp device, uint8_t key, int64_t* value, uint8_t* exponent);

/**
 * \brief Записать значение параметра.
 * \ingroup device_functions
 * \param[in] device Указатель на структуру универсального устройства \c usd_device.
 * \param[in] key Идентификатор параметра.
 * \param[in] value Мантисса числа.
 * \param[in] exponent Экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_set_parameter(usd_device_tp device, uint8_t key, int32_t value, uint8_t exponent);

/**
 * \brief Записать значение параметра (64-разрядная версия).
 * \ingroup device_functions
 * \param[in] device Указатель на структуру универсального устройства \c usd_device.
 * \param[in] key Идентификатор параметра.
 * \param[in] value Мантисса числа.
 * \param[in] exponent Экспонента числа.
 * \return Результат выполнения операции <code>\link usd_error_code_t \endlink</code>.
 */
usd_error_code_t usd_set_parameter_64(usd_device_tp device, uint8_t key, int64_t value, uint8_t exponent);

#endif // USD_DEVICE_H_INCLUDED
