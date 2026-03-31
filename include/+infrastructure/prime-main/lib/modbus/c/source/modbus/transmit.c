#include <modbus/debug.h>
#include <modbus/transmit.h>
#include <modbus/endpoint.h>
#include <modbus/stack_trace.h>

#include <string.h>

void mbs_init_buffer(mbs_buffer_tp buffer, mbs_get_pdu_length_fp get_pdu_length, mbs_check_frame_fp check_frame) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!buffer) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_init_buffer> Buffer is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    buffer->get_pdu_length = get_pdu_length;
    buffer->check_frame = check_frame;
    buffer->length = 0;
    buffer->complete = 0;
    buffer->time = 0;
}

void mbs_clear_buffer(mbs_buffer_tp buffer) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!buffer) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_clear_buffer> Buffer is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    buffer->complete = 0;
    buffer->length = 0;
}

void mbs_flush_buffer(mbs_buffer_tp buffer) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!buffer) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_flush_buffer> Buffer is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    buffer->complete = 0;
}

mbs_transmit_status_t mbs_receive(mbs_buffer_tp buffer, mbs_endpoint_tp endpoint, mbs_time_t time, mbs_error_code_t* error) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_receive> Buffer is null");
        mbs_print_stack_trace(stdout);
        if (error) *error = MBS_ERRC_NULL_POINTER;
        return MBS_TRANSMIT_ERROR;
    }
    if (!endpoint) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_receive> Endpoint is null");
        mbs_print_stack_trace(stdout);
        if (error) *error = MBS_ERRC_NULL_POINTER;
        return MBS_TRANSMIT_ERROR;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    buffer->complete += mbs_read_data(
        endpoint,
        #if (MODBUS_WAIT_SWITCHING > 0)
        time,
        #endif // (MODBUS_WAIT_SWITCHING > 0)
        buffer->data + buffer->complete, MODBUS_BUFFER_SIZE - buffer->complete
    );
    buffer->length = buffer->complete;
    #ifdef MODBUS_ENABLE_BOUNDARY_CHECKS
    if (buffer->complete > MODBUS_BUFFER_SIZE) {
        mbs_debug_print(MBS_ERRC_OUT_OF_BOUNDS, "mbs_receive> Out of bounds on data read");
        mbs_print_stack_trace(stdout);
        return MBS_TRANSMIT_ERROR;
    }
    #endif // MODBUS_ENABLE_BOUNDARY_CHECKS
    if (buffer->complete == 0) {
        return MBS_TRANSMIT_IDLE;
    }

    mbs_parse_status_t status = buffer->check_frame(buffer, error);
    if (status == MBS_PARSE_NOT_END) {
        return MBS_TRANSMIT_PENDING;
    } else if (status == MBS_PARSE_OK) {
        return MBS_TRANSMIT_OK;
    } else return MBS_TRANSMIT_ERROR;
}

mbs_transmit_status_t mbs_transmit(mbs_buffer_tp buffer, mbs_endpoint_tp endpoint, mbs_time_t time, mbs_error_code_t* error) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_transmit> Buffer is null");
        mbs_print_stack_trace(stdout);
        if (error) *error = MBS_ERRC_NULL_POINTER;
        return MBS_TRANSMIT_ERROR;
    }
    if (!endpoint) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_transmit> Endpoint is null");
        mbs_print_stack_trace(stdout);
        if (error) *error = MBS_ERRC_NULL_POINTER;
        return MBS_TRANSMIT_ERROR;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (buffer->complete >= buffer->length) {
        #ifdef MODBUS_ENABLE_BOUNDARY_CHECKS
        if (buffer->complete > buffer->length) {
            mbs_debug_print(MBS_ERRC_WARNING, "mbs_transmit> Transmitted more then buffer length");
            mbs_print_stack_trace(stdout);
        }
        #endif // MODBUS_ENABLE_BOUNDARY_CHECKS
        mbs_write_data(
            endpoint,
            #if (MODBUS_WAIT_SWITCHING > 0)
            time,
            #endif // (MODBUS_WAIT_SWITCHING > 0)
            buffer->data,
            0
        );
        return MBS_TRANSMIT_IDLE;
    } else {
        buffer->complete += mbs_write_data(
            endpoint,
            #if (MODBUS_WAIT_SWITCHING > 0)
            time,
            #endif // (MODBUS_WAIT_SWITCHING > 0)
            buffer->data + buffer->complete, buffer->length - buffer->complete
        );
        if (buffer->complete >= buffer->length) {
            #ifdef MODBUS_ENABLE_BOUNDARY_CHECKS
            if (buffer->complete > buffer->length) {
                mbs_debug_print(MBS_ERRC_WARNING, "mbs_transmit> Transmitted more then buffer length");
                mbs_print_stack_trace(stdout);
            }
            #endif // MODBUS_ENABLE_BOUNDARY_CHECKS
            return MBS_TRANSMIT_OK;
        } else return MBS_TRANSMIT_PENDING;
    }
}

uint8_t mbs_transmit_is_ready(mbs_buffer_tp buffer) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_transmit_is_ready> Buffer is null");
        mbs_print_stack_trace(stdout);
        return 0;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (buffer->complete < buffer->length) {
        return 1;
    } else return 0;
}

mbs_size_t mbs_receive_bytes(mbs_buffer_tp buffer, mbs_byte_t* data, mbs_size_t size) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_receive_bytes> Buffer is null");
        mbs_print_stack_trace(stdout);
        return 0;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (buffer->length < size)
        size = buffer->length;
    if (data && size > 0) {
        memcpy(data, buffer->data, size * sizeof(mbs_byte_t));
        #ifdef MODBUS_TRACE_BUFFERS
        {
            char string[MODBUS_BUFFER_SIZE * 3];
            char* it = string;
            *it = 0;
            for (mbs_size_t i = 0; i < size; i++) {
                sprintf(it, " %02x", data[i] & 0xFF);
                it += 3;
            }
            mbs_debug_print(MBS_ERRC_TRACE, "Read:%s (%d)", string, size);
        }
        #endif // MODBUS_TRACE_BUFFERS
    }
    if (buffer->length > size) {
        mbs_size_t remain = buffer->length - size;
        memcpy(buffer->data, buffer->data + size, remain);
        buffer->length = remain;
        buffer->complete = remain;
    } else {
        buffer->length = 0;
        buffer->complete = 0;
    }
    return size;
}
