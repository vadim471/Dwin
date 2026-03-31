#ifndef INITPLUS_TOOLS_DECIMAL_H_INCLUDED
#define INITPLUS_TOOLS_DECIMAL_H_INCLUDED

/**
 * \file
 */

#include <stdint.h>

/**
 * \brief Изменить степень мантиссы числа с округлением.
 * \ingroup tools_decimal
 * \param[in] number Мантисса числа.
 * \param[in] source Исходная степень числа.
 * \param[in] target Целевая степень числа.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат операции типа \c int16_t.
 */
int16_t tools_round_int16(int16_t number, uint8_t source, uint8_t target, uint8_t round);

/**
 * \brief Изменить степень мантиссы числа с округлением.
 * \ingroup tools_decimal
 * \param[in] number Мантисса числа.
 * \param[in] source Исходная степень числа.
 * \param[in] target Целевая степень числа.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат операции типа \c int32_t.
 */
int32_t tools_round_int32(int32_t number, uint8_t source, uint8_t target, uint8_t round);

/**
 * \brief Изменить степень мантиссы числа с округлением.
 * \ingroup tools_decimal
 * \param[in] number Мантисса числа.
 * \param[in] source Исходная степень числа.
 * \param[in] target Целевая степень числа.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат операции типа \c int64_t.
 */
int64_t tools_round_int64(int64_t number, uint8_t source, uint8_t target, uint8_t round);

/**
 * \brief Изменить степень мантиссы числа с округлением.
 * \ingroup tools_decimal
 * \param[in] number Мантисса числа.
 * \param[in] source Исходная степень числа.
 * \param[in] target Целевая степень числа.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат операции типа \c uint16_t.
 */
uint16_t tools_round_uint16(uint16_t number, uint8_t source, uint8_t target, uint8_t round);

/**
 * \brief Изменить степень мантиссы числа с округлением.
 * \ingroup tools_decimal
 * \param[in] number Мантисса числа.
 * \param[in] source Исходная степень числа.
 * \param[in] target Целевая степень числа.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат операции типа \c uint32_t.
 */
uint32_t tools_round_uint32(uint32_t number, uint8_t source, uint8_t target, uint8_t round);

/**
 * \brief Изменить степень мантиссы числа с округлением.
 * \ingroup tools_decimal
 * \param[in] number Мантисса числа.
 * \param[in] source Исходная степень числа.
 * \param[in] target Целевая степень числа.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат операции типа \c uint64_t.
 */
uint64_t tools_round_uint64(uint64_t number, uint8_t source, uint8_t target, uint8_t round);

/**
 * \brief Сумма чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса первого слагаемого.
 * \param[in] ae Степень первого слагаемого.
 * \param[in] b Мантисса второго слагаемого.
 * \param[in] be Степень второго слагаемого.
 * \param[in] e Степень суммы.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат сложения, приведённый к степени \b e.
 */
