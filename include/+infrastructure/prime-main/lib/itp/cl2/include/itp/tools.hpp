#include <cstdint>
#include <string>

/**
 * \file tools.hpp
 * \brief Файл содержит служебные функции и инструменты.
 */

namespace itp {
namespace tools {

/**
 * \brief Возвращает список функций устройства по его типу интерфейса.
 * \ingroup tools_list
 * \param[in] api Тип интерфейса устройства.
 * \return Функции устройства в кодировке UTF-8.
 */
std::string get_device_api(uint32_t api);

/**
 * \brief Возвращает имя устройства по его типу.
 * \ingroup tools_list
 * \param[in] type Тип устройства.
 * \return Имя устройства в кодировке UTF-8.
 */
std::string get_device_name(uint8_t type);

/**
 * \brief Возвращает строковое состояние устройства.
 * \ingroup tools_list
 * \param[in] status Состояние устройства.
 * \return Состояние устройства в кодировке UTF-8.
 */
std::string get_device_status(uint32_t status);

/**
 * \brief Преобразовывает время в строку.
 * \ingroup tools_list
 * \param[in] prefix Префикс.
 * \param[in] time Время.
 * \return Строка.
 */
std::string time_to_string(const std::string& prefix, uint64_t time);

/**
 * \brief Приводит уровень системного логирования к локальному.
 * \ingroup tools_list
 * \param[in] level Уровень логирования.
 * \return Уровень логирования.
 */
uint8_t cast_system_debug_level(uint8_t level);

}}
