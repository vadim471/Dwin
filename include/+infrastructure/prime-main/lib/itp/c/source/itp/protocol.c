#include <itp/debug.h>
#include <itp/protocol.h>
#include <itp/transmit.h>
#include <itp/queue.h>
#include <itp/frame.h>
#include <itp/memory.h>
#include <itp/package.h>
#include <itp/error.h>
#include <itp/stack_trace.h>

#include <initp/tools/crc.h>

#include <string.h>

static uint16_t itp_eval_crc16(const itp_byte_t* data, itp_size_t length) {
    return sys_eval_crc16(0, (const char*)data, (size_t)length);
}

static uint16_t itp_read_uint16(itp_byte_t* data) {
    uint16_t value = ((uint16_t)data[0] << 8) | (uint16_t)data[1];
    //uint8_t* hi = (uint8_t*)&value + 1;
    //uint8_t* lo = (uint8_t*)&value;
    //*hi = data[0];
    //*lo = data[1];
    return value;
}

static void itp_write_uint16(itp_byte_t* data, uint16_t value) {
    //uint8_t* hi = (uint8_t*)&value + 1;
    //uint8_t* lo = (uint8_t*)&value;
    //data[0] = *hi;
    //data[1] = *lo;
    data[0] = (itp_byte_t)(value >> 8);
    data[1] = (itp_byte_t)(value);
}

static const uint8_t ITP_OFFSET_VERSION = 0;
static const uint8_t ITP_OFFSET_HEADER_SIZE = 1;
static const uint8_t ITP_OFFSET_ADDRESS_FROM = 2;
static const uint8_t ITP_OFFSET_ADDRESS_TO = 3;
static const uint8_t ITP_OFFSET_ORDER = 4;
static const uint8_t ITP_OFFSET_STATUS = 6;
static const uint8_t ITP_OFFSET_COMMAND = 7;
static const uint8_t ITP_OFFSET_DATA_SIZE = 9;
static const itp_size_t ITP_HEADER_SIZE = 11;

itp_parse_status_t itp_check_package(itp_buffer_tp buffer, itp_size_t* expected_size) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_check_package> Buffer is null");
        itp_print_stack_trace(stdout);
        return ITP_PARSE_ERROR;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Min package size
    if (buffer->length < ITP_HEADER_SIZE) {
        if (expected_size) *expected_size = 0;
        return ITP_PARSE_NOT_END;
    }

    // Check frame size
    uint8_t version = buffer->data[ITP_OFFSET_VERSION] & 0x3F;
    if (version != ITP_REVISION) {
        itp_debug_print(ITP_ERRC_PARSE_FAILED, "itp_check_package> Wrong protocol version (or bad data)");
        #ifdef ITP_TRACE_OFFSET_ERRORS
        {
            char data_buffer[0x50];
            char* string = data_buffer;
            for (itp_size_t i = 0; i < ITP_HEADER_SIZE; ++i) {
                sprintf(string, " %02x", (int)buffer->data[i]);
                string += 3;
            }
            itp_debug_print(ITP_ERRC_TRACE, "itp_check_package> Head:%s", data_buffer);
        }
        #endif // ITP_TRACE_OFFSET_ERRORS
        return ITP_PARSE_ERROR;
    }
    itp_size_t header_size = buffer->data[ITP_OFFSET_HEADER_SIZE];
    if (header_size != ITP_HEADER_SIZE) {
        itp_debug_print(ITP_ERRC_PARSE_FAILED, "itp_check_package> Wrong header size (or bad data)");
        return ITP_PARSE_ERROR;
    }
    itp_size_t data_size = itp_read_uint16(buffer->data + ITP_OFFSET_DATA_SIZE);
    itp_size_t size = buffer->data[ITP_OFFSET_HEADER_SIZE] + data_size;
    if (!(buffer->data[ITP_OFFSET_VERSION] & 0x80))
        size += 2;
    if (expected_size) *expected_size = size;
    if (buffer->length < size) {
        return ITP_PARSE_NOT_END;
    } else return ITP_PARSE_OK;
}

static void itp_move_data(itp_buffer_tp buffer, itp_size_t offset) {
    if (offset < 1) return;
    if (buffer->length <= offset) {
        buffer->length = 0;
        buffer->complete = 0;
        return;
    }
    itp_size_t remain = buffer->length - offset;
    for (itp_size_t i = 0; i < remain; ++i)
        buffer->data[i] = buffer->data[offset + i];
    if (buffer->complete > 0)
        buffer->complete = remain;
    buffer->length = remain;
}