uint16_t tools_uint16_addition(uint16_t a, uint8_t ae, uint16_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Сумма чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса первого слагаемого.
 * \param[in] ae Степень первого слагаемого.
 * \param[in] b Мантисса второго слагаемого.
 * \param[in] be Степень второго слагаемого.
 * \param[in] e Степень суммы.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат сложения, приведённый к степени \b e.
 */
uint32_t tools_uint32_addition(uint32_t a, uint8_t ae, uint32_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Сумма чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса первого слагаемого.
 * \param[in] ae Степень первого слагаемого.
 * \param[in] b Мантисса второго слагаемого.
 * \param[in] be Степень второго слагаемого.
 * \param[in] e Степень суммы.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат сложения, приведённый к степени \b e.
 */
uint64_t tools_uint64_addition(uint64_t a, uint8_t ae, uint64_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Сумма чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса первого слагаемого.
 * \param[in] ae Степень первого слагаемого.
 * \param[in] b Мантисса второго слагаемого.
 * \param[in] be Степень второго слагаемого.
 * \param[in] e Степень суммы.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат сложения, приведённый к степени \b e.
 */
int16_t tools_int16_addition(int16_t a, uint8_t ae, int16_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Сумма чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса первого слагаемого.
 * \param[in] ae Степень первого слагаемого.
 * \param[in] b Мантисса второго слагаемого.
 * \param[in] be Степень второго слагаемого.
 * \param[in] e Степень суммы.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат сложения, приведённый к степени \b e.
 */
int32_t tools_int32_addition(int32_t a, uint8_t ae, int32_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Сумма чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса первого слагаемого.
 * \param[in] ae Степень первого слагаемого.
 * \param[in] b Мантисса второго слагаемого.
 * \param[in] be Степень второго слагаемого.
 * \param[in] e Степень суммы.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат сложения, приведённый к степени \b e.
 */
int64_t tools_int64_addition(int64_t a, uint8_t ae, int64_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Разность чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса уменьшаемого.
 * \param[in] ae Степень уменьшаемого.
 * \param[in] b Мантисса вычитаемого.
 * \param[in] be Степень вычитаемого.
 * \param[in] e Степень разности.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат вычитания, приведённый к степени \b e.
 */
uint16_t tools_uint16_subtraction(uint16_t a, uint8_t ae, uint16_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Разность чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса уменьшаемого.
 * \param[in] ae Степень уменьшаемого.
 * \param[in] b Мантисса вычитаемого.
 * \param[in] be Степень вычитаемого.
 * \param[in] e Степень разности.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат вычитания, приведённый к степени \b e.
 */
uint32_t tools_uint32_subtraction(uint32_t a, uint8_t ae, uint32_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Разность чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса уменьшаемого.
 * \param[in] ae Степень уменьшаемого.
 * \param[in] b Мантисса вычитаемого.
 * \param[in] be Степень вычитаемого.
 * \param[in] e Степень разности.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат вычитания, приведённый к степени \b e.
 */
uint64_t tools_uint64_subtraction(uint64_t a, uint8_t ae, uint64_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Разность чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса уменьшаемого.
 * \param[in] ae Степень уменьшаемого.
 * \param[in] b Мантисса вычитаемого.
 * \param[in] be Степень вычитаемого.
 * \param[in] e Степень разности.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат вычитания, приведённый к степени \b e.
 */
int16_t tools_int16_subtraction(int16_t a, uint8_t ae, int16_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Разность чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса уменьшаемого.
 * \param[in] ae Степень уменьшаемого.
 * \param[in] b Мантисса вычитаемого.
 * \param[in] be Степень вычитаемого.
 * \param[in] e Степень разности.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат вычитания, приведённый к степени \b e.
 */
int32_t tools_int32_subtraction(int32_t a, uint8_t ae, int32_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Разность чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса уменьшаемого.
 * \param[in] ae Степень уменьшаемого.
 * \param[in] b Мантисса вычитаемого.
 * \param[in] be Степень вычитаемого.
 * \param[in] e Степень разности.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат вычитания, приведённый к степени \b e.
 */
int64_t tools_int64_subtraction(int64_t a, uint8_t ae, int64_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Произведение чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса первого множителя.
 * \param[in] ae Степень первого множителя.
 * \param[in] b Мантисса второго множителя.
 * \param[in] be Степень второго множителя.
 * \param[in] e Степень произведения.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат умножения, приведённый к степени \b e.
 */
uint16_t tools_uint16_multiplication(uint16_t a, uint8_t ae, uint16_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Произведение чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса первого множителя.
 * \param[in] ae Степень первого множителя.
 * \param[in] b Мантисса второго множителя.
 * \param[in] be Степень второго множителя.
 * \param[in] e Степень произведения.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат умножения, приведённый к степени \b e.
 */
uint32_t tools_uint32_multiplication(uint32_t a, uint8_t ae, uint32_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Произведение чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса первого множителя.
 * \param[in] ae Степень первого множителя.
 * \param[in] b Мантисса второго множителя.
 * \param[in] be Степень второго множителя.
 * \param[in] e Степень произведения.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат умножения, приведённый к степени \b e.
 */
uint64_t tools_uint64_multiplication(uint64_t a, uint8_t ae, uint64_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Произведение чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса первого множителя.
 * \param[in] ae Степень первого множителя.
 * \param[in] b Мантисса второго множителя.
 * \param[in] be Степень второго множителя.
 * \param[in] e Степень произведения.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат умножения, приведённый к степени \b e.
 */
int16_t tools_int16_multiplication(int16_t a, uint8_t ae, int16_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Произведение чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса первого множителя.
 * \param[in] ae Степень первого множителя.
 * \param[in] b Мантисса второго множителя.
 * \param[in] be Степень второго множителя.
 * \param[in] e Степень произведения.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат умножения, приведённый к степени \b e.
 */
int32_t tools_int32_multiplication(int32_t a, uint8_t ae, int32_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Произведение чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса первого множителя.
 * \param[in] ae Степень первого множителя.
 * \param[in] b Мантисса второго множителя.
 * \param[in] be Степень второго множителя.
 * \param[in] e Степень произведения.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат умножения, приведённый к степени \b e.
 */
int64_t tools_int64_multiplication(int64_t a, uint8_t ae, int64_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Частное чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса делимого.
 * \param[in] ae Степень делимого.
 * \param[in] b Мантисса делителя.
 * \param[in] be Степень делителя.
 * \param[in] e Степень частного.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат деления, приведённый к степени \b e.
 */
uint16_t tools_uint16_division(uint16_t a, uint8_t ae, uint16_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Частное чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса делимого.
 * \param[in] ae Степень делимого.
 * \param[in] b Мантисса делителя.
 * \param[in] be Степень делителя.
 * \param[in] e Степень частного.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат деления, приведённый к степени \b e.
 */
uint32_t tools_uint32_division(uint32_t a, uint8_t ae, uint32_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Частное чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса делимого.
 * \param[in] ae Степень делимого.
 * \param[in] b Мантисса делителя.
 * \param[in] be Степень делителя.
 * \param[in] e Степень частного.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат деления, приведённый к степени \b e.
 */
uint64_t tools_uint64_division(uint64_t a, uint8_t ae, uint64_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Частное чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса делимого.
 * \param[in] ae Степень делимого.
 * \param[in] b Мантисса делителя.
 * \param[in] be Степень делителя.
 * \param[in] e Степень частного.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат деления, приведённый к степени \b e.
 */
int16_t tools_int16_division(int16_t a, uint8_t ae, int16_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Частное чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса делимого.
 * \param[in] ae Степень делимого.
 * \param[in] b Мантисса делителя.
 * \param[in] be Степень делителя.
 * \param[in] e Степень частного.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат деления, приведённый к степени \b e.
 */
int32_t tools_int32_division(int32_t a, uint8_t ae, int32_t b, uint8_t be, uint8_t e, uint8_t round);

/**
 * \brief Частное чисел, заданных мантиссой и степенью десяти.
 * \ingroup tools_decimal
 * \param[in] a Мантисса делимого.
 * \param[in] ae Степень делимого.
 * \param[in] b Мантисса делителя.
 * \param[in] be Степень делителя.
 * \param[in] e Степень частного.
 * \param[in] round Тип округления (0 - вниз, 1 - вверх, 2 - до ближайшего).
 * \return Результат деления, приведённый к степени \b e.
 */
int64_t tools_int64_division(int64_t a, uint8_t ae, int64_t b, uint8_t be, uint8_t e, uint8_t round);

#endif // INITPLUS_TOOLS_DECIMAL_H_INCLUDED
