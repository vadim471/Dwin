#ifndef INITPLUS_TOOLS_IP_HPP_INCLUDED
#define INITPLUS_TOOLS_IP_HPP_INCLUDED

#include <cstdint>
#include <string>
#include <tuple>

namespace initp {
namespace tools {
namespace ip {

/**
 * \brief Разделить строку адреса на хост и порт.
 * \param[in] address Полный адрес.
 * \param[in] default_port Порт по умолчанию.
 * \return Кортеж из хоста и порта.
 */
std::tuple<std::string, std::string> split_address(const std::string& address, const std::string& default_port);

/**
 * \brief Разделить строку адреса на хост и порт.
 * \param[in] address Полный адрес.
 * \param[in] default_port Порт по умолчанию.
 * \return Кортеж из хоста и порта.
 */
std::tuple<std::string, uint16_t> split_address(const std::string& address, uint16_t default_port);

}}}

#endif // INITPLUS_TOOLS_IP_HPP_INCLUDED
