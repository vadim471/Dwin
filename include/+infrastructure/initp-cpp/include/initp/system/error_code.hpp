#ifndef INITPLUS_SYSTEM_ERROR_CODE_HPP_INCLUDED
#define INITPLUS_SYSTEM_ERROR_CODE_HPP_INCLUDED

#include <boost/system/error_code.hpp>
#include <iostream>
#include <sstream>
#include <string>

namespace initp {
namespace system {

namespace errc {

enum errc_t {
    success = 0,
    generic_error,
    internal_error,
    server_error,
    cancelled,
    timeout,
    memory_failure,
    null_pointer,
    invalid_state,
    parsing_failed,
    checksum_not_match,
    serialization_failed,
    authorization_failed,
    connection_failed,
    not_implemented,
    bad_configuration,
    serial_port_error,
    file_not_found,
    overflow,
    bad_argument,
    bad_data
};

}

class error_category:
    public boost::system::error_category {
    virtual const char* name() const noexcept { return "initp:system"; }
    virtual std::string message(int ev) const {
        std::stringstream stream;
        stream << this->name() << ":" << ev;
        return stream.str();
    }
};

typedef boost::system::error_code error_code;

error_code make_error(errc::errc_t e);

struct err {
    static const error_category category;
    static const error_code success;
    static const error_code cancelled;
    static const error_code generic_error;
    static const error_code internal_error;
    static const error_code server_error;
    static const error_code timeout;
    static const error_code memory_failure;
    static const error_code null_pointer;
    static const error_code invalid_state;
    static const error_code parsing_failed;
    static const error_code checksum_not_match;
    static const error_code serialization_failed;
    static const error_code authorization_failed;
    static const error_code connection_failed;
    static const error_code not_implemented;
    static const error_code bad_configuration;
    static const error_code serial_port_error;
    static const error_code file_not_found;
    static const error_code overflow;
    static const error_code bad_argument;
    static const error_code bad_data;
};

}}

#endif // INITPLUS_SYSTEM_ERROR_CODE_HPP_INCLUDED
