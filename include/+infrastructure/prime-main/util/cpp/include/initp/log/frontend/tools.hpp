#ifndef INITPLUS_LOG_FRONTEND_TOOLS_HPP_INCLUDED
#define INITPLUS_LOG_FRONTEND_TOOLS_HPP_INCLUDED

#include <initp/log/level.hpp>
#include <initp/log/frontend/config.pb.h>

namespace initp {
namespace log {

level::value to_native_level(log_level level);

}}

#endif // INITPLUS_LOG_FRONTEND_TOOLS_HPP_INCLUDED
