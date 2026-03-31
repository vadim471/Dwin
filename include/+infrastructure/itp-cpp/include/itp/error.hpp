#ifndef ITP_ERROR_HPP_INCLUDED
#define ITP_ERROR_HPP_INCLUDED

#include "config.hpp"

extern "C" {

#include <itp/error.h>

}

#include <string>

namespace itp {

static std::string get_error_description(uint16_t) __attribute__((unused));
static std::string get_error_description(uint16_t error_code) {
    return itp_get_error_description(error_code);
}

static uint16_t cast_error_code(itp_error_code_t) __attribute__((unused));
static uint16_t cast_error_code(itp_error_code_t error_code) {
    return itp_cast_error_code(error_code);
}

}

#endif // ITP_ERROR_HPP_INCLUDED
