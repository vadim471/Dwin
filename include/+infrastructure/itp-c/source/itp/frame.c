#include <itp/debug.h>
#include <itp/frame.h>
#include <itp/error.h>
#include <itp/memory.h>
#include <itp/stack_trace.h>

#include <string.h>
#include <inttypes.h>


//---------------------------------------------------------------------------
// Dsc : Инициализация фрейма, сброс всех полей
//---------------------------------------------------------------------------
void itp_init_frame(itp_frame_tp frame, uint16_t command) 
{

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_init_frame> Frame is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    frame->version = ITP_REVISION;
    frame->from    = 0;
    frame->to      = 0;
    frame->command = command;
    frame->status  = 0;
    frame->error   = ITP_ERR_NONE;
    frame->data    = NULL;
    frame->length  = 0;
    frame->order   = 0;
    frame->offset  = 0;
}


//---------------------------------------------------------------------------
// Dsc : Создание фрейма, выделение памяти
//---------------------------------------------------------------------------
itp_frame_tp itp_create_frame(uint16_t command) 
{
    itp_frame_tp frame = (itp_frame_tp)itp_malloc(sizeof(itp_frame_t), "itp_create_frame:itp_frame");
    if (!frame) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_create_frame> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    itp_init_frame(frame, command);
    return frame;
}

//---------------------------------------------------------------------------
// Dsc:
//---------------------------------------------------------------------------
itp_frame_tp itp_copy_frame(itp_frame_tp frame) 
{

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_copy_frame> Frame is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    itp_frame_tp copy = (itp_frame_tp)itp_malloc(sizeof(itp_frame_t), "itp_copy_frame:itp_frame");
    if (!copy) 
    {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_copy_frame> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return NULL;
    }

    copy->version = frame->version;
    copy->from    = frame->from;
    copy->to      = frame->to;
    copy->command = frame->command;
    copy->status  = frame->status;
    copy->error   = frame->error;
    copy->order   = frame->order;
    copy->offset  = frame->offset;

    if (frame->data && frame->length > 0) 
    {
        copy->data = itp_malloc(frame->length * sizeof(itp_byte_t), "itp_copy_frame:data");
        if (!copy->data) 
        {
            itp_free(copy);
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_copy_frame> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return NULL;
        }
        memcpy(copy->data, frame->data, frame->length);
        copy->length = frame->length;
    } 
    else 
    {
        copy->data = NULL;
        copy->length = 0;
    }
    return copy;
}

void itp_free_frame(itp_frame_tp frame) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_init_frame> Frame is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    itp_dispose_frame(frame);
    itp_free(frame);
}

itp_error_code_t itp_prepare_frame(itp_frame_tp frame, itp_size_t size) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_init_frame> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    itp_dispose_frame(frame);
    if (size > 0) {
        frame->data = itp_malloc(size * sizeof(itp_byte_t), "itp_prepare_frame:data");
        if (!frame->data) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_prepare_frame> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        frame->length = size;
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_expand_frame(itp_frame_tp frame, itp_size_t size) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_expand_frame> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    itp_size_t length = frame->offset + size;
    if (length > frame->length) {
        itp_byte_t* array = itp_malloc(length, "itp_expand_frame:data");
        if (!array) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_expand_frame> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_NULL_POINTER;
        }
        if (frame->offset > 0) {
            if (!frame->data) {
                itp_free(array);
                itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_expand_frame> Frame data is null, but length > 0");
                itp_print_stack_trace(stdout);
                return ITP_ERRC_NULL_POINTER;
            }
            memcpy(array, frame->data, frame->offset);
        }
        if (frame->data)
            itp_free(frame->data);
        frame->data = array;
        frame->length = length;
    }
    return ITP_ERRC_NONE;
}

void itp_dispose_frame(itp_frame_tp frame) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_init_frame> Frame is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (frame->data) {
        itp_free(frame->data);
        frame->data = NULL;
        frame->length = 0;
        frame->offset = 0;
    }
}

