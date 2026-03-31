#ifndef INITPLUS_SYSTEM_ATEXIT_HPP_INCLUDED
#define INITPLUS_SYSTEM_ATEXIT_HPP_INCLUDED

#include <cstddef>

namespace initp {
namespace system {

typedef void(*at_exit_f)(void);
typedef void(*at_exit_ctx_f)(void*);

void at_exit(at_exit_ctx_f f, void* context);

/**
 * \brief Выполнить функцию перед выходом из приложения.
 *
 * Обёртка над вызовом std::atexit.
 * \param[in] f Вызываемая функция \c at_exit_ctx_f.
 * \param[in] context Контекст вызова, если необходим.
 * \param[in] priority Приоритет. Чем выше, тем раньше вызовется.
 */
void at_exit(at_exit_ctx_f f, void* context, size_t priority);

void at_exit(at_exit_f f);

void at_exit(at_exit_f f, size_t priority);

/**
 * \brief Проверяет, что началась очистка перед выходом.
 */
bool is_exit_started(void);

}}

#endif // INITPLUS_SYSTEM_ATEXIT_HPP_INCLUDED
