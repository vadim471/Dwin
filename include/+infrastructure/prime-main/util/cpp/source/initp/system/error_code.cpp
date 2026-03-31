#include <initp/system/error_code.hpp>

namespace initp {
namespace system {

error_code make_error(errc::errc_t e) {
    return error_code(e, err::category);
}

const error_category err::category;
const error_code err::success(make_error(errc::success));
const error_code err::cancelled(make_error(errc::cancelled));
const error_code err::generic_error(make_error(errc::generic_error));
const error_code err::internal_error(make_error(errc::internal_error));
const error_code err::server_error(make_error(errc::server_error));
const error_code err::timeout(make_error(errc::timeout));
const error_code err::memory_failure(make_error(errc::memory_failure));
const error_code err::null_pointer(make_error(errc::null_pointer));
const error_code err::invalid_state(make_error(errc::invalid_state));
const error_code err::parsing_failed(make_error(errc::parsing_failed));
const error_code err::checksum_not_match(make_error(errc::checksum_not_match));
const error_code err::serialization_failed(make_error(errc::serialization_failed));
const error_code err::authorization_failed(make_error(errc::authorization_failed));
const error_code err::connection_failed(make_error(errc::connection_failed));
const error_code err::not_implemented(make_error(errc::not_implemented));
const error_code err::bad_configuration(make_error(errc::bad_configuration));
const error_code err::serial_port_error(make_error(errc::serial_port_error));
const error_code err::file_not_found(make_error(errc::file_not_found));
const error_code err::overflow(make_error(errc::overflow));
const error_code err::bad_argument(make_error(errc::bad_argument));
const error_code err::bad_data(make_error(errc::bad_data));

}}