void itp_dump_frame(itp_frame_tp frame) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_dump_frame> Frame is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    printf( "Frame dump ------------------------------->\r\n");
    printf( "Memory      : 0x%" PRIuPTR "\r\n", (size_t)frame);
    printf( "Version     : 0x%02x\r\n", frame->version);
    printf( "From        : 0x%02x\r\n", frame->from);
    printf( "To          : 0x%02x\r\n", frame->to);
    printf( "Command     : 0x%04x\r\n", frame->command);
    printf( "Status      : 0x%02x\r\n", frame->status);
    printf( "Error       : 0x%04x\r\n", frame->error);
    printf( "Order       : 0x%04x\r\n", frame->order);
    printf( "Offset      : %d\r\n", (int)frame->offset);
    printf( "Data size   : %d\r\n", (int)frame->length);
    printf( "Data memory : 0x%" PRIuPTR "\r\n", (size_t)frame->data);
    if( frame->length > 0 )
    {
        printf( "Data:" );
        for (itp_size_t i = 0; i < frame->length; ++i)
            printf(" %02x", frame->data[i]);
        printf("\r\n");
    }
    printf("<-----------------------------------------\r\n");
}

void itp_frame_prepare_to_read(itp_frame_tp frame) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_prepare_to_read> Frame is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    frame->offset = 0;
}

void itp_frame_prepare_to_write(itp_frame_tp frame) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_prepare_to_write> Frame is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    frame->offset = frame->length;
}

uint8_t itp_frame_has_more(itp_frame_tp frame) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_has_more> Frame is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (frame->offset < frame->length) {
        return 1;
    } else return 0;
}

itp_size_t itp_frame_remain(itp_frame_tp frame) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_remain> Frame is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (frame->offset < frame->length) {
        return (frame->length - frame->offset);
    } else return 0;
}

//-------------------------------------------------------------------------------------------
// Dsc: переворачивает массив, например 1 2 3 4 5 6 7 => 7 6 5 4 3 2 1
// Par: itp_byte_t*      data - * масcив, данные frame
//      const itp_size_t size - размер, itp_size_е = uint16_t 
// Ret: Нет.
//-------------------------------------------------------------------------------------------
static void itp_invert_bytes( itp_byte_t* data, const itp_size_t size ) 
{
    if( size < 2 ) return;

    itp_byte_t value = 0x00;
    itp_size_t index = 0x00;

    for( itp_size_t i = 0; i < size / 2; i++ ) 
    {
        index       = size - i - 1;
        value       = data[i];
        data[i]     = data[index];
        data[index] = value;
    }
}

