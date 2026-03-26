#include <itp/debug.h>
#include <itp/package.h>
#include <itp/protocol.h>
#include <itp/memory.h>
#include <itp/frame.h>
#include <itp/stack_trace.h>

#include <string.h>

void itp_init_package(itp_package_tp package) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!package) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_package> Package is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    memset(package, 0x0, sizeof(itp_package_t));
    /*package->address = 0;
    package->use_checksum = 0;
    package->status = 0;
    package->data = (itp_byte_t*)0;
    package->size = 0;
    package->next = (itp_package_tp)0;*/
}

itp_error_code_t itp_prepare_package(itp_package_tp package, itp_size_t size) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!package) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_package> Package is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    package->data = itp_malloc(size * sizeof(itp_byte_t), "itp_prepare_package:data");
    if (!package->data) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_init_package> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    package->size = size;
    return ITP_ERRC_NONE;
}

void itp_dispose_package(itp_package_tp package) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!package) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_dispose_package> Package is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (package->data) {
        itp_free(package->data);
        package->data = NULL;
        package->size = 0;
    }
}

void itp_free_package(itp_package_tp package) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!package) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_free_package> Package is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (package->data)
        itp_free(package->data);
    itp_free(package);
}

itp_package_tp itp_serialize_result(uint8_t from, uint8_t to, uint16_t command, uint16_t order, uint8_t status, uint16_t error, itp_error_code_t* result) {
    itp_frame_tp frame = itp_create_frame(command);
    if (!frame) {
        if (result) *result = ITP_ERRC_MALLOC_FAILED;
        return NULL;
    }
    frame->command = command;
    frame->order = order;
    frame->error = error;
    frame->status = status;
    frame->from = from;
    frame->to = to;
    itp_package_tp package = itp_serialize_frame(frame, result);
    itp_free_frame(frame);
    return package;
}
