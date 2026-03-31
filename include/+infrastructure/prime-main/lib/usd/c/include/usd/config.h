#ifndef USD_CONFIG_H_INCLUDED
#define USD_CONFIG_H_INCLUDED

/**
 * \file config.h
 * \brief Файл содержит настройки библиотеки.
 *
 * Все макросы можно определить перед включением модулей библиотеки или передать компилятору напрямую (например, gcc -DKEY=VALUE -c *.c).
 */

#include <stdint.h>

#ifndef USD_MAX_DEVICES
/**
 * \brief Максимальное количество устройств в группе.
 */
#define USD_MAX_DEVICES 255
#endif // USD_MAX_DEVICES

#endif // USD_CONFIG_H_INCLUDED
