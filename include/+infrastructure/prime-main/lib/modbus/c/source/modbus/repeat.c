#include <modbus/debug.h>
#include <modbus/repeat.h>
#include <modbus/stack_trace.h>

void mbs_repeat_once_init(mbs_repeat_once_t* object) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!object) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_repeat_once_init> Repeat object is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    object->active = 1;
}

mbs_repeat_status_t mbs_repeat_once_check(
    void* object,
    mbs_time_t time,
    mbs_error_code_t* error
) {
    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!object) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_repeat_once_check> Repeat object is null");
        mbs_print_stack_trace(stdout);
        if (error) *error = MBS_ERRC_INTERNAL;
        return MBS_RS_ERROR;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_repeat_once_t* repeat = (mbs_repeat_once_t*)object;
    return repeat->active ? MBS_RS_READY : MBS_RS_IDLE;
}

mbs_error_code_t mbs_repeat_once_reset(
    void* object,
    mbs_time_t time
) {
    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!object) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_repeat_once_reset> Repeat object is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_repeat_once_t* repeat = (mbs_repeat_once_t*)object;
    repeat->active = 0;
    return MBS_ERRC_NONE;
}

void mbs_repeat_cyclic_init(mbs_repeat_cyclic_t* object, mbs_time_t timeout) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!object) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_repeat_cyclic_init> Repeat object is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    object->timeout = timeout;
    object->begin = 0;
}

mbs_repeat_status_t mbs_repeat_cyclic_check(
    void* object,
    mbs_time_t time,
    mbs_error_code_t* error
) {
    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!object) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_repeat_cyclic_check> Repeat object is null");
        mbs_print_stack_trace(stdout);
        if (error) *error = MBS_ERRC_INTERNAL;
        return MBS_RS_ERROR;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_repeat_cyclic_t* repeat = (mbs_repeat_cyclic_t*)object;
    if (repeat->begin + repeat->timeout > time) {
        return MBS_RS_PENDING;
    } else return MBS_RS_READY;
}

mbs_error_code_t mbs_repeat_cyclic_reset(
    void* object,
    mbs_time_t time
) {
    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!object) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_repeat_cyclic_reset> Repeat object is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_repeat_cyclic_t* repeat = (mbs_repeat_cyclic_t*)object;
    repeat->begin = time;
    return MBS_ERRC_NONE;
}

void mbs_repeat_manual_init(mbs_repeat_manual_t* object, uint8_t* trigger) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!object) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_repeat_manual_init> Repeat object is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    object->trigger = trigger;
}

mbs_repeat_status_t mbs_repeat_manual_check(
    void* object,
    mbs_time_t time,
    mbs_error_code_t* error
) {
    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!object) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_repeat_manual_check> Repeat object is null");
        mbs_print_stack_trace(stdout);
        if (error) *error = MBS_ERRC_INTERNAL;
        return MBS_RS_ERROR;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_repeat_manual_t* repeat = (mbs_repeat_manual_t*)object;
    return (*repeat->trigger) ? MBS_RS_READY : MBS_RS_PENDING;
}

mbs_error_code_t mbs_repeat_manual_reset(
    void* object,
    mbs_time_t time
) {
    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!object) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_repeat_manual_reset> Repeat object is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_repeat_manual_t* repeat = (mbs_repeat_manual_t*)object;
    *repeat->trigger = 0;
    return MBS_ERRC_NONE;
}