itp_parse_status_t itp_repair_offset(itp_buffer_tp buffer) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_repair_offset> Buffer is null");
        itp_print_stack_trace(stdout);
        return ITP_PARSE_ERROR;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    for (itp_size_t i = 0; i < buffer->length; ++i) {

        // Check version
        if (ITP_OFFSET_VERSION + i >= buffer->length) break;
        uint8_t version = buffer->data[ITP_OFFSET_VERSION + i] & 0x3F;
        if (version != ITP_REVISION) continue;

        // Check header size
        if (ITP_OFFSET_HEADER_SIZE + i >= buffer->length) {
            itp_move_data(buffer, i);
            return ITP_PARSE_NOT_END;
        }
        itp_size_t header_size = buffer->data[ITP_OFFSET_HEADER_SIZE + i];
        if (header_size != ITP_HEADER_SIZE) continue;

        // Min package size
        itp_move_data(buffer, i);
        if (buffer->length < ITP_HEADER_SIZE) {
            return ITP_PARSE_NOT_END;
        }

        // Check data size
        itp_size_t data_size = itp_read_uint16(buffer->data + ITP_OFFSET_DATA_SIZE);
        itp_size_t size = buffer->data[ITP_OFFSET_HEADER_SIZE] + data_size;
        if (!(buffer->data[ITP_OFFSET_VERSION] & 0x80))
            size += 2;
        if (buffer->length < size) {
            return ITP_PARSE_NOT_END;
        } else return ITP_PARSE_OK;
    }
    return ITP_PARSE_ERROR;
}

itp_package_tp itp_extract_package(
#ifdef ITP_TRACE_BUFFERS
uint8_t root_address,
#endif // ITP_TRACE_BUFFERS
itp_buffer_tp buffer, itp_error_code_t* error) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_extract_package> Buffer is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Min package size
    if (buffer->length < ITP_HEADER_SIZE) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_extract_package> Package not complete");
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_PARSE_FAILED;
        return NULL;
    }

    // Check frame size
    itp_size_t data_size = itp_read_uint16(buffer->data + ITP_OFFSET_DATA_SIZE);
    itp_size_t size = buffer->data[ITP_OFFSET_HEADER_SIZE] + data_size;
    if (!(buffer->data[ITP_OFFSET_VERSION] & 0x80))
        size += 2;
    if (buffer->length >= size) {
        itp_package_tp package = itp_malloc(sizeof(itp_package_t), "itp_extract_package:itp_package");
        if (!package) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_extract_package> Memory allocation failed");
            itp_print_stack_trace(stdout);
            if (error) *error = ITP_ERRC_MALLOC_FAILED;
            return NULL;
        }
        itp_init_package(package);
        itp_error_code_t result = itp_prepare_package(package, size);
        if (result) {
            if (error) *error = result;
            return NULL;
        }
        if (itp_receive_bytes(
                #ifdef ITP_TRACE_BUFFERS
                root_address,
                #endif // ITP_TRACE_BUFFERS
                buffer, package->data, size
            ) != size) {
            itp_free_package(package);
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_extract_package> Failed to receive %d bytes", size);
            itp_print_stack_trace(stdout);
            if (error) *error = ITP_ERRC_INTERNAL;
            return NULL;
        }
        package->address = package->data[ITP_OFFSET_ADDRESS_TO];
        if (!(package->data[ITP_OFFSET_VERSION] & 0x80))
            package->use_checksum = 1;
        package->status = package->data[ITP_OFFSET_STATUS] & 0x07;
        return package;
    } else {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_extract_package> Package not complete");
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_PARSE_FAILED;
        return NULL;
    }
}

itp_error_code_t itp_write_package(itp_buffer_tp buffer, itp_package_tp package) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_write_package> Buffer is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!package) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_write_package> Package is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_clear_buffer(buffer);

    if (buffer->size < package->size) {
        itp_error_code_t errc = itp_resize_buffer(buffer, package->size, 0);
        if (errc) return errc;
    }

    memcpy(buffer->data, package->data, package->size);
    buffer->length = package->size;
    return ITP_ERRC_NONE;
}

