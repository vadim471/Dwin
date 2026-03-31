#include <modbus/debug.h>
#include <modbus/endpoint.h>
#include <modbus/stack_trace.h>

mbs_error_code_t mbs_init_endpoint(mbs_endpoint_tp endpoint, mbs_ep_read_fp read, mbs_ep_write_fp write, void* object) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!endpoint) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_init_endpoint> Endpoint is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    if (!read) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_init_endpoint> Read function is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    if (!write) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_init_endpoint> Write function is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    endpoint->read = read;
    endpoint->write = write;
    endpoint->object = object;
    #if (MODBUS_WAIT_SWITCHING > 0)
    endpoint->time = 0;
    #endif // (MODBUS_WAIT_SWITCHING > 0)
    return MBS_ERRC_NONE;
}

mbs_size_t mbs_read_data(
    mbs_endpoint_tp endpoint,
    #if (MODBUS_WAIT_SWITCHING > 0)
    mbs_time_t time,
    #endif // (MODBUS_WAIT_SWITCHING > 0)
    mbs_byte_t* data,
    mbs_size_t length
) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!endpoint) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_read_data> Endpoint is null");
        mbs_print_stack_trace(stdout);
        return 0;
    }
    if (!data) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_read_data> Data is null");
        mbs_print_stack_trace(stdout);
        return 0;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (!endpoint->read) {
        mbs_debug_print(MBS_ERRC_EP_NOT_CONFIGURED, "mbs_read_data> Endpoint not configured properly");
        mbs_print_stack_trace(stdout);
        return 0;
    }
    #if (MODBUS_WAIT_SWITCHING > 0)
    endpoint->time = time;
    #endif // (MODBUS_WAIT_SWITCHING > 0)
    return endpoint->read(endpoint->object, data, length);
}

mbs_size_t mbs_write_data(
    mbs_endpoint_tp endpoint,
    #if (MODBUS_WAIT_SWITCHING > 0)
    mbs_time_t time,
    #endif // (MODBUS_WAIT_SWITCHING > 0)
    mbs_byte_t* data,
    mbs_size_t length
) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!endpoint) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_write_data> Endpoint is null");
        mbs_print_stack_trace(stdout);
        return 0;
    }
    if (!data) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_write_data> Data is null");
        mbs_print_stack_trace(stdout);
        return 0;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (!endpoint->write) {
        mbs_debug_print(MBS_ERRC_EP_NOT_CONFIGURED, "mbs_write_data> Endpoint not configured properly");
        mbs_print_stack_trace(stdout);
        return 0;
    }
    #if (MODBUS_WAIT_SWITCHING > 0)
    if (endpoint->time + MODBUS_WAIT_SWITCHING > time) return 0;
    #endif // (MODBUS_WAIT_SWITCHING > 0)
    return endpoint->write(endpoint->object, data, length);
}
