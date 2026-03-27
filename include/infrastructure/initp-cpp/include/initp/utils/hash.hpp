#ifndef INITPLUS_UTILS_HASH_HPP_INCLUDED
#define INITPLUS_UTILS_HASH_HPP_INCLUDED

#include <cstddef>
#include <string>

namespace initp {
namespace utils {
namespace hash {

/**
 * \brief Вычислить MD5.
 * \param[in] data Указатель на массив с данными.
 * \param[in] bytes Количество байт данных.
 * \return Строка с результатом в шестнадцатиричной форме.
 */
std::string md5(const void* data, size_t bytes);

/**
 * \brief Вычислить MD5.
 * \param[in] data Строка с данными.
 * \return Строка с результатом в шестнадцатиричной форме.
 */
std::string md5(const std::string& data);

/**
 * \brief Закодировать в BASE64.
 * \param[in] data Строка с данными.
 * \return Строка с результатом.
 */
std::string encode_base64(const std::string& data);

/**
 * \brief Раскодировать BASE64.
 * \param[in] data Строка с данными.
 * \return Строка с результатом.
 */
std::string decode_base64(std::string& data);

}}}

#endif // INITPLUS_UTILS_HASH_HPP_INCLUDED