itp_error_code_t itp_frame_read_value(itp_frame_tp frame, void* value, uint16_t size) 
{

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_read_value> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if ((size > 1) && (size % 2)) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_read_value> Wrong value size");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Check offset
    if( ( frame->offset + size ) > frame->length ) 
    {
        itp_debug_print( ITP_ERRC_OUT_OF_BOUNDS, "itp_frame_read_value> Not enough data in frame" );
        itp_print_stack_trace( stdout );
        return ITP_ERRC_OUT_OF_BOUNDS;
    }

    // Read value
    if( value )
    {
        memcpy( value, frame->data + frame->offset, size );
        itp_invert_bytes( value, size );
    }
    frame->offset += size;
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_frame_read_value_1b(itp_frame_tp frame, void* value) 
{
    return itp_frame_read_value(frame, value, 1);
}

itp_error_code_t itp_frame_read_value_2b(itp_frame_tp frame, void* value) 
{
    return itp_frame_read_value(frame, value, 2);
}

itp_error_code_t itp_frame_read_value_4b(itp_frame_tp frame, void* value) 
{
    return itp_frame_read_value(frame, value, 4);
}

itp_error_code_t itp_frame_read_value_8b(itp_frame_tp frame, void* value) 
{
    return itp_frame_read_value(frame, value, 8);
}

itp_error_code_t itp_frame_read_string(itp_frame_tp frame, char** string) 
{

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_read_string> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Read string length
    uint8_t length;
    itp_error_code_t error = itp_frame_read_value_1b(frame, &length);
    if (error) return error;

    // Read data
    if (length > 0) {
        if ((frame->offset + length) > frame->length) {
            itp_debug_print(ITP_ERRC_OUT_OF_BOUNDS, "itp_frame_read_string> Not enough data in frame");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OUT_OF_BOUNDS;
        }
        *string = itp_malloc(length + 1, "itp_frame_read_string:data");
        memcpy(*string, frame->data + frame->offset, length);
        (*string)[length] = 0;
        frame->offset += length;
    } else *string = NULL;
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_frame_read_wide_string(itp_frame_tp frame, char** string) 
{

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_read_wide_string> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Read string length
    uint16_t length;
    itp_error_code_t error = itp_frame_read_value_2b(frame, &length);
    if (error) return error;

    // Read data
    if (length > 0) 
    {
        if ((frame->offset + length) > frame->length) 
        {
            itp_debug_print(ITP_ERRC_OUT_OF_BOUNDS, "itp_frame_read_wide_string> Not enough data in frame");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OUT_OF_BOUNDS;
        }
        *string = itp_malloc(length + 1, "itp_frame_read_wide_string:data");
        memcpy(*string, frame->data + frame->offset, length);
        (*string)[length] = 0;
        frame->offset += length;
    } else *string = NULL;
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_frame_read_array( itp_frame_tp frame, void** array, uint16_t* length, uint16_t size )
{

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_read_array> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Read string size
    itp_error_code_t error = itp_frame_read_value_2b( frame, length );
    if (error) return error;

    // Read data
    if (*length > 0)
    {
        itp_size_t bytes = *length * (itp_size_t)size;
        if ((frame->offset + bytes) > frame->length) 
        {
            itp_dump_frame( frame );
            itp_debug_print(ITP_ERRC_OUT_OF_BOUNDS, "itp_frame_read_array> Not enough data in frame OFFSET %d LENGTH %d SIZE %d", frame->offset, frame->length, size);
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OUT_OF_BOUNDS;
        }
        *array = itp_malloc(bytes, "itp_frame_read_array:data");
        memcpy(*array, frame->data + frame->offset, bytes);
        frame->offset += bytes;
        for (itp_size_t i = 0; i < *length; i++) 
        {
            itp_invert_bytes( ( (itp_byte_t*)*array ) + ( i * size ), size );
        }
    } 
    else 
    {
      *array = NULL;
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_frame_read_array_1b(itp_frame_tp frame, void** array, uint16_t* length) 
{
    return itp_frame_read_array(frame, array, length, 1);
}

itp_error_code_t itp_frame_read_array_2b(itp_frame_tp frame, void** array, uint16_t* length) 
{
    return itp_frame_read_array(frame, array, length, 2);
}

itp_error_code_t itp_frame_read_array_4b(itp_frame_tp frame, void** array, uint16_t* length) 
{
    return itp_frame_read_array(frame, array, length, 4);
}

itp_error_code_t itp_frame_read_array_8b(itp_frame_tp frame, void** array, uint16_t* length) 
{
    return itp_frame_read_array(frame, array, length, 8);
}

itp_error_code_t itp_frame_read_wide_array(itp_frame_tp frame, void** array, uint16_t* length, uint8_t size) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_read_wide_array> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Read string size
    itp_error_code_t error = itp_frame_read_value_2b(frame, length);
    if (error) return error;

    // Read data
    if (*length > 0) {
        itp_size_t bytes = *length * (itp_size_t)size;
        if ((frame->offset + bytes) > frame->length) {
            itp_debug_print(ITP_ERRC_OUT_OF_BOUNDS, "itp_frame_read_wide_array> Not enough data in frame");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OUT_OF_BOUNDS;
        }
        *array = itp_malloc(bytes, "itp_frame_read_wide_array:data");
        memcpy(*array, frame->data + frame->offset, bytes);
        frame->offset += bytes;
        for (itp_size_t i = 0; i < *length; i++) {
            itp_invert_bytes(((itp_byte_t*)*array) + (i * size), size);
        }
    } else *array = NULL;
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_frame_read_wide_array_1b(itp_frame_tp frame, void** array, uint16_t* length) {
    return itp_frame_read_wide_array(frame, array, length, 1);
}

itp_error_code_t itp_frame_read_wide_array_2b(itp_frame_tp frame, void** array, uint16_t* length) {
    return itp_frame_read_wide_array(frame, array, length, 2);
}

itp_error_code_t itp_frame_read_wide_array_4b(itp_frame_tp frame, void** array, uint16_t* length) {
    return itp_frame_read_wide_array(frame, array, length, 4);
}

itp_error_code_t itp_frame_read_wide_array_8b(itp_frame_tp frame, void** array, uint16_t* length) {
    return itp_frame_read_wide_array(frame, array, length, 8);
}

itp_error_code_t itp_frame_write_value(itp_frame_tp frame, const void* value, uint8_t size) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_write_value> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!value) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_write_value> Value is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!size) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_write_value> Value size is 0");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Write value
    itp_error_code_t error = itp_expand_frame(frame, size);
    if (error) return error;
    memcpy(frame->data + frame->offset, value, size);
    itp_invert_bytes(frame->data + frame->offset, size);
    frame->offset += size;
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_frame_write_value_1b(itp_frame_tp frame, void* value) 
{
    return itp_frame_write_value(frame, value, 1);
}

itp_error_code_t itp_frame_write_value_2b(itp_frame_tp frame, void* value) 
{
    return itp_frame_write_value(frame, value, 2);
}

itp_error_code_t itp_frame_write_value_4b(itp_frame_tp frame, void* value) 
{
    return itp_frame_write_value(frame, value, 4);
}

itp_error_code_t itp_frame_write_value_8b(itp_frame_tp frame, void* value)
{
    return itp_frame_write_value(frame, value, 8);
}

itp_error_code_t itp_frame_write_string(itp_frame_tp frame, const char* string) 
{

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_write_string> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!string) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_write_string> String is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Write length
    itp_size_t length = (itp_size_t)strlen(string);
    itp_error_code_t error = itp_expand_frame(frame, length + 1);
    if (error) return error;
    error = itp_frame_write_value_1b(frame, &length);
    if (error) return error;

    // Write data
    memcpy(frame->data + frame->offset, string, length);
    frame->offset += length;
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_frame_write_wide_string(itp_frame_tp frame, const char* string) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_write_wide_string> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!string) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_write_wide_string> String is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Write length
    size_t length = (size_t)strlen(string);
    size_t total  = length + 2 + (size_t)frame->offset;
    if (total >= 0xFFFF) length = 0xFFFF - 2 - (size_t)frame->offset;
    itp_error_code_t error = itp_expand_frame(frame, length + 2);
    if (error) return error;
    error = itp_frame_write_value_2b(frame, &length);
    if (error) return error;

    // Write data
    memcpy(frame->data + frame->offset, string, length);
    frame->offset += length;
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_frame_write_array(itp_frame_tp frame, const void* array, uint16_t length, uint16_t size) 
{

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_write_array> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!size) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_write_array> Value size is 0");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Check array length
    if (!array && length > 0) length = 0;

    // Write length
    itp_size_t data_size = length * size;
    // + 2 потому что потом будем записывать длину фрейма в 2 байта itp_frame_write_value_2b
    itp_error_code_t error = itp_expand_frame(frame, data_size + 2);
    if (error) return error;
    error = itp_frame_write_value_2b(frame, &length);
    if (error) return error;

    // Write data
    if (length > 0) 
    {
        memcpy(frame->data + frame->offset, array, data_size);
        for (itp_size_t i = 0; i < length; i++)
            itp_invert_bytes(frame->data + frame->offset + (i * size), size);
        frame->offset += length * size;
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_frame_write_array_1b(itp_frame_tp frame, const void* array, uint16_t length) {
    return itp_frame_write_array(frame, array, length, 1);
}

itp_error_code_t itp_frame_write_array_2b(itp_frame_tp frame, const void* array, uint16_t length) {
    return itp_frame_write_array(frame, array, length, 2);
}

itp_error_code_t itp_frame_write_array_4b(itp_frame_tp frame, const void* array, uint16_t length) {
    return itp_frame_write_array(frame, array, length, 4);
}

itp_error_code_t itp_frame_write_array_8b(itp_frame_tp frame, const void* array, uint16_t length) {
    return itp_frame_write_array(frame, array, length, 8);
}

itp_error_code_t itp_frame_write_wide_array(itp_frame_tp frame, const void* array, uint16_t length, uint8_t size) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_write_wide_array> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!size) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_frame_write_wide_array> Value size is 0");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Check array length
    if (!array && length > 0) length = 0;

    // Write length
    itp_size_t data_size = length * size;
    itp_error_code_t error = itp_expand_frame(frame, data_size + 2);
    if (error) return error;
    error = itp_frame_write_value_2b(frame, &length);
    if (error) return error;

    // Write data
    if (length > 0) {
        memcpy(frame->data + frame->offset, array, data_size);
        for (itp_size_t i = 0; i < length; i++)
            itp_invert_bytes(frame->data + frame->offset + (i * size), size);
        frame->offset += length * size;
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_frame_write_wide_array_1b(itp_frame_tp frame, const void* array, uint16_t length) {
    return itp_frame_write_wide_array(frame, array, length, 1);
}

itp_error_code_t itp_frame_write_wide_array_2b(itp_frame_tp frame, const void* array, uint16_t length) {
    return itp_frame_write_wide_array(frame, array, length, 2);
}

itp_error_code_t itp_frame_write_wide_array_4b(itp_frame_tp frame, const void* array, uint16_t length) {
    return itp_frame_write_wide_array(frame, array, length, 4);
}

itp_error_code_t itp_frame_write_wide_array_8b(itp_frame_tp frame, const void* array, uint16_t length) {
    return itp_frame_write_wide_array(frame, array, length, 8);
}
