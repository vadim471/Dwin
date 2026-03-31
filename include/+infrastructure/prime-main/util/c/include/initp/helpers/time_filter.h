#ifndef INITPLUS_HELPERS_TIME_FILTER_H_INCLUDED
#define INITPLUS_HELPERS_TIME_FILTER_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

/**
 * \file
 */

/**
 * \brief Структура для хранения фильтра по времени.
 */
typedef struct {
    uint64_t time;
    uint32_t delay;
} tools_time_filter_t;

/**
 * \brief Выставить всем фильтрам указанную задержку.
 * \param[in] array Указатель на массив \c tools_time_filter_t.
 * \param[in] size Размер массива.
 * \param[in] delay Задержка в миллисекундах.
 */
void tools_time_filter_init(tools_time_filter_t* array, size_t size, uint32_t delay);

/**
 * \brief Проверить время для указанного фильтра.
 * \param[in] array Указатель на массив \c tools_time_filter_t.
 * \param[in] index Индекс фильтра.
 * \param[in] time Текущее время в миллисекундах.
 * \return Возвращает 1, если с момента предыдущего вызова прошло дольше \b delay, иначе возвращает 0.
 */
uint8_t tools_time_filter_check(tools_time_filter_t* array, size_t index, uint64_t time);

#endif // INITPLUS_HELPERS_TIME_FILTER_H_INCLUDED

