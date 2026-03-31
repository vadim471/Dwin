#ifndef INITPLUS_PLATFORM_NET_HPP_INCLUDED
#define INITPLUS_PLATFORM_NET_HPP_INCLUDED

#include <string>

namespace initp {
namespace system {
namespace tools {

std::string get_host_name(void);

std::string get_user_name(void);

std::string get_ip_address(const std::string&);

}}}

#endif // INITPLUS_PLATFORM_NET_HPP_INCLUDED

