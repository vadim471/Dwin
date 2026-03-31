#ifndef INITPLUS_TOOLS_NUMBER_H_INCLUDED
#define INITPLUS_TOOLS_NUMBER_H_INCLUDED

/**
 * \file
 */

#include <stdint.h>

/**
 * \brief Возвести 10 (десять) в указанную степень.
 * \ingroup tools_integral
 * \param[in] exponent Степень числа.
 * \return Результат операции типа \c int16_t.
 */
int16_t tools_pow10_int16(uint8_t exponent);

/**
 * \brief Возвести 10 (десять) в указанную степень.
 * \ingroup tools_integral
 * \param[in] exponent Степень числа.
 * \return Результат операции типа \c int32_t.
 */
int32_t tools_pow10_int32(uint8_t exponent);

/**
 * \brief Возвести 10 (десять) в указанную степень.
 * \ingroup tools_integral
 * \param[in] exponent Степень числа.
 * \return Результат операции типа \c int64_t.
 */
int64_t tools_pow10_int64(uint8_t exponent);

/**
 * \brief Возвести 10 (десять) в указанную степень.
 * \ingroup tools_integral
 * \param[in] exponent Степень числа.
 * \return Результат операции типа \c uint16_t.
 */
uint16_t tools_pow10_uint16(uint8_t exponent);

/**
 * \brief Возвести 10 (десять) в указанную степень.
 * \ingroup tools_integral
 * \param[in] exponent Степень числа.
 * \return Результат операции типа \c uint32_t.
 */
uint32_t tools_pow10_uint32(uint8_t exponent);

/**
 * \brief Возвести 10 (десять) в указанную степень.
 * \ingroup tools_integral
 * \param[in] exponent Степень числа.
 * \return Результат операции типа \c uint64_t.
 */
uint64_t tools_pow10_uint64(uint8_t exponent);

/**
 * \brief Преобразовать мантиссу и степень в число с плавающей точкой.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Результат операции типа \c double.
 */
double tools_int16_to_double(int16_t number, uint8_t exponent);

/**
 * \brief Преобразовать мантиссу и степень в число с плавающей точкой.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Результат операции типа \c double.
 */
double tools_int32_to_double(int32_t number, uint8_t exponent);

/**
 * \brief Преобразовать мантиссу и степень в число с плавающей точкой.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Результат операции типа \c double.
 */
double tools_int64_to_double(int64_t number, uint8_t exponent);

/**
 * \brief Преобразовать мантиссу и степень в число с плавающей точкой.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Результат операции типа \c double.
 */
double tools_uint16_to_double(uint16_t number, uint8_t exponent);

/**
 * \brief Преобразовать мантиссу и степень в число с плавающей точкой.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Результат операции типа \c double.
 */
double tools_uint32_to_double(uint32_t number, uint8_t exponent);

/**
 * \brief Преобразовать мантиссу и степень в число с плавающей точкой.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Результат операции типа \c double.
 */
double tools_uint64_to_double(uint64_t number, uint8_t exponent);

/**
 * \brief Преобразовать число с плавающей точкой в мантиссу с указанной степенью.
 * \ingroup tools_number
 * \param[in] value Число с плавающей точкой.
 * \param[in] exponent Степень числа.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Мантисса числа типа \c int16_t.
 */
int16_t tools_double_to_int16(double value, uint8_t exponent, uint8_t round);

/**
 * \brief Преобразовать число с плавающей точкой в мантиссу с указанной степенью.
 * \ingroup tools_number
 * \param[in] value Число с плавающей точкой.
 * \param[in] exponent Степень числа.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Мантисса числа типа \c int32_t.
 */
int32_t tools_double_to_int32(double value, uint8_t exponent, uint8_t round);

/**
 * \brief Преобразовать число с плавающей точкой в мантиссу с указанной степенью.
 * \ingroup tools_number
 * \param[in] value Число с плавающей точкой.
 * \param[in] exponent Степень числа.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Мантисса числа типа \c int64_t.
 */
int64_t tools_double_to_int64(double value, uint8_t exponent, uint8_t round);

/**
 * \brief Преобразовать число с плавающей точкой в мантиссу с указанной степенью.
 * \ingroup tools_number
 * \param[in] value Число с плавающей точкой.
 * \param[in] exponent Степень числа.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Мантисса числа типа \c uint16_t.
 */
uint16_t tools_double_to_uint16(double value, uint8_t exponent, uint8_t round);

/**
 * \brief Преобразовать число с плавающей точкой в мантиссу с указанной степенью.
 * \ingroup tools_number
 * \param[in] value Число с плавающей точкой.
 * \param[in] exponent Степень числа.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Мантисса числа типа \c uint32_t.
 */
uint32_t tools_double_to_uint32(double value, uint8_t exponent, uint8_t round);

/**
 * \brief Преобразовать число с плавающей точкой в мантиссу с указанной степенью.
 * \ingroup tools_number
 * \param[in] value Число с плавающей точкой.
 * \param[in] exponent Степень числа.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Мантисса числа типа \c uint64_t.
 */
uint64_t tools_double_to_uint64(double value, uint8_t exponent, uint8_t round);

/**
 * \brief Вернуть целую часть числа, заданного мантиссой и степенью.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Целую часть числа типа \c int16_t.
 */
int16_t tools_ipart_of_int16(int16_t number, uint8_t exponent);

/**
 * \brief Вернуть целую часть числа, заданного мантиссой и степенью.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Целую часть числа типа \c int32_t.
 */
int32_t tools_ipart_of_int32(int32_t number, uint8_t exponent);