itp_frame_tp itp_parse_package(itp_package_tp package, itp_error_code_t* error) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!package) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_parse_package> Package is null");
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_INTERNAL;
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Min package size
    if (package->size < ITP_HEADER_SIZE) {
        itp_debug_print(ITP_ERRC_PARSE_FAILED, "itp_parse_package> Package length < %d, but early check failed", ITP_HEADER_SIZE);
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_PARSE_FAILED;
        return NULL;
    }

    // Check frame size
    uint8_t skip_crc = package->data[ITP_OFFSET_VERSION] & 0x80;
    itp_size_t data_size = itp_read_uint16(package->data + ITP_OFFSET_DATA_SIZE);
    itp_size_t header_size = package->data[ITP_OFFSET_HEADER_SIZE];
    itp_size_t size = header_size + data_size;
    if (!skip_crc) size += 2;
    if (package->size < size) {
        itp_debug_print(ITP_ERRC_PARSE_FAILED, "itp_parse_package> Package length < %d, but early check failed", size);
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_PARSE_FAILED;
        return NULL;
    }

    // Check CRC
    if (!skip_crc) {
        size -= 2;
        uint16_t crc = itp_read_uint16(package->data + size);
        if (crc != itp_eval_crc16(package->data, size)) {
            itp_debug_print(ITP_ERRC_CRC_FAILED, "itp_parse_package> CRC failed");
            itp_print_stack_trace(stdout);
            if (error) *error = ITP_ERRC_CRC_FAILED;
            return NULL;
        }
    }

    // Read header
    itp_frame_tp frame = itp_malloc(sizeof(itp_frame_t), "itp_parse_package:itp_frame");
    if (!frame) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_parse_package> Memory allocation failed");
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_MALLOC_FAILED;
        return NULL;
    }
    itp_init_frame(frame, itp_read_uint16(package->data + ITP_OFFSET_COMMAND));
    frame->from = package->data[ITP_OFFSET_ADDRESS_FROM];
    frame->to = package->data[ITP_OFFSET_ADDRESS_TO];
    frame->order = itp_read_uint16(package->data + ITP_OFFSET_ORDER);
    frame->status = package->data[ITP_OFFSET_STATUS] & 0x07;
    if ((package->data[ITP_OFFSET_STATUS] >> 7) & 0x01) {
        if (data_size >= 2) {
            itp_byte_t* value = (itp_byte_t*)(&frame->error);
            *value = package->data[ITP_HEADER_SIZE + 1];
            *(value + 1) = package->data[ITP_HEADER_SIZE];
            if (data_size > 2) {
                data_size -= 2;
                header_size += 2;
                goto READ_DATA;
            }
        } else {
            frame->error = ITP_ERR_BAD_DATA;
            itp_debug_print(ITP_ERRC_PARSE_FAILED, "itp_parse_package> Error flag is set, but no data found");
        }
        return frame;
    }

    // Read data
READ_DATA:
    if (data_size > 0) {
        itp_error_code_t err = itp_prepare_frame(frame, data_size);
        if (err) {
            itp_free(frame);
            if (error) *error = err;
            return NULL;
        }
        memcpy(frame->data, package->data + header_size, data_size);
    }
    return frame;
}

itp_package_tp itp_serialize_frame(itp_frame_tp frame, itp_error_code_t* error) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_serialize_frame> Frame is null");
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_INTERNAL;
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Allocate memory
    itp_package_tp package = itp_malloc(sizeof(itp_package_t), "itp_serialize_frame:itp_package");
    if (!package) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_serialize_frame> Memory allocation failed");
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_MALLOC_FAILED;
        return NULL;
    }
    itp_init_package(package);
    itp_size_t size = ITP_HEADER_SIZE + frame->length;
    if (frame->error) size += 2;
    itp_error_code_t errc = itp_prepare_package(package, size);
    if (errc) {
        if (error) *error = errc;
        itp_free_package(package);
        return NULL;
    }

    // Write header
    package->use_checksum = 0;
    package->address = frame->to;
    package->data[ITP_OFFSET_VERSION] = ITP_REVISION | 0x80;
    package->data[ITP_OFFSET_HEADER_SIZE] = (itp_byte_t)ITP_HEADER_SIZE;
    package->data[ITP_OFFSET_ADDRESS_FROM] = frame->from;
    package->data[ITP_OFFSET_ADDRESS_TO] = frame->to;
    itp_write_uint16(package->data + ITP_OFFSET_ORDER, frame->order);
    package->data[ITP_OFFSET_STATUS] = frame->status & 0x07;
    itp_write_uint16(package->data + ITP_OFFSET_COMMAND, frame->command);

    uint16_t data_size = 0;
    if (frame->error) {
        // Write error
        //itp_byte_t* value = (itp_byte_t*)(&frame->error);
        package->data[ITP_OFFSET_STATUS] |= 0x80;
        itp_write_uint16(package->data + ITP_HEADER_SIZE, frame->error);
        //package->data[ITP_HEADER_SIZE] = *(value + 1);
        //package->data[ITP_HEADER_SIZE + 1] = *value;
        data_size += 2;
    }
    if (frame->length > 0) {
        // Write data
        memcpy(package->data + ITP_HEADER_SIZE + data_size, frame->data, frame->length);
        data_size += (uint16_t)frame->length;
    }
    itp_write_uint16(package->data + ITP_OFFSET_DATA_SIZE, data_size);
    return package;
}

