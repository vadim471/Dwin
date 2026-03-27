#ifndef INITPLUS_PLATFORM_TOOLS_HPP_INCLUDED
#define INITPLUS_PLATFORM_TOOLS_HPP_INCLUDED

#include <string>
#include <cstdint>

namespace initp {
namespace system {
namespace tools {

std::string get_host_name(void);

std::string get_user_name(void);

std::string get_ip_address(const std::string&);

std::string get_cpu_id(void);

uint32_t get_pid(const std::string&);

}}}

#endif // INITPLUS_PLATFORM_TOOLS_HPP_INCLUDED
