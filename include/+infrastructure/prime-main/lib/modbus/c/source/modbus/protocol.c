#include <modbus/debug.h>
#include <modbus/protocol.h>
#include <modbus/symbol.h>
#include <modbus/task.h>
#include <modbus/frame.h>
#include <modbus/queue.h>
#include <modbus/master.h>
#include <modbus/device.h>
#include <modbus/range.h>
#include <modbus/tools.h>
#include <modbus/memory.h>
#include <modbus/stack_trace.h>

#include <string.h>

mbs_size_t mbs_get_pdu_length_master(mbs_buffer_tp buffer) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_get_pdu_length_master> Buffer is null");
        mbs_print_stack_trace(stdout);
        return 0;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (!buffer) return 0;
    if (buffer->data[1] & 0x80) return 2;
    mbs_byte_t command = buffer->data[1] & 0x7F;
    switch (command) {
        case MBS_CMD_READ_COILS:
        case MBS_CMD_READ_DISCRETE_INPUTS:
        case MBS_CMD_READ_HOLDING_REGISTERS:
        case MBS_CMD_READ_INPUT_REGISTERS:
            return buffer->data[2] + 2;
        case MBS_CMD_WRITE_SINGLE_COIL:
        case MBS_CMD_WRITE_SINGLE_REGISTER:
        case MBS_CMD_WRITE_MULTIPLE_COILS:
        case MBS_CMD_WRITE_MULTIPLE_REGISTERS:
            return 5;
        default:
            mbs_debug_print(MBS_ERRC_WARNING, "mbs_get_pdu_length_master> Unsupported command found 0x%02x", command);
            return 0;
    }
}

mbs_size_t mbs_get_pdu_length_slave(mbs_buffer_tp buffer) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_get_pdu_length_slave> Buffer is null");
        mbs_print_stack_trace(stdout);
        return 0;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (!buffer) return 0;
    mbs_byte_t command = buffer->data[1] & 0x7F;
    switch (command) {
        case MBS_CMD_READ_COILS:
        case MBS_CMD_READ_DISCRETE_INPUTS:
        case MBS_CMD_READ_HOLDING_REGISTERS:
        case MBS_CMD_READ_INPUT_REGISTERS:
        case MBS_CMD_WRITE_SINGLE_COIL:
        case MBS_CMD_WRITE_SINGLE_REGISTER:
            return 5;
        case MBS_CMD_WRITE_MULTIPLE_COILS:
        case MBS_CMD_WRITE_MULTIPLE_REGISTERS:
            if (buffer->length >= 7) {
                return buffer->data[6] + 6;
            } else return 7;
        default:
            mbs_debug_print(MBS_ERRC_WARNING, "mbs_get_pdu_length_slave> Unsupported command found 0x%02x", command);
            return 0;
    }
}