/**
 * \brief Вернуть целую часть числа, заданного мантиссой и степенью.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Целую часть числа типа \c int64_t.
 */
int64_t tools_ipart_of_int64(int64_t number, uint8_t exponent);

/**
 * \brief Вернуть целую часть числа, заданного мантиссой и степенью.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Целую часть числа типа \c uint16_t.
 */
uint16_t tools_ipart_of_uint16(uint16_t number, uint8_t exponent);

/**
 * \brief Вернуть целую часть числа, заданного мантиссой и степенью.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Целую часть числа типа \c uint32_t.
 */
uint32_t tools_ipart_of_uint32(uint32_t number, uint8_t exponent);

/**
 * \brief Вернуть целую часть числа, заданного мантиссой и степенью.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Целую часть числа типа \c uint64_t.
 */
uint64_t tools_ipart_of_uint64(uint64_t number, uint8_t exponent);

/**
 * \brief Вернуть дробную часть числа, заданного мантиссой и степенью, в виде целого числа.
 *
 * Знак игнорируется (учитывается в целой части).
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Дробную часть числа типа \c int16_t.
 */
int16_t tools_fract_of_int16(int16_t number, uint8_t exponent);

/**
 * \brief Вернуть дробную часть числа, заданного мантиссой и степенью, в виде целого числа.
 *
 * Знак игнорируется (учитывается в целой части).
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Дробную часть числа типа \c int32_t.
 */
int32_t tools_fract_of_int32(int32_t number, uint8_t exponent);

/**
 * \brief Вернуть дробную часть числа, заданного мантиссой и степенью, в виде целого числа.
 *
 * Знак игнорируется (учитывается в целой части).
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Дробную часть числа типа \c int32_t.
 */
int64_t tools_fract_of_int64(int64_t number, uint8_t exponent);

/**
 * \brief Вернуть дробную часть числа, заданного мантиссой и степенью, в виде целого числа.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Дробную часть числа типа \c uint16_t.
 */
uint16_t tools_fract_of_uint16(uint16_t number, uint8_t exponent);

/**
 * \brief Вернуть дробную часть числа, заданного мантиссой и степенью, в виде целого числа.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Дробную часть числа типа \c uint16_t.
 */
uint32_t tools_fract_of_uint32(uint32_t number, uint8_t exponent);

/**
 * \brief Вернуть дробную часть числа, заданного мантиссой и степенью, в виде целого числа.
 * \ingroup tools_number
 * \param[in] number Мантисса числа.
 * \param[in] exponent Степень числа.
 * \return Дробную часть числа типа \c uint32_t.
 */
uint64_t tools_fract_of_uint64(uint64_t number, uint8_t exponent);

/**
 * \brief Преобразовать число, заданное целой и дробной частями, в мантиссу с той же степенью.
 *
 * Учитывается знак целой части.
 * \ingroup tools_number
 * \param[in] ipart Целая часть числа.
 * \param[in] fract Дробная часть числа.
 * \param[in] exponent Количество знаков дробной части.
 * \return Мантисса числа типа \c int16_t.
 */
int16_t tools_ifract_to_int16(int16_t ipart, uint16_t fract, uint8_t exponent);

/**
 * \brief Преобразовать число, заданное целой и дробной частями, в мантиссу с той же степенью.
 *
 * Учитывается знак целой части.
 * \ingroup tools_number
 * \param[in] ipart Целая часть числа.
 * \param[in] fract Дробная часть числа.
 * \param[in] exponent Количество знаков дробной части.
 * \return Мантисса числа типа \c int32_t.
 */
int32_t tools_ifract_to_int32(int32_t ipart, uint16_t fract, uint8_t exponent);

/**
 * \brief Преобразовать число, заданное целой и дробной частями, в мантиссу с той же степенью.
 *
 * Учитывается знак целой части.
 * \ingroup tools_number
 * \param[in] ipart Целая часть числа.
 * \param[in] fract Дробная часть числа.
 * \param[in] exponent Количество знаков дробной части.
 * \return Мантисса числа типа \c int64_t.
 */
int64_t tools_ifract_to_int64(int64_t ipart, uint32_t fract, uint8_t exponent);

/**
 * \brief Преобразовать число, заданное целой и дробной частями, в мантиссу с той же степенью.
 * \ingroup tools_number
 * \param[in] ipart Целая часть числа.
 * \param[in] fract Дробная часть числа.
 * \param[in] exponent Количество знаков дробной части.
 * \return Мантисса числа типа \c uint16_t.
 */
uint16_t tools_ifract_to_uint16(uint16_t ipart, uint16_t fract, uint8_t exponent);

/**
 * \brief Преобразовать число, заданное целой и дробной частями, в мантиссу с той же степенью.
 * \ingroup tools_number
 * \param[in] ipart Целая часть числа.
 * \param[in] fract Дробная часть числа.
 * \param[in] exponent Количество знаков дробной части.
 * \return Мантисса числа типа \c uint32_t.
 */
uint32_t tools_ifract_to_uint32(uint32_t ipart, uint16_t fract, uint8_t exponent);

/**
 * \brief Преобразовать число, заданное целой и дробной частями, в мантиссу с той же степенью.
 * \ingroup tools_number
 * \param[in] ipart Целая часть числа.
 * \param[in] fract Дробная часть числа.
 * \param[in] exponent Количество знаков дробной части.
 * \return Мантисса числа типа \c uint64_t.
 */
uint64_t tools_ifract_to_uint64(uint64_t ipart, uint32_t fract, uint8_t exponent);

#endif // INITPLUS_TOOLS_NUMBER_H_INCLUDED
