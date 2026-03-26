#ifndef ITP_FRAME_H_INCLUDED
#define ITP_FRAME_H_INCLUDED

/**
 * \file frame.h
 * \brief Файл содержит функции для работы с кадром.
 */

#include "config.h"
#include "types.h"
#include "error_code.h"
#include "symbol.h"

/**
 * \brief Структура кадра.
 */
struct itp_frame {
    /**
     * \brief Версия протокола, использованная при упаковке кадра.
     */
    uint8_t version;
    /**
     * \brief Адрес отправителя.
     */
    uint8_t from;
    /**
     * \brief Адрес получателя.
     */
    uint8_t to;
    /**
     * \brief Команда.
     */
    uint16_t command;
    /**
     * \brief Статус выполнения запроса (без флагов).
     */
    uint8_t status;
    /**
     * \brief Код ошибки из тела пакета.
     */
    uint16_t error;
    /**
     * \brief Данные.
     */
    itp_byte_t* data;
    /**
     * \brief Размер массива данных.
     */
    itp_size_t length;
    /**
     * \brief Номер пакета или 0 (ноль).
     *
     * При отправке номера в пакете, он будет передан в ответе.
     */
    uint16_t order;
    /**
     * \brief Смещение чтения/записи параметров.
     */
    itp_size_t offset;
};

/**
 * \brief Инициализация структуры кадра \c itp_frame.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] command Команда.
 */
void itp_init_frame(itp_frame_tp frame, uint16_t command);

/**
 * \brief Выделение памяти и инициализация структуры кадра \c itp_frame.
 * \ingroup frame_functions
 * \param[in] command Команда.
 * \return Указатель на кадр \c itp_frame.
 */
itp_frame_tp itp_create_frame(uint16_t command);

/**
 * \brief Создание копии указанного кадра.
 * \ingroup frame_functions
 * \param[in] frame Указатель на исходный кадр \c itp_frame.
 * \return Указатель на копию \c itp_frame.
 */
itp_frame_tp itp_copy_frame(itp_frame_tp frame);

/**
 * \brief Освобождение динамически выделенной для кадра памяти.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 */
void itp_free_frame(itp_frame_tp frame);

/**
 * \brief Выделить память для буфера кадра \c itp_frame.
 *
 * Записываемые данные будут занимать выделенную память, что минимизирует количество вызовов \c itp_malloc.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] size Размер буфера.
 */
itp_error_code_t itp_prepare_frame(itp_frame_tp frame, itp_size_t size);

/**
 * \brief Расширить память для буфера кадра \c itp_frame.
 *
 * Записываемые данные будут занимать выделенную память, что минимизирует количество вызовов \c itp_malloc.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] size Размер буфера.
 */
itp_error_code_t itp_expand_frame(itp_frame_tp frame, itp_size_t size);

/**
 * \brief Освободить буфер кадра \c itp_frame.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 */
void itp_dispose_frame(itp_frame_tp frame);

/**
 * \brief Вывести в \c stdout содержимое кадра.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 */
void itp_dump_frame(itp_frame_tp frame);

/**
 * \brief Подготовить кадр к чтению данных.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 */
void itp_frame_prepare_to_read(itp_frame_tp frame);

/**
 * \brief Подготовить кадр к записи данных.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 */
void itp_frame_prepare_to_write(itp_frame_tp frame);

/**
 * \brief Проверить наличие параметров в кадре.
 *
 * 1 - в кадре ещё есть непрочитанные параметры, 0 - нет.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 */
uint8_t itp_frame_has_more(itp_frame_tp frame);

/**
 * \brief Размер оставшихся данных.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 */
itp_size_t itp_frame_remain(itp_frame_tp frame);

