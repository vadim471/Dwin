#include <itp/debug.h>
#include <itp/transmit.h>
#include <itp/protocol.h>
#include <itp/endpoint.h>
#include <itp/symbol.h>
#include <itp/memory.h>
#include <itp/stack_trace.h>

#include <string.h>
#ifdef ITP_TRACE_BUFFERS
#include <stdio.h>
#endif // ITP_TRACE_BUFFERS

itp_error_code_t itp_init_buffer(itp_buffer_tp buffer) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_buffer> Buffer is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    buffer->data = NULL;
    buffer->size = 0;
    buffer->length = 0;
    buffer->complete = 0;
    buffer->time = 0;

    return itp_resize_buffer(buffer, ITP_MIN_BUFFER_SIZE, 0);
}

void itp_clear_buffer(itp_buffer_tp buffer) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_clear_buffer> Buffer is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    buffer->length = 0;
    buffer->complete = 0;
}

void itp_flush_buffer(itp_buffer_tp buffer) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_flush_buffer> Buffer is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    buffer->complete = 0;
}

void itp_dump_buffer(itp_buffer_tp buffer) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_dump_buffer> Buffer is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    printf("Buffer dump ------->\r\n");
    if (buffer->length > 0) {
        printf("Data:");
        for (itp_size_t i = 0; i < buffer->length; ++i)
            printf(" %02x", buffer->data[i]);
        printf("\r\n");
    }
    printf("<-------------------\r\n");
}

void itp_dispose_buffer(itp_buffer_tp buffer) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_dispose_buffer> Buffer is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (buffer->data) {
        itp_free(buffer->data);
        buffer->size = 0;
    }
}

itp_error_code_t itp_resize_buffer(itp_buffer_tp buffer, itp_size_t size, uint8_t preserve) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_resize_buffer> Buffer is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (size > ITP_MAX_BUFFER_SIZE) {
        itp_debug_print(ITP_ERRC_OVERFLOW, "itp_resize_buffer> Buffer size exceed max value");
        return ITP_ERRC_OVERFLOW;
    }

    if (!buffer->data) {
        buffer->data = (itp_byte_t*)itp_malloc(size, "itp_resize_buffer:data");
        if (!buffer->data) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_resize_buffer> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        buffer->size = size;
        buffer->length = 0;
    } else {
        itp_byte_t* data = (itp_byte_t*)itp_malloc(size, "itp_resize_buffer:data");
        if (!data) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_resize_buffer> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        if (buffer->length > 0) {
            if (buffer->length > size)
                buffer->length = size;
            if (preserve) {
                memcpy(data, buffer->data, buffer->length);
            }
        }
        itp_free(buffer->data);
        buffer->data = data;
        buffer->size = size;
    }
    return ITP_ERRC_NONE;
}

itp_transmit_status_t itp_receive(itp_buffer_tp buffer, itp_endpoint_tp endpoint, itp_error_code_t* error) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_receive> Buffer is null");
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_NULL_POINTER;
        return ITP_TRANSMIT_ERROR;
    }
    if (!endpoint) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_receive> Endpoint is null");
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_NULL_POINTER;
        return ITP_TRANSMIT_ERROR;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    buffer->complete += itp_read_data(endpoint, buffer->data + buffer->complete, buffer->size - buffer->complete);
    buffer->length = buffer->complete;
    #ifdef ITP_ENABLE_BOUNDARY_CHECKS
    if (buffer->complete > buffer->size) {
        itp_debug_print(ITP_ERRC_OUT_OF_BOUNDS, "itp_receive> Out of bounds on data read");
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_OUT_OF_BOUNDS;
        return ITP_TRANSMIT_ERROR;
    }
    #endif // ITP_ENABLE_BOUNDARY_CHECKS
    if (buffer->complete == 0) {
        return ITP_TRANSMIT_IDLE;
    }

    itp_size_t expected_size = 0;
    itp_parse_status_t status = itp_check_package(buffer, &expected_size);
    if (status == ITP_PARSE_NOT_END) {
        if (expected_size > buffer->size) {
            itp_error_code_t errc = itp_resize_buffer(buffer, expected_size, 1);
            if (errc) {
                if (error) *error = errc;
                return ITP_TRANSMIT_ERROR;
            }
        }
        return ITP_TRANSMIT_PENDING;
    } else if (status == ITP_PARSE_OK) {
        return ITP_TRANSMIT_OK;
    } else {
        status = itp_repair_offset(buffer);
        if (status == ITP_PARSE_NOT_END) {
            return ITP_TRANSMIT_PENDING;
        } else if (status == ITP_PARSE_OK) {
            return ITP_TRANSMIT_OK;
        } else {
            if (error) *error = ITP_ERRC_PARSE_FAILED;
            return ITP_TRANSMIT_ERROR;
        }
    }
}

