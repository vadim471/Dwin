#include <itp/debug.h>
#include <itp/endpoint.h>
#include <itp/stack_trace.h>

itp_error_code_t itp_init_endpoint(itp_endpoint_tp endpoint, itp_ep_read_fp read, itp_ep_write_fp write, void* object) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!endpoint) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_init_endpoint> Endpoint is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    endpoint->read = read;
    endpoint->write = write;
    endpoint->use_checksum = 0;
    endpoint->object = object;
    return ITP_ERRC_NONE;
}

itp_size_t itp_read_data(itp_endpoint_tp endpoint, itp_byte_t* data, itp_size_t size) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!endpoint) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_read_data> Endpoint is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    if (!data) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_read_data> Data is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (!endpoint->read) {
        itp_debug_print(ITP_ERRC_EP_NOT_CONFIGURED, "itp_read_data> Endpoint not configured properly");
        itp_print_stack_trace(stdout);
        return 0;
    }
    return endpoint->read(endpoint->object, data, size);
}

itp_size_t itp_write_data(itp_endpoint_tp endpoint, const itp_byte_t* data, itp_size_t length) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!endpoint) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_write_data> Endpoint is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    if (!data) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_write_data> Data is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (!endpoint->write) {
        itp_debug_print(ITP_ERRC_EP_NOT_CONFIGURED, "itp_write_data> Endpoint not configured properly");
        itp_print_stack_trace(stdout);
        return 0;
    }
    return endpoint->write(endpoint->object, data, length);
}
