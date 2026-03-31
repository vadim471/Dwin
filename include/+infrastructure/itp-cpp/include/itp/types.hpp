#ifndef ITP_TYPES_HPP_INCLUDED
#define ITP_TYPES_HPP_INCLUDED

#include "config.hpp"

#include <memory>

namespace itp {

class root;
class frame;
class endpoint;

typedef std::shared_ptr<root> root_ptr;
typedef std::shared_ptr<frame> frame_ptr;
typedef std::unique_ptr<frame> frame_uptr;
typedef std::unique_ptr<endpoint> endpoint_uptr;

}

#endif // ITP_TYPES_HPP_INCLUDED
