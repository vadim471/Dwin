#include <modbus/debug.h>
#include <modbus/handler.h>
#include <modbus/slave.h>
#include <modbus/range.h>
#include <modbus/memory.h>
#include <modbus/stack_trace.h>

static mbs_error_code_t mbs_build_on_request_handler(
    mbs_slave_tp slave,
    mbs_byte_t command,
    uint16_t address,
    uint16_t count,
    void* on_request
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_request_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!slave) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_build_on_request_handler> Slave is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    if (!on_request) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_build_on_request_handler> Handler is null");
        return MBS_ERRC_NULL_POINTER;
    }

    if (count < 1) {
        mbs_debug_print(MBS_ERRC_BAD_ARGUMENT, "mbs_build_on_request_handler> Count must be positive");
        return MBS_ERRC_BAD_ARGUMENT;
    }

    mbs_error_code_t errc = MBS_ERRC_NONE;
    mbs_range_tp range = mbs_get_or_create_range(&slave->ranges, command, &errc);
    if (!range) return errc;

    mbs_ranged_handler_tp handler = (mbs_ranged_handler_tp)mbs_malloc(sizeof(mbs_ranged_handler_t), "mbs_build_on_request_handler:handler");
    if (!handler) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_build_on_request_handler> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }

    mbs_init_ranged_handler(handler);
    handler->first = address;
    handler->last = address + count - 1;
    handler->handler = on_request;
    #ifdef MODBUS_EXTEND_CALLBACKS
    handler->eparam = on_request_eparam;
    #endif // MODBUS_EXTEND_CALLBACKS

    errc = mbs_push_ranged_handler(range, handler);
    if (errc) {
        mbs_free_ranged_handler(handler);
    }
    return errc;
}

mbs_error_code_t mbs_on_read_coils(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    mbs_slave_read_coils_fp on_request
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_request_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    return mbs_build_on_request_handler(
        slave,
        MBS_CMD_READ_COILS,
        address,
        count,
        (void*)on_request
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_request_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
}

mbs_error_code_t mbs_on_read_discrete_inputs(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    mbs_slave_read_discrete_inputs_fp on_request
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_request_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    return mbs_build_on_request_handler(
        slave,
        MBS_CMD_READ_DISCRETE_INPUTS,
        address,
        count,
        (void*)on_request
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_request_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
}

mbs_error_code_t mbs_on_read_holding_registers(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    mbs_slave_read_holding_registers_fp on_request
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_request_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    return mbs_build_on_request_handler(
        slave,
        MBS_CMD_READ_HOLDING_REGISTERS,
        address,
        count,
        (void*)on_request
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_request_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
}

mbs_error_code_t mbs_on_read_input_registers(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    mbs_slave_read_input_registers_fp on_request
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_request_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    return mbs_build_on_request_handler(
        slave,
        MBS_CMD_READ_INPUT_REGISTERS,
        address,
        count,
        (void*)on_request
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_request_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
}

mbs_error_code_t mbs_on_write_multiple_coils(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    mbs_slave_write_multiple_coils_fp on_request
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_request_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    return mbs_build_on_request_handler(
        slave,
        MBS_CMD_WRITE_MULTIPLE_COILS,
        address,
        count,
        (void*)on_request
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_request_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
}

mbs_error_code_t mbs_on_write_multiple_registers(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    mbs_slave_write_multiple_registers_fp on_request
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_request_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    return mbs_build_on_request_handler(
        slave,
        MBS_CMD_WRITE_MULTIPLE_REGISTERS,
        address,
        count,
        (void*)on_request
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_request_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
}