mbs_error_code_t mbs_bufferize_request(mbs_master_tp master, mbs_buffer_tp buffer, mbs_request_tp request) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_bufferize_request> Master is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!buffer) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_bufferize_request> Buffer is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!request) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_bufferize_request> Request is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_task_tp task = request->task;
    mbs_frame_t frame;
    mbs_init_frame(&frame);
    frame.slave = task->device->address;
    frame.command = task->command;
    mbs_error_code_t errc = MBS_ERRC_NONE;

    switch (task->command) {
        case MBS_CMD_READ_COILS:
        case MBS_CMD_READ_DISCRETE_INPUTS:
        case MBS_CMD_READ_HOLDING_REGISTERS:
        case MBS_CMD_READ_INPUT_REGISTERS:
            errc = mbs_prepare_frame(&frame, 4);
            if (errc) {
                mbs_dispose_frame(&frame);
                return MBS_ERRC_INTERNAL;
            } else {
                mbs_write_register(frame.data, task->address);
                mbs_write_register(frame.data + 2, task->count);
            }
            break;
        case MBS_CMD_WRITE_SINGLE_COIL:
        case MBS_CMD_WRITE_SINGLE_REGISTER:
            errc = mbs_prepare_frame(&frame, 4);
            if (errc) {
                mbs_dispose_frame(&frame);
                return MBS_ERRC_INTERNAL;
            } else {
                mbs_write_register(frame.data, task->address);
                uint16_t value = 0;
                if (task->command == MBS_CMD_WRITE_SINGLE_COIL) {
                    uint8_t coil = 0;
                    errc = ((mbs_master_write_single_coil_fp)task->on_request)(
                        #ifdef MODBUS_EXTEND_CALLBACKS
                        task->on_request_eparam,
                        #endif // MODBUS_EXTEND_CALLBACKS
                        master, task->device, task->address, &coil
                    );
                    value = coil ? 0xFF00 : 0;
                } else {
                    errc = ((mbs_master_write_single_register_fp)task->on_request)(
                        #ifdef MODBUS_EXTEND_CALLBACKS
                        task->on_request_eparam,
                        #endif // MODBUS_EXTEND_CALLBACKS
                        master, task->device, task->address, &value
                    );
                }
                if (errc) {
                    mbs_dispose_frame(&frame);
                    return MBS_ERRC_INTERNAL;
                } else {
                    mbs_write_register(frame.data + 2, value);
                }
            }
            break;
        case MBS_CMD_WRITE_MULTIPLE_COILS:
            {
                uint8_t* coils = (uint8_t*)mbs_malloc(sizeof(uint8_t) * task->count, "mbs_bufferize_request:coils");
                if (!coils) {
                    mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_bufferize_request> Memory allocation failed");
                    return MBS_ERRC_MALLOC_FAILED;
                }
                errc = ((mbs_master_write_multiple_coils_fp)task->on_request)(
                    #ifdef MODBUS_EXTEND_CALLBACKS
                    task->on_request_eparam,
                    #endif // MODBUS_EXTEND_CALLBACKS
                    master, task->device, task->address, task->count, coils
                );
                if (errc) {
                    mbs_free(coils);
                    return MBS_ERRC_INTERNAL;
                }
                uint16_t size = task->count / 8;
                if (task->count % 8) size++;
                errc = mbs_prepare_frame(&frame, size + 5);
                if (errc) {
                    mbs_free(coils);
                    mbs_dispose_frame(&frame);
                    return MBS_ERRC_INTERNAL;
                }
                mbs_write_register(frame.data, task->address);
                mbs_write_register(frame.data + 2, task->count);
                frame.data[4] = size;
                for (uint16_t i = 0; i < task->count; ++i) {
                    if (coils[i]) {
                        frame.data[i / 8 + 5] |= (0x01 << (i % 8));
                    }
                }
                mbs_free(coils);
            }
            break;
        case MBS_CMD_WRITE_MULTIPLE_REGISTERS:
            errc = mbs_prepare_frame(&frame, task->count * 2 + 5);
            if (errc) {
                mbs_dispose_frame(&frame);
                return MBS_ERRC_INTERNAL;
            } else {
                mbs_write_register(frame.data, task->address);
                mbs_write_register(frame.data + 2, task->count);
                frame.data[4] = task->count * 2;
                errc = ((mbs_master_write_multiple_registers_fp)task->on_request)(
                    #ifdef MODBUS_EXTEND_CALLBACKS
                    task->on_request_eparam,
                    #endif // MODBUS_EXTEND_CALLBACKS
                    master, task->device, task->address, task->count, (uint16_t*)(frame.data + 5)
                );
                if (errc) {
                    mbs_dispose_frame(&frame);
                    return MBS_ERRC_INTERNAL;
                }
                mbs_byte_t t;
                for (uint16_t i = 0, j, k; i < task->count; ++i) {
                    j = 5 + i * 2;
                    k = j + 1;
                    t = frame.data[j];
                    frame.data[j] = frame.data[k];
                    frame.data[k] = t;
                }
            }
            break;
        default:
            mbs_debug_print(MBS_ERRC_WARNING, "mbs_bufferize_request> Unsupported command found 0x%02x", task->command);
            break;
    }

    errc = master->serialize(buffer, &frame);
    mbs_dispose_frame(&frame);
    return errc;
}