/**
 * \brief Прочитать параметр указанного размера из кадра \c itp_frame.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] value Указатель на переменную, куда будет записано значение.
 * \param[in] size Размер параметра в байтах.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_value(itp_frame_tp frame, void* value, uint16_t size);

/**
 * \brief Прочитать параметр размером 1 байт из кадра \c itp_frame.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] value Указатель на переменную, куда будет записано значение.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_value_1b(itp_frame_tp frame, void* value);

/**
 * \brief Прочитать параметр размером 2 байта из кадра \c itp_frame.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] value Указатель на переменную, куда будет записано значение.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_value_2b(itp_frame_tp frame, void* value);

/**
 * \brief Прочитать параметр размером 4 байта из кадра \c itp_frame.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] value Указатель на переменную, куда будет записано значение.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_value_4b(itp_frame_tp frame, void* value);

/**
 * \brief Прочитать параметр размером 8 байт из кадра \c itp_frame.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] value Указатель на переменную, куда будет записано значение.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_value_8b(itp_frame_tp frame, void* value);

/**
 * \brief Прочитать строку из кадра \c itp_frame.
 *
 * Строка будет дополнена символом 0x0 в конце. Память будет выделена динамически вызовом <code>\link itp_malloc \endlink</code>.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] value Указатель на переменную, куда будет записано значение.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_string(itp_frame_tp frame, char** string);

/**
 * \brief Прочитать строку из кадра \c itp_frame.
 *
 * Строка будет дополнена символом 0x0 в конце. Память будет выделена динамически вызовом <code>\link itp_malloc \endlink</code>.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] value Указатель на переменную, куда будет записано значение.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_wide_string(itp_frame_tp frame, char** string);

/**
 * \brief Прочитать массив параметров из кадра \c itp_frame.
 *
 * Память будет выделена вызовом <code>\link itp_malloc \endlink</code>.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на переменную, куда будет записано значение.
 * \param[in] length Указатель на переменную, куда будет записан размер массива.
 * \param[in] size Размер параметров в байтах.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_array(itp_frame_tp frame, void** array, uint16_t* length, uint16_t size);

/**
 * \brief Прочитать массив параметров размером 1 байт из кадра \c itp_frame.
 *
 * Память для массива будет выделена динамически вызовом <code>\link itp_malloc \endlink</code>.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на переменную, куда будет записано значение.
 * \param[in] length Указатель на переменную, куда будет записан размер массива.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_array_1b(itp_frame_tp frame, void** array, uint16_t* length);

/**
 * \brief Прочитать массив параметров размером 2 байта из кадра \c itp_frame.
 *
 * Память для массива будет выделена динамически вызовом <code>\link itp_malloc \endlink</code>.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на переменную, куда будет записано значение.
 * \param[in] length Указатель на переменную, куда будет записан размер массива.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_array_2b(itp_frame_tp frame, void** array, uint16_t* length);

/**
 * \brief Прочитать массив параметров размером 4 байта из кадра \c itp_frame.
 *
 * Память для массива будет выделена динамически вызовом <code>\link itp_malloc \endlink</code>.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на переменную, куда будет записано значение.
 * \param[in] length Указатель на переменную, куда будет записан размер массива.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_array_4b(itp_frame_tp frame, void** array, uint16_t* length);

/**
 * \brief Прочитать массив параметров размером 8 байт из кадра \c itp_frame.
 *
 * Память для массива будет выделена динамически вызовом <code>\link itp_malloc \endlink</code>.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на переменную, куда будет записано значение.
 * \param[in] length Указатель на переменную, куда будет записан размер массива.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_array_8b(itp_frame_tp frame, void** array, uint16_t* length);

/**
 * \brief Прочитать массив параметров из кадра \c itp_frame.
 *
 * Память будет выделена вызовом <code>\link itp_malloc \endlink</code>.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на переменную, куда будет записано значение.
 * \param[in] length Указатель на переменную, куда будет записан размер массива.
 * \param[in] size Размер параметров в байтах.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_wide_array(itp_frame_tp frame, void** array, uint16_t* length, uint8_t size);

/**
 * \brief Прочитать массив параметров размером 1 байт из кадра \c itp_frame.
 *
 * Память для массива будет выделена динамически вызовом <code>\link itp_malloc \endlink</code>.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на переменную, куда будет записано значение.
 * \param[in] length Указатель на переменную, куда будет записан размер массива.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_wide_array_1b(itp_frame_tp frame, void** array, uint16_t* length);

/**
 * \brief Прочитать массив параметров размером 2 байта из кадра \c itp_frame.
 *
 * Память для массива будет выделена динамически вызовом <code>\link itp_malloc \endlink</code>.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на переменную, куда будет записано значение.
 * \param[in] length Указатель на переменную, куда будет записан размер массива.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_wide_array_2b(itp_frame_tp frame, void** array, uint16_t* length);

/**
 * \brief Прочитать массив параметров размером 4 байта из кадра \c itp_frame.
 *
 * Память для массива будет выделена динамически вызовом <code>\link itp_malloc \endlink</code>.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на переменную, куда будет записано значение.
 * \param[in] length Указатель на переменную, куда будет записан размер массива.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_wide_array_4b(itp_frame_tp frame, void** array, uint16_t* length);

/**
 * \brief Прочитать массив параметров размером 8 байт из кадра \c itp_frame.
 *
 * Память для массива будет выделена динамически вызовом <code>\link itp_malloc \endlink</code>.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на переменную, куда будет записано значение.
 * \param[in] length Указатель на переменную, куда будет записан размер массива.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_read_wide_array_8b(itp_frame_tp frame, void** array, uint16_t* length);

/**
 * \brief Записать параметр указанного размера в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] value Указатель на записываемое значение.
 * \param[in] size Размер записываемого значения.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_value(itp_frame_tp frame, const void* value, uint8_t size);

/**
 * \brief Записать параметр размером 1 байт в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] value Указатель на записываемое значение.
 * \param[in] size Размер записываемого значения.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_value_1b(itp_frame_tp frame, void* value);

/**
 * \brief Записать параметр размером 2 байта в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] value Указатель на записываемое значение.
 * \param[in] size Размер записываемого значения.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_value_2b(itp_frame_tp frame, void* value);

/**
 * \brief Записать параметр размером 4 байта в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] value Указатель на записываемое значение.
 * \param[in] size Размер записываемого значения.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_value_4b(itp_frame_tp frame, void* value);

/**
 * \brief Записать параметр размером 8 байт в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] value Указатель на записываемое значение.
 * \param[in] size Размер записываемого значения.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_value_8b(itp_frame_tp frame, void* value);

/**
 * \brief Записать строку в кадр \c itp_frame.
 *
 * Строка должна быть дополнена символом 0x0 в конце.<br>
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] string Указатель на строку.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_string(itp_frame_tp frame, const char* string);

/**
 * \brief Записать строку в кадр \c itp_frame.
 *
 * Строка должна быть дополнена символом 0x0 в конце.<br>
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] string Указатель на строку.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_wide_string(itp_frame_tp frame, const char* string);

/**
 * \brief Записать массив с элементами указанного размера в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на массив данных.
 * \param[in] length Размер массива данных.
 * \param[in] size Размер элемента данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_array(itp_frame_tp frame, const void* array, uint16_t length, uint16_t size);

/**
 * \brief Записать массив с элементами размером 1 байт в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на массив данных.
 * \param[in] length Размер массива данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_array_1b(itp_frame_tp frame, const void* array, uint16_t length);

/**
 * \brief Записать массив с элементами размером 2 байта в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на массив данных.
 * \param[in] length Размер массива данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_array_2b(itp_frame_tp frame, const void* array, uint16_t length);

/**
 * \brief Записать массив с элементами размером 4 байта в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на массив данных.
 * \param[in] length Размер массива данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_array_4b(itp_frame_tp frame, const void* array, uint16_t length);

/**
 * \brief Записать массив с элементами размером 8 байт в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на массив данных.
 * \param[in] length Размер массива данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_array_8b(itp_frame_tp frame, const void* array, uint16_t length);

/**
 * \brief Записать массив с элементами указанного размера в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на массив данных.
 * \param[in] length Размер массива данных.
 * \param[in] size Размер элемента данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_wide_array(itp_frame_tp frame, const void* array, uint16_t length, uint8_t size);

/**
 * \brief Записать массив с элементами размером 1 байт в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на массив данных.
 * \param[in] length Размер массива данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_wide_array_1b(itp_frame_tp frame, const void* array, uint16_t length);

/**
 * \brief Записать массив с элементами размером 2 байта в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на массив данных.
 * \param[in] length Размер массива данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_wide_array_2b(itp_frame_tp frame, const void* array, uint16_t length);

/**
 * \brief Записать массив с элементами размером 4 байта в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на массив данных.
 * \param[in] length Размер массива данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_wide_array_4b(itp_frame_tp frame, const void* array, uint16_t length);

/**
 * \brief Записать массив с элементами размером 8 байт в кадр \c itp_frame.
 *
 * Если в буфере кадра недостаточно места, память будет выделена заново.
 * \ingroup frame_functions
 * \param[in] frame Указатель на кадр \c itp_frame.
 * \param[in] array Указатель на массив данных.
 * \param[in] length Размер массива данных.
 * \return Результат выполнения операции <code>\link itp_error_code_t \endlink</code>.
 */
itp_error_code_t itp_frame_write_wide_array_8b(itp_frame_tp frame, const void* array, uint16_t length);

#endif // ITP_FRAME_H_INCLUDED