itp_size_t itp_receive_bytes(
#ifdef ITP_TRACE_BUFFERS
uint8_t root_address,
#endif // ITP_TRACE_BUFFERS
itp_buffer_tp buffer, itp_byte_t* data, itp_size_t size) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_receive_bytes> Buffer is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (buffer->length < size)
        size = buffer->length;
    if (data && size > 0) {
        memcpy(data, buffer->data, size * sizeof(itp_byte_t));
        #ifdef ITP_TRACE_BUFFERS
        {
            char* string = (char*)itp_malloc((int)size * 3, "itp_receive_bytes:debug");
            char* it = string;
            *it = 0;
            for (itp_size_t i = 0; i < size; i++) {
                if (i > 0) {
                    sprintf(it, " ");
                    it += 1;
                }
                sprintf(it, "%02x", data[i] & 0xFF);
                it += 2;
            }
            if (buffer->length >= 9) {
                uint8_t status = buffer->data[6] & 0x7F;
                const char* sym =
                    (status == ITP_SYM_ACK) ? "ACK" :
                    (status == ITP_SYM_NAK) ? "NAK" :
                    (status == ITP_SYM_CTR) ? "CTR" :
                    (status == ITP_SYM_DAT) ? "DAT" :
                    (status == ITP_SYM_REQ) ? "REQ" :
                    "ERR";
                itp_debug_print(ITP_ERRC_TRACE, "Read (%d, %d->%d, %s, 0x%02x%02x): %s (%d)", root_address, buffer->data[2], (int)buffer->data[3], sym, buffer->data[7] & 0xFF, buffer->data[8] & 0xFF, string, size);
            } else if (buffer->length >= 4) {
                itp_debug_print(ITP_ERRC_TRACE, "Read (%d, %d->%d): %s (%d)", root_address, buffer->data[2], buffer->data[3], string, size);
            } else if (buffer->length >= 3) {
                itp_debug_print(ITP_ERRC_TRACE, "Read (%d, %d): %s (%d)", root_address, buffer->data[2], string, size);
            } else itp_debug_print(ITP_ERRC_TRACE, "Read (%d): %s (%d)", root_address, string, size);
            itp_free(string);
        }
        #endif // ITP_TRACE_BUFFERS
    }
    if (buffer->length > size) {
        itp_size_t remain = buffer->length - size;
        //memcpy(buffer->data, buffer->data + size, remain);
        for (itp_size_t i = 0; i < remain; ++i)
            buffer->data[i] = buffer->data[size + i];
        buffer->length = remain;
        buffer->complete = remain;
    } else {
        buffer->length = 0;
        buffer->complete = 0;
    }
    return size;
}

uint8_t itp_transmit_is_ready(itp_buffer_tp buffer) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_transmit_is_ready> Buffer is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (buffer->complete < buffer->length) {
        return 1;
    } else return 0;
}

itp_transmit_status_t itp_transmit(itp_buffer_tp buffer, itp_endpoint_tp endpoint, itp_error_code_t* error) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_transmit> Buffer is null");
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_NULL_POINTER;
        return ITP_TRANSMIT_ERROR;
    }
    if (!endpoint) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_transmit> Endpoint is null");
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_NULL_POINTER;
        return ITP_TRANSMIT_ERROR;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (buffer->complete >= buffer->length) {
        #ifdef ITP_ENABLE_BOUNDARY_CHECKS
        if (buffer->complete > buffer->length) {
            itp_debug_print(ITP_ERRC_WARNING, "itp_transmit> Transmitted more then buffer length");
            itp_print_stack_trace(stdout);
        }
        #endif // ITP_ENABLE_BOUNDARY_CHECKS
        itp_write_data(endpoint, buffer->data, 0);
        return ITP_TRANSMIT_IDLE;
    } else {
        buffer->complete += itp_write_data(endpoint, buffer->data + buffer->complete, buffer->length - buffer->complete);
        if (buffer->complete >= buffer->length) {
            #ifdef ITP_ENABLE_BOUNDARY_CHECKS
            if (buffer->complete > buffer->length) {
                itp_debug_print(ITP_ERRC_WARNING, "itp_transmit> Transmitted more then buffer length");
                itp_print_stack_trace(stdout);
            }
            #endif // ITP_ENABLE_BOUNDARY_CHECKS
            return ITP_TRANSMIT_OK;
        } else return ITP_TRANSMIT_PENDING;
    }
}
