#ifndef INITPLUS_PLATFORM_CONSOLE_H_INCLUDED
#define INITPLUS_PLATFORM_CONSOLE_H_INCLUDED

/**
 * \file
 */

#define SYS_COLOR_BLACK "\e[30m"
#define SYS_COLOR_RED "\e[31m"
#define SYS_COLOR_GREEN "\e[32m"
#define SYS_COLOR_YELLOW "\e[93m"
#define SYS_COLOR_BLUE "\e[34m"
#define SYS_COLOR_MAGENTA "\e[35m"
#define SYS_COLOR_CYAN "\e[36m"
#define SYS_COLOR_LIGHT_GRAY "\e[37m"
#define SYS_COLOR_DARK_GRAY "\e[38m"
#define SYS_COLOR_DEFAULT "\e[39m"
#define SYS_COLOR_WHITE "\e[97m"

#define SYS_STYLE_NORMAL "\e[0m"
#define SYS_STYLE_BOLD "\e[1m"
#define SYS_STYLE_DIM "\e[2m"
#define SYS_STYLE_UNDERLINED "\e[4m"
#define SYS_STYLE_BLINK "\e[5m"
#define SYS_STYLE_INVERTED "\e[7m"
#define SYS_STYLE_HIDDEN "\e[8m"

/**
 * \brief Очистить экран консоли.
 * \ingroup platform_console
 */
void sys_clear_screen(void);

/**
 * \brief Очистить текущую строку в консоли.
 * \ingroup platform_console
 */
void sys_clear_line(void);

/**
 * \brief Установить консольный курсор в указанную позицию.
 * \ingroup platform_console
 * \param[in] x Индекс столбца.
 * \param[in] y Индекс строки.
 */
void sys_set_cursor_position(int x, int y);

/**
 * \brief Отключить буферизацию в консоли.
 * \ingroup platform_console
 */
void sys_disable_console_buffer(void);

#endif // INITPLUS_PLATFORM_CONSOLE_H_INCLUDED