mbs_error_code_t mbs_call_on_result(mbs_master_tp master, mbs_device_tp device, mbs_request_tp request, mbs_frame_tp frame) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_call_on_result> Master is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!device) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_call_on_result> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!request) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_call_on_result> Request is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!frame) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_call_on_result> Frame is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (frame->error) {
        mbs_report_task_error(master, device, request->task, frame->error);
        return MBS_ERRC_NONE;
    }

    mbs_task_tp task = request->task;

    switch (frame->command) {
        case MBS_CMD_READ_COILS:
        case MBS_CMD_READ_DISCRETE_INPUTS:
            {
                uint16_t size = task->count / 8;
                if (task->count % 8) size++;
                if (frame->data[0] != size) {
                    mbs_debug_print(MBS_ERRC_BAD_RESPONSE, "mbs_call_on_result> Expected %d bytes, got %d", size, frame->data[0]);
                    mbs_report_task_error(master, device, request->task, MBS_ERR_BAD_DATA);
                    return MBS_ERRC_NONE;
                }
                uint8_t* data = (uint8_t*)mbs_malloc(sizeof(uint8_t) * task->count, "mbs_call_on_result:data");
                if (!data) {
                    mbs_report_task_error(master, device, task, MBS_ERR_INTERNAL);
                } else {
                    for (uint16_t i = 0; i < task->count; ++i) {
                        if ((frame->data[i / 8 + 1] >> (i % 8)) & 0x1) {
                            data[i] = 0xFF;
                        } else data[i] = 0x00;
                    }
                    ((mbs_master_read_coils_fp)task->on_response)(
                        #ifdef MODBUS_EXTEND_CALLBACKS
                        task->on_response_eparam,
                        #endif // MODBUS_EXTEND_CALLBACKS
                        master, device, MBS_ERR_NONE,
                        task->address, task->count, data
                    );
                    mbs_free(data);
                }
            }
            break;
        case MBS_CMD_READ_HOLDING_REGISTERS:
        case MBS_CMD_READ_INPUT_REGISTERS:
            {
                uint16_t size = task->count * 2;
                if (frame->data[0] != size) {
                    mbs_debug_print(MBS_ERRC_BAD_RESPONSE, "mbs_call_on_result> Expected %d bytes, got %d", size, frame->data[0]);
                    mbs_report_task_error(master, device, request->task, MBS_ERR_BAD_DATA);
                    return MBS_ERRC_NONE;
                }
                uint16_t* data = (uint16_t*)mbs_malloc(sizeof(uint16_t) * task->count, "mbs_call_on_result:data");
                if (!data) {
                    mbs_report_task_error(master, device, task, MBS_ERR_INTERNAL);
                } else {
                    for (uint16_t i = 0; i < task->count; ++i) {
                        data[i] = mbs_read_register(frame->data + i * 2 + 1);
                    }
                    ((mbs_master_read_holding_registers_fp)task->on_response)(
                        #ifdef MODBUS_EXTEND_CALLBACKS
                        task->on_response_eparam,
                        #endif // MODBUS_EXTEND_CALLBACKS
                        master, device, MBS_ERR_NONE,
                        task->address, task->count, data
                    );
                    mbs_free(data);
                }
            }
            break;
        case MBS_CMD_WRITE_SINGLE_COIL:
        case MBS_CMD_WRITE_SINGLE_REGISTER:
        case MBS_CMD_WRITE_MULTIPLE_COILS:
        case MBS_CMD_WRITE_MULTIPLE_REGISTERS:
            if (task->on_response) {
                ((mbs_master_result_fp)task->on_response)(
                    #ifdef MODBUS_EXTEND_CALLBACKS
                    task->on_response_eparam,
                    #endif // MODBUS_EXTEND_CALLBACKS
                    master, device, MBS_ERR_NONE
                );
            }
            break;
        default:
            mbs_debug_print(MBS_ERRC_WARNING, "mbs_call_on_result> Unsupported command found 0x%02x", frame->command);
            break;
    }
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_call_on_request(mbs_slave_tp slave, mbs_range_tp range, mbs_frame_tp request, mbs_frame_tp response) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!slave) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_call_on_request> Slave is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!range) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_call_on_request> Range is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!request) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_call_on_request> Request is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!response) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_call_on_request> Response is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    // Get bounds
    switch (request->command) {
        case MBS_CMD_READ_COILS:
        case MBS_CMD_READ_DISCRETE_INPUTS:
            {
                // Allocate memory
                uint16_t first = mbs_read_register(request->data);
                uint16_t count = mbs_read_register(request->data + 2);
                uint16_t last = first + count - 1;
                uint16_t end, current_count;
                uint16_t size = count / 8;
                if (count % 8) size++;
                if (size > 0xFF) {
                    mbs_debug_print(MBS_ERRC_BAD_ARGUMENT, "mbs_call_on_request> Coils count out of range");
                    response->error = MBS_ERR_ILLEGAL_DATA_ADDRESS;
                    return MBS_ERRC_NONE;
                }
                size_t memory_size = sizeof(uint8_t) * count;
                uint8_t* data = (uint8_t*)mbs_malloc(memory_size, "mbs_call_on_request:data");
                if (!data) {
                    mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_call_on_request> Memory allocation failed");
                    return MBS_ERRC_MALLOC_FAILED;
                } else {
                    memset(data, 0x0, memory_size);
                }

                // Iterate over ranges
                mbs_ranged_handler_tp it = range->first;
                uint8_t* data_it = data;
                while (it && (first <= last)) {
                    if ((first < it->first) ||
                        (first > it->last)) {
                        it = it->next;
                        continue;
                    }
                    end = (last > it->last) ? it->last : last;
                    current_count = end - first + 1;
                    response->error = ((mbs_slave_read_coils_fp)it->handler)(
                        #ifdef MODBUS_EXTEND_CALLBACKS
                        it->eparam,
                        #endif // MODBUS_EXTEND_CALLBACKS
                        slave, first, current_count, data_it
                    );
                    data_it += current_count;
                    if (!response->error) {
                        first = end + 1;
                    } else {
                        mbs_free(data);
                        return MBS_ERRC_NONE;
                    }
                    it = it->next;
                }

                // Return operation result
                if (first <= last) {
                    response->error = MBS_ERR_ILLEGAL_DATA_ADDRESS;
                } else {
                    mbs_error_code_t errc = mbs_prepare_frame(response, size + 1);
                    if (errc) {
                        mbs_free(data);
                        return errc;
                    }
                    response->data[0] = (uint8_t)size;
                    for (uint16_t i = 0; i < count; ++i) {
                        if (data[i]) {
                            response->data[i / 8 + 1] |= 0x1 << (i % 8);
                        }
                    }
                }
                mbs_free(data);
            }
            break;
        case MBS_CMD_READ_HOLDING_REGISTERS:
        case MBS_CMD_READ_INPUT_REGISTERS:
            {
                // Allocate memory
                uint16_t first = mbs_read_register(request->data);
                uint16_t count = mbs_read_register(request->data + 2);
                uint16_t last = first + count - 1;
                uint16_t end, current_count;
                if (count > 0x7F) {
                    mbs_debug_print(MBS_ERRC_BAD_ARGUMENT, "mbs_call_on_request> Registers count out of range");
                    response->error = MBS_ERR_ILLEGAL_DATA_ADDRESS;
                    return MBS_ERRC_NONE;
                }
                size_t memory_size = sizeof(uint16_t) * count;
                uint16_t* data = (uint16_t*)mbs_malloc(memory_size, "mbs_call_on_request:data");
                if (!data) {
                    mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_call_on_request> Memory allocation failed");
                    return MBS_ERRC_MALLOC_FAILED;
                } else {
                    memset(data, 0x0, memory_size);
                }

                // Iterate over ranges
                mbs_ranged_handler_tp it = range->first;
                uint16_t* data_it = data;
                while (it && (first <= last)) {
                    if ((first < it->first) ||
                        (first > it->last)) {
                        it = it->next;
                        continue;
                    }
                    end = (last > it->last) ? it->last : last;
                    current_count = end - first + 1;
                    response->error = ((mbs_slave_read_holding_registers_fp)it->handler)(
                        #ifdef MODBUS_EXTEND_CALLBACKS
                        it->eparam,
                        #endif // MODBUS_EXTEND_CALLBACKS
                        slave, first, current_count, data_it
                    );
                    data_it += current_count;
                    if (!response->error) {
                        first = end + 1;
                    } else {
                        mbs_free(data);
                        return MBS_ERRC_NONE;
                    }
                    it = it->next;
                }

                // Return operation result
                if (first <= last) {
                    response->error = MBS_ERR_ILLEGAL_DATA_ADDRESS;
                } else {
                    uint16_t size = count * 2;
                    mbs_error_code_t errc = mbs_prepare_frame(response, size + 1);
                    if (errc) {
                        mbs_free(data);
                        return errc;
                    }
                    response->data[0] = (uint8_t)size;
                    for (uint16_t i = 0; i < count; ++i) {
                        mbs_write_register(response->data + i * 2 + 1, data[i]);
                    }
                }
                mbs_free(data);
            }
            break;
        case MBS_CMD_WRITE_SINGLE_COIL:
        case MBS_CMD_WRITE_MULTIPLE_COILS:
            {
                // Allocate memory
                uint16_t first = mbs_read_register(request->data);
                uint16_t count = 1;
                if (request->command == MBS_CMD_WRITE_MULTIPLE_COILS)
                    count = mbs_read_register(request->data + 2);
                uint16_t last = first + count - 1;
                uint16_t end, current_count;
                uint16_t origin = first;
                size_t memory_size = sizeof(uint8_t) * count;
                if (request->command == MBS_CMD_WRITE_MULTIPLE_COILS) {
                    mbs_byte_t data_size = count / 8;
                    if (count % 8) data_size++;
                    if (data_size != (size_t)request->data[4]) {
                        mbs_debug_print(MBS_ERRC_BAD_RESPONSE, "mbs_call_on_request> Bad data in request");
                        response->error = MBS_ERR_ILLEGAL_DATA_VALUE;
                        return MBS_ERRC_NONE;
                    }
                }
                uint8_t* data = (uint8_t*)mbs_malloc(memory_size, "mbs_call_on_request:data");
                if (!data) {
                    mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_call_on_request> Memory allocation failed");
                    return MBS_ERRC_MALLOC_FAILED;
                } else {
                    if (request->command == MBS_CMD_WRITE_MULTIPLE_COILS) {
                        for (uint16_t i = 0; i < count; ++i) {
                            if ((request->data[i / 8 + 5] >> (i % 8)) & 0x1) {
                                data[i] = 0xFF;
                            } else data[i] = 0x00;
                        }
                    } else {
                        uint16_t value = mbs_read_register(request->data + 2);
                        if (value == 0xFF00) {
                            *data = 1;
                        } else if (value == 0x0000) {
                            *data = 0;
                        } else {
                            mbs_free(data);
                            mbs_debug_print(MBS_ERRC_BAD_RESPONSE, "mbs_call_on_request> Bad data in request");
                            response->error = MBS_ERR_ILLEGAL_DATA_VALUE;
                            return MBS_ERRC_NONE;
                        }
                    }
                }

                // Iterate over ranges
                mbs_ranged_handler_tp it = range->first;
                uint8_t* data_it = data;
                while (it && (first <= last)) {
                    if ((first < it->first) ||
                        (first > it->last)) {
                        it = it->next;
                        continue;
                    }
                    end = (last > it->last) ? it->last : last;
                    current_count = end - first + 1;
                    response->error = ((mbs_slave_write_multiple_coils_fp)it->handler)(
                        #ifdef MODBUS_EXTEND_CALLBACKS
                        it->eparam,
                        #endif // MODBUS_EXTEND_CALLBACKS
                        slave, first, current_count, data_it
                    );
                    data_it += current_count;
                    if (!response->error) {
                        first = end + 1;
                    } else {
                        mbs_free(data);
                        return MBS_ERRC_NONE;
                    }
                    it = it->next;
                }

                // Return operation result
                if (first <= last) {
                    response->error = MBS_ERR_ILLEGAL_DATA_ADDRESS;
                } else {
                    mbs_error_code_t errc = mbs_prepare_frame(response, 4);
                    if (errc) {
                        mbs_free(data);
                        return errc;
                    }
                    mbs_write_register(response->data, origin);
                    if (request->command == MBS_CMD_WRITE_MULTIPLE_COILS) {
                        mbs_write_register(response->data + 2, count);
                    } else {
                        mbs_write_register(response->data + 2, data[0] ? 0xFF00 : 0x0000);
                    }
                }
                mbs_free(data);
            }
            break;
        case MBS_CMD_WRITE_SINGLE_REGISTER:
        case MBS_CMD_WRITE_MULTIPLE_REGISTERS:
            {
                // Allocate memory
                uint16_t first = mbs_read_register(request->data);
                uint16_t count = 1;
                if (request->command == MBS_CMD_WRITE_MULTIPLE_REGISTERS)
                    count = mbs_read_register(request->data + 2);
                uint16_t last = first + count - 1;
                uint16_t end, current_count;
                uint16_t origin = first;
                size_t memory_size = sizeof(uint16_t) * count;
                if (request->command == MBS_CMD_WRITE_MULTIPLE_REGISTERS) {
                    if (memory_size != (size_t)request->data[4]) {
                        mbs_debug_print(MBS_ERRC_BAD_RESPONSE, "mbs_call_on_request> Bad data in request");
                        response->error = MBS_ERR_ILLEGAL_DATA_VALUE;
                        return MBS_ERRC_NONE;
                    }
                }
                uint16_t* data = (uint16_t*)mbs_malloc(memory_size, "mbs_call_on_request:data");
                if (!data) {
                    mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_call_on_request> Memory allocation failed");
                    return MBS_ERRC_MALLOC_FAILED;
                } else {
                    if (request->command == MBS_CMD_WRITE_MULTIPLE_REGISTERS) {
                        for (uint16_t i = 0; i < count; ++i) {
                            data[i] = mbs_read_register(request->data + i * 2 + 5);
                        }
                    } else {
                        *data = mbs_read_register(request->data + 2);
                    }
                }

                // Iterate over ranges
                mbs_ranged_handler_tp it = range->first;
                uint16_t* data_it = data;
                while (it && (first <= last)) {
                    if ((first < it->first) ||
                        (first > it->last)) {
                        it = it->next;
                        continue;
                    }
                    end = (last > it->last) ? it->last : last;
                    current_count = end - first + 1;
                    response->error = ((mbs_slave_write_multiple_registers_fp)it->handler)(
                        #ifdef MODBUS_EXTEND_CALLBACKS
                        it->eparam,
                        #endif // MODBUS_EXTEND_CALLBACKS
                        slave, first, current_count, data_it
                    );
                    data_it += current_count;
                    if (!response->error) {
                        first = end + 1;
                    } else {
                        mbs_free(data);
                        return MBS_ERRC_NONE;
                    }
                    it = it->next;
                }

                // Return operation result
                if (first <= last) {
                    response->error = MBS_ERR_ILLEGAL_DATA_ADDRESS;
                } else {
                    mbs_error_code_t errc = mbs_prepare_frame(response, 4);
                    if (errc) {
                        mbs_free(data);
                        return errc;
                    }
                    mbs_write_register(response->data, origin);
                    if (request->command == MBS_CMD_WRITE_MULTIPLE_REGISTERS) {
                        mbs_write_register(response->data + 2, count);
                    } else {
                        mbs_write_register(response->data + 2, *data);
                    }
                }
                mbs_free(data);
            }
            break;
        default:
            response->error = MBS_ERR_ILLEGAL_FUNCTION;
            mbs_debug_print(MBS_ERRC_WARNING, "mbs_call_on_request> Unsupported command found 0x%02x", request->command);
            return MBS_ERRC_NONE;
    }
    return MBS_ERRC_NONE;
}

