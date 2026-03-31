#include <modbus/debug.h>
#include <modbus/rtu.h>
#include <modbus/symbol.h>
#include <modbus/transmit.h>
#include <modbus/frame.h>
#include <modbus/tools.h>
#include <modbus/memory.h>
#include <modbus/stack_trace.h>

#include <initp/tools/crc.h>

#include <string.h>

uint16_t mbs_eval_crc16(const mbs_byte_t* data, mbs_size_t length) {
    return sys_eval_crc16(0xFFFF, (const char*)data, (size_t)length);
}

mbs_parse_status_t mbs_check_rtu_frame(mbs_buffer_tp buffer, mbs_error_code_t* error) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_check_rtu_frame> Buffer is null");
        mbs_print_stack_trace(stdout);
        if (error) *error = MBS_ERRC_INTERNAL;
        return MBS_PARSE_ERROR;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (buffer->length < 4) return MBS_PARSE_NOT_END;

    mbs_size_t length = buffer->get_pdu_length(buffer) + 3;
    if (!length) {
        if (error) *error = MBS_ERRC_UNKNOWN_COMMAND;
        return MBS_PARSE_ERROR;
    } else if (buffer->length < length) {
        return MBS_PARSE_NOT_END;
    } else return MBS_PARSE_OK;
}

mbs_frame_tp mbs_extract_rtu_frame(mbs_buffer_tp buffer, mbs_error_code_t* error) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_extract_rtu_frame> Buffer is null");
        mbs_print_stack_trace(stdout);
        if (error) *error = MBS_ERRC_INTERNAL;
        return NULL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (buffer->length < 4) {
        if (error) *error = MBS_ERRC_INTERNAL;
        return NULL;
    }

    mbs_size_t length = buffer->get_pdu_length(buffer) + 3;
    if (!length) {
        if (error) *error = MBS_ERRC_UNKNOWN_COMMAND;
        return NULL;
    } else if (buffer->length < length) {
        if (error) *error = MBS_ERRC_INTERNAL;
        return NULL;
    }

    uint16_t crc = *(uint16_t*)(buffer->data + length - 2);
    if (crc != mbs_eval_crc16(buffer->data, length - 2)) {
        mbs_debug_print(MBS_ERRC_CRC_FAILED, "mbs_extract_rtu_frame> Checksum failed");
        if (error) *error = MBS_ERRC_CRC_FAILED;
        return NULL;
    }

    mbs_frame_tp frame = (mbs_frame_tp)mbs_malloc(sizeof(mbs_frame_t), "mbs_extract_rtu_frame:mbs_frame");
    if (!frame) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_extract_rtu_frame> Memory allocation failed");
        if (error) *error = MBS_ERRC_MALLOC_FAILED;
        return NULL;
    }
    mbs_init_frame(frame);

    frame->slave = buffer->data[0];
    frame->command = buffer->data[1] & 0x7F;

    if (buffer->data[1] & 0x80) {
        frame->error = buffer->data[2];
    } else if (length > 4) {
        mbs_error_code_t errc = mbs_prepare_frame(frame, length - 4);
        if (errc) {
            mbs_free_frame(frame);
            if (error) *error = errc;
            return NULL;
        }
        memcpy(frame->data, buffer->data + 2, length - 4);
    }
    return frame;
}

mbs_error_code_t mbs_serialize_rtu_frame(mbs_buffer_tp buffer, mbs_frame_tp frame) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_serialize_rtu_frame> Buffer is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_clear_buffer(buffer);
    buffer->data[buffer->length++] = frame->slave;
    buffer->data[buffer->length++] = frame->command;
    if (frame->error) {
        buffer->data[1] |= 0x80;
        buffer->data[buffer->length++] = frame->error;
    } else if (frame->size > 0) {
        memcpy(buffer->data + 2, frame->data, frame->size);
        buffer->length += frame->size;
    }
    uint16_t crc = mbs_eval_crc16(buffer->data, buffer->length);
    *(uint16_t*)(buffer->data + buffer->length) = crc;
    buffer->length += 2;
    return MBS_ERRC_NONE;
}