uint8_t itp_get_package_version(itp_package_tp package, itp_error_code_t* error) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!package) {
        if (error) *error = ITP_ERRC_INTERNAL;
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_get_package_version> Package is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (package->size <= ITP_OFFSET_VERSION) {
        if (error) *error = ITP_ERRC_PARSE_FAILED;
        itp_debug_print(ITP_ERRC_PARSE_FAILED, "itp_get_package_version> Failed to parse package");
        itp_print_stack_trace(stdout);
        return 0;
    } else {
        uint8_t version = package->data[ITP_OFFSET_VERSION] & 0x3F;
        return version;
    }
}

uint8_t itp_get_package_from(itp_package_tp package, itp_error_code_t* error) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!package) {
        if (error) *error = ITP_ERRC_INTERNAL;
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_get_package_from> Package is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (package->size <= ITP_OFFSET_ADDRESS_FROM) {
        if (error) *error = ITP_ERRC_PARSE_FAILED;
        itp_debug_print(ITP_ERRC_PARSE_FAILED, "itp_get_package_from> Failed to parse package");
        itp_print_stack_trace(stdout);
        return 0;
    } else {
        uint8_t from = package->data[ITP_OFFSET_ADDRESS_FROM];
        return from;
    }
}

uint8_t itp_get_package_to(itp_package_tp package, itp_error_code_t* error) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!package) {
        if (error) *error = ITP_ERRC_INTERNAL;
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_get_package_to> Package is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (package->size <= ITP_OFFSET_ADDRESS_TO) {
        if (error) *error = ITP_ERRC_PARSE_FAILED;
        itp_debug_print(ITP_ERRC_PARSE_FAILED, "itp_get_package_to> Failed to parse package");
        itp_print_stack_trace(stdout);
        return 0;
    } else {
        uint8_t to = package->data[ITP_OFFSET_ADDRESS_TO];
        return to;
    }
}

uint16_t itp_get_package_command(itp_package_tp package, itp_error_code_t* error) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!package) {
        if (error) *error = ITP_ERRC_INTERNAL;
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_get_package_command> Package is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (package->size <= ITP_OFFSET_COMMAND + 1) {
        if (error) *error = ITP_ERRC_PARSE_FAILED;
        itp_debug_print(ITP_ERRC_PARSE_FAILED, "itp_get_package_command> Failed to parse package");
        itp_print_stack_trace(stdout);
        return 0;
    } else {
        return itp_read_uint16(package->data + ITP_OFFSET_COMMAND);
    }
}

uint16_t itp_get_package_order(itp_package_tp package, itp_error_code_t* error) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!package) {
        if (error) *error = ITP_ERRC_INTERNAL;
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_get_package_order> Package is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (package->size <= ITP_OFFSET_ORDER + 1) {
        if (error) *error = ITP_ERRC_PARSE_FAILED;
        itp_debug_print(ITP_ERRC_PARSE_FAILED, "itp_get_package_order> Failed to parse package");
        itp_print_stack_trace(stdout);
        return 0;
    } else {
        return itp_read_uint16(package->data + ITP_OFFSET_ORDER);
    }
}

void itp_append_checksum(itp_buffer_tp buffer) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_append_checksum> Buffer is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    uint8_t no_crc = buffer->data[ITP_OFFSET_VERSION] & 0x80;
    if (no_crc) {
        buffer->data[ITP_OFFSET_VERSION] &= 0x7F;
        uint16_t crc = itp_eval_crc16(buffer->data, buffer->length);
        itp_write_uint16(buffer->data + buffer->length, crc);
        buffer->length += 2;
    }
}

void itp_remove_checksum(itp_buffer_tp buffer) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_remove_checksum> Buffer is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    uint8_t no_crc = buffer->data[ITP_OFFSET_VERSION] & 0x80;
    if (!no_crc) {
        buffer->data[ITP_OFFSET_VERSION] |= 0x80;
        buffer->length -= 2;
    }
}
