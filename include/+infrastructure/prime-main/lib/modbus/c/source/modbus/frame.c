#include <modbus/debug.h>
#include <modbus/frame.h>
#include <modbus/memory.h>
#include <modbus/stack_trace.h>

#include <string.h>

void mbs_init_frame(mbs_frame_tp frame) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!frame) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_init_frame> Frame is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    frame->slave = 0;
    frame->command = 0;
    frame->error = 0;
    frame->data = NULL;
    frame->size = 0;
}

mbs_frame_tp mbs_create_frame(void) {
    mbs_frame_tp object = (mbs_frame_tp)mbs_malloc(sizeof(mbs_frame_t), "mbs_create_frame:mbs_frame");
    if (!object) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_create_frame> Memory allocation failed");
        mbs_print_stack_trace(stdout);
        return NULL;
    }
    mbs_init_frame(object);
    return object;
}

void mbs_free_frame(mbs_frame_tp frame) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!frame) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_free_frame> Frame is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_dispose_frame(frame);
    mbs_free(frame);
}

mbs_error_code_t mbs_prepare_frame(mbs_frame_tp frame, mbs_size_t size) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!frame) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_prepare_frame> Frame is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    frame->data = mbs_malloc(size * sizeof(char), "mbs_prepare_frame:data");
    if (!frame->data) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_prepare_frame> Memory allocation failed");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_MALLOC_FAILED;
    }
    memset(frame->data, 0x0, size);
    frame->size = size;
    return MBS_ERRC_NONE;
}

void mbs_dispose_frame(mbs_frame_tp frame) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!frame) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_dispose_frame> Frame is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (frame->data) {
        mbs_free(frame->data);
        frame->data = NULL;
    }
}
