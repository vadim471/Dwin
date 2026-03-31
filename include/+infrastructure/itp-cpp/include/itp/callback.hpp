#ifndef ITP_CALLBACK_HPP_INCLUDED
#define ITP_CALLBACK_HPP_INCLUDED

#include "config.hpp"

extern "C" {

#include <itp/error_code.h>

}

#include <tuple>
#include <memory>
#include <functional>
#include <string>

namespace itp {

class root;
class frame;
class endpoint;
class remote_endpoint;

typedef std::function<
    void(root&, uint8_t, itp_error_code_t)
> on_error_f;

typedef std::function<
    void(root&)
> on_signal_f;

typedef std::function<
    void(root&, uint8_t, uint16_t)
> on_connect_f;

typedef std::function<
    void(root&, uint8_t, uint16_t, std::unique_ptr<remote_endpoint>)
> on_connect_endpoint_f;

typedef std::function<
    std::tuple<std::unique_ptr<endpoint>, uint16_t>(const std::string&)
> on_create_f;

typedef std::function<
    void(root&, uint16_t)
> on_result_f;

typedef std::function<
    uint16_t(root&, frame&)
> on_request_f;

typedef std::function<
    void(root&, uint16_t, frame&)
> on_response_f;

typedef std::function<
    void(root&, uint16_t, uint16_t, uint32_t)
> on_update_f;

}

#endif // ITP_CALLBACK_HPP_INCLUDED