void mbs_report_task_error(mbs_master_tp master, mbs_device_tp device, mbs_task_tp task, mbs_byte_t error) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_report_task_error> Master is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    if (!device) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_report_task_error> Device is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    if (!task) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_report_task_error> Task is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (task->on_response) {
        switch (task->command) {
            case MBS_CMD_READ_COILS:
            case MBS_CMD_READ_DISCRETE_INPUTS:
                ((mbs_master_read_coils_fp)task->on_response)(
                    #ifdef MODBUS_EXTEND_CALLBACKS
                    task->on_response_eparam,
                    #endif // MODBUS_EXTEND_CALLBACKS
                    master, device, error,
                    task->address, task->count, NULL
                );
                break;
            case MBS_CMD_READ_HOLDING_REGISTERS:
            case MBS_CMD_READ_INPUT_REGISTERS:
                ((mbs_master_read_holding_registers_fp)task->on_response)(
                    #ifdef MODBUS_EXTEND_CALLBACKS
                    task->on_response_eparam,
                    #endif // MODBUS_EXTEND_CALLBACKS
                    master, device, error,
                    task->address, task->count, NULL
                );
                break;
            case MBS_CMD_WRITE_SINGLE_COIL:
            case MBS_CMD_WRITE_SINGLE_REGISTER:
            case MBS_CMD_WRITE_MULTIPLE_COILS:
            case MBS_CMD_WRITE_MULTIPLE_REGISTERS:
                ((mbs_master_result_fp)task->on_response)(
                    #ifdef MODBUS_EXTEND_CALLBACKS
                    task->on_response_eparam,
                    #endif // MODBUS_EXTEND_CALLBACKS
                    master, device, error
                );
                break;
            default:
                mbs_debug_print(MBS_ERRC_WARNING, "mbs_report_task_error> Unsupported command found 0x%02x", task->command);
                break;
        }
    }
}
