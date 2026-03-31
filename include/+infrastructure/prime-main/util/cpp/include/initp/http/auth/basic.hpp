#ifndef INITPLUS_HTTP_AUTH_BASIC_HPP_INCLUDED
#define INITPLUS_HTTP_AUTH_BASIC_HPP_INCLUDED

#include <string>

namespace initp {
namespace http {
namespace auth {
namespace basic {

std::string generate(
    const std::string& user,
    const std::string& password
);

}}}}

#endif // INITPLUS_HTTP_AUTH_BASIC_HPP_INCLUDED
