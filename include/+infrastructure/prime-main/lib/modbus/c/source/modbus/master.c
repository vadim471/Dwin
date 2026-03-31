#include <modbus/debug.h>
#include <modbus/stack_trace.h>
#include <modbus/master.h>
#include <modbus/endpoint.h>
#include <modbus/memory.h>
#include <modbus/protocol.h>
#include <modbus/symbol.h>
#include <modbus/frame.h>

#include <modbus/rtu.h>

#include <string.h>
#include <inttypes.h>

mbs_error_code_t mbs_init_master_rtu(mbs_master_tp master, mbs_endpoint_tp endpoint) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_init_master_rtu> Master is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    master->devices = NULL;
    master->devices_size = 0;
    master->devices_count = 0;
    master->line = MBS_LINE_IDLE;
    master->endpoint = endpoint;
    master->extract = &mbs_extract_rtu_frame;
    master->serialize = &mbs_serialize_rtu_frame;
    master->time = 0;
    master->debug_time = 0;
    mbs_init_requests_queue(&master->queue);
    mbs_init_buffer(&master->buffer, &mbs_get_pdu_length_master, &mbs_check_rtu_frame);
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_expand_master(mbs_master_tp master, uint8_t size) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_expand_master> Master is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    if (master->devices) {
        uint16_t new_size = master->devices_size + size;
        if (new_size > 0xFF) {
            mbs_debug_print(MBS_ERRC_OUT_OF_BOUNDS, "mbs_expand_master> Devices array out of bounds");
            mbs_print_stack_trace(stdout);
            return MBS_ERRC_OUT_OF_BOUNDS;
        }
        if (master->devices_size > 0) {
            mbs_device_tp* array = mbs_malloc(new_size * sizeof(mbs_device_tp), "mbs_expand_master:data:1");
            if (!array) {
                mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_expand_master> Failed to allocate memory for devices array");
                mbs_print_stack_trace(stdout);
                return MBS_ERRC_MALLOC_FAILED;
            }
            memcpy(array, master->devices, master->devices_size * sizeof(mbs_device_tp));
            memset(array + master->devices_size, 0, size * sizeof(mbs_device_tp));
            mbs_free(master->devices);
            master->devices = array;
            master->devices_size = (uint8_t)new_size;
        } else {
            mbs_free(master->devices);
            master->devices = mbs_malloc((uint32_t)size * sizeof(mbs_device_tp), "mbs_expand_master:data:2");
            if (!master->devices) {
                mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_expand_master> Failed to allocate memory for devices array");
                mbs_print_stack_trace(stdout);
                return MBS_ERRC_MALLOC_FAILED;
            }
            memset(master->devices, 0, size * sizeof(mbs_device_tp));
            master->devices_size = size;
        }
    } else {
        /*if (size > 0xFF) {
            mbs_debug_print(MBS_ERRC_OUT_OF_BOUNDS, "mbs_expand_master> Devices array out of bounds");
            mbs_print_stack_trace(stdout);
            return MBS_ERRC_OUT_OF_BOUNDS;
        }*/
        master->devices = mbs_malloc((int)size * sizeof(mbs_device_tp), "mbs_expand_master:data:3");
        if (!master->devices) {
            mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_expand_master> Failed to allocate memory for devices array");
            mbs_print_stack_trace(stdout);
            return MBS_ERRC_MALLOC_FAILED;
        }
        master->devices_size = size;
        if (master->devices_count > 0) {
            master->devices_count = 0;
        }
    }
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_create_device(mbs_master_tp master, uint8_t address, mbs_device_tp* device) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_create_device> Master is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    if (address < 0x01) {
        mbs_debug_print(MBS_ERRC_WRONG_ADDRESS, "mbs_create_device> Device address can't be zero");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_WRONG_ADDRESS;
    }
    mbs_device_tp instance = (mbs_device_tp)mbs_malloc(sizeof(mbs_device_t), "mbs_create_device:mbs_device");
    if (!instance) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_create_device> Failed to allocate memory for device");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_init_device(instance, address);
    mbs_error_code_t error = mbs_add_device(master, instance);
    if (error) {
        mbs_free(instance);
        return error;
    }
    if (device) *device = instance;
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_add_device(mbs_master_tp master, mbs_device_tp device) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_add_device> Master is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    if (!device) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_add_device> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    // Проверяем, нет ли уже колонки с таким адресом
    for (uint8_t i = 0; i < master->devices_count; ++i) {
        mbs_debug_print(MBS_ERRC_TRACE, "mbs_add_device> Check device %d with count %d", (int)i, (int)master->devices_count);
        if (master->devices[i] == device) {
            mbs_debug_print(MBS_ERRC_GENERAL, "mbs_add_device> This device already added");
            mbs_print_stack_trace(stdout);
            return MBS_ERRC_GENERAL;
        #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
        } else if (!master->devices[i]) {
            mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_add_device> Null pointer found in devices array");
            mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_add_device> Master: %" PRIxPTR "; Count: %" PRIxPTR, (size_t)master, (size_t)master->devices_count);
            mbs_print_stack_trace(stdout);
            return MBS_ERRC_INTERNAL;
        #endif // MODBUS_ENABLE_INTERNAL_CHECKS
        } else if (master->devices[i]->address == device->address) {
            mbs_debug_print(MBS_ERRC_GENERAL, "mbs_add_device> Device with this address already added");
            mbs_print_stack_trace(stdout);
            return MBS_ERRC_GENERAL;
        }
    }

    // Добавляем в конец массива
    if (master->devices_count >= master->devices_size) {
        mbs_error_code_t error = mbs_expand_master(master, 1);
        if (error) return error;
    }
    master->devices[master->devices_count++] = device;
    return MBS_ERRC_NONE;
}

mbs_device_tp mbs_get_device(mbs_master_tp master, uint8_t address) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_get_device> Master is null");
        mbs_print_stack_trace(stdout);
        return NULL;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    for (uint8_t i = 0; i < master->devices_count; i++)
        if (master->devices[i]->address == address)
            return master->devices[i];
    mbs_debug_print(MBS_ERRC_WRONG_ADDRESS, "mbs_get_device> Device with address %d not found", address);
    //mbs_print_stack_trace(stdout);
    return NULL;
}

void mbs_dispose_master(mbs_master_tp master) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_dispose_master> Master is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    if (master->devices) {
        if (master->devices_count > 0) {
            for (uint8_t i = 0; i < master->devices_count; ++i)
                mbs_free_device(master, master->devices[i]);
            master->devices_count = 0;
        }
        mbs_free(master->devices);
        master->devices = NULL;
        master->devices_size = 0;
    }
    if (master->endpoint) {
        master->endpoint = NULL;
    }
    master->buffer.complete = 0;
    master->buffer.length = 0;
    master->buffer.time = 0;
    mbs_clear_requests_queue(&master->queue);
}

void mbs_free_master(mbs_master_tp master) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_free_master> Master is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    mbs_dispose_master(master);
    mbs_free(master);
}

void mbs_poll_master(mbs_master_tp master, mbs_time_t time) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_poll_master> Master is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    if (!master->endpoint) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_poll_master> Endpoint is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    if (!master->devices && (master->devices_count > 0)) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_poll_master> Devices array is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    { // Poll devices
        mbs_device_tp device;
        for (uint8_t i = 0; i < master->devices_count; ++i) {
            device = master->devices[i];
            mbs_poll_device(master, device, time);
        }
    }

    mbs_buffer_tp buffer = &master->buffer;
    mbs_endpoint_tp endpoint = master->endpoint;

    // Check line
    switch (master->line) {
        case MBS_LINE_IDLE:
            if (!mbs_is_requests_queue_empty(&master->queue)) {
                master->line = MBS_LINE_PREPARE_FRAME;
                goto PREPARE_FRAME;
            } else {
                mbs_read_data(
                    endpoint,
                    #if (MODBUS_WAIT_SWITCHING > 0)
                    time,
                    #endif // (MODBUS_WAIT_SWITCHING > 0)
                    buffer->data,
                    MODBUS_BUFFER_SIZE
                );
            }
            break;
        case MBS_LINE_PREPARE_FRAME:
            PREPARE_FRAME:
            if (master->queue.first) {
                mbs_request_tp request = master->queue.first;
                mbs_error_code_t errc = mbs_bufferize_request(master, buffer, request);
                if (errc) {
                    request = mbs_pop_request(&master->queue);
                    if (!request) {
                        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_poll_master> Queue pop operation failed");
                    } else {
                        request->task->enabled = 0;
                        mbs_report_task_error(master, request->task->device, request->task, MBS_ERR_BAD_DATA);
                        mbs_reset_or_erase_task(master, request->task->device, request->task, time);
                        mbs_free_request(request);
                    }
                    master->line = MBS_LINE_IDLE;
                } else {
                    master->line = MBS_LINE_SEND_FRAME;
                    buffer->time = time;
                    goto SEND_FRAME;
                }
            } else {
                mbs_debug_print(MBS_ERRC_WARNING, "mbs_poll_master> Trying to send request with empty queue");
                master->line = MBS_LINE_IDLE;
            }
            break;
        case MBS_LINE_SEND_FRAME:
            SEND_FRAME:
            {
                mbs_error_code_t errc = MBS_ERRC_NONE;
                mbs_transmit_status_t result = mbs_transmit(buffer, endpoint, time, &errc);
                if (result == MBS_TRANSMIT_ERROR) {
                    mbs_debug_print(MBS_ERRC_TRANSMIT_FAILED, "mbs_poll_master> Request transmission failed");
                    master->line = MBS_LINE_IDLE;
                } else if (result == MBS_TRANSMIT_PENDING) {
                    if (master->buffer.time + MODBUS_WAIT_SENDING < time) {
                        mbs_debug_print(MBS_ERRC_TIMEOUT, "mbs_poll_master> Failed to send data, timeout");
                        master->line = MBS_LINE_IDLE;
                    }
                } else {
                    if (result == MBS_TRANSMIT_IDLE) {
                        mbs_debug_print(MBS_ERRC_WARNING, "mbs_poll_master> Send request with no data");
                    }
                    mbs_flush_buffer(&master->buffer);
                    master->line = MBS_LINE_WAIT_RESPONSE;
                    master->buffer.time = time;
                }
            }
            break;
        case MBS_LINE_WAIT_RESPONSE:
            {
                mbs_error_code_t errc;
                mbs_request_tp request = master->queue.first;
                if (!request) {
                    mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_poll_master> Wait response, but no request");
                    master->line = MBS_LINE_IDLE;
                    return;
                }
                mbs_size_t size = buffer->complete;
                mbs_transmit_status_t result = mbs_receive(buffer, endpoint, time, &errc);
                if (result == MBS_TRANSMIT_ERROR) {
                    // После ошибки продолжаем ждать ответ
                    mbs_flush_buffer(buffer);
                } else if (result == MBS_TRANSMIT_PENDING ||
                           result == MBS_TRANSMIT_IDLE) {
                    mbs_time_t timeout;
                    if (buffer->complete > 0) {
                        if (!size) buffer->time = time;
                        timeout = MODBUS_WAIT_RECEIVING;
                    } else timeout = MODBUS_WAIT_RESPONSE;
                    if (buffer->time + timeout < time) {
                        if (master->debug_time + 5000 < time) {
                            master->debug_time = time;
                            if (buffer->complete > 0) {
                                mbs_debug_print(MBS_ERRC_TIMEOUT, "mbs_poll_master> Receiving timeout");
                            } else {
                                mbs_debug_print(MBS_ERRC_TIMEOUT, "mbs_poll_master> Response timeout");
                            }
                        }
                        if (++request->errors >= MODBUS_MAX_REQUEST_ERRORS) {
                            request = mbs_pop_request(&master->queue);
                            if (!request) {
                                mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_poll_master> Queue pop operation failed");
                            } else {
                                request->task->enabled = 0;
                                mbs_report_task_error(master, request->task->device, request->task, MBS_ERR_TIMEOUT);
                                mbs_reset_or_erase_task(master, request->task->device, request->task, time);
                                mbs_free_request(request);
                            }
                            master->line = MBS_LINE_IDLE;
                        }
                        master->line = MBS_LINE_IDLE;
                    }
                } else {
                    mbs_frame_tp frame = master->extract(buffer, &errc);
                    mbs_flush_buffer(buffer);
                    if (frame) {
                        if (request->task->device->address != frame->slave) {
                            mbs_free_frame(frame);
                            mbs_debug_print(MBS_ERRC_WRONG_ADDRESS, "mbs_poll_master> Frame with wrong address received");
                            // Продолжаем ждать сообщение
                        } else {
                            request = mbs_pop_request(&master->queue);
                            if (request) {
                                mbs_call_on_result(master, request->task->device, request, frame);
                                mbs_reset_or_erase_task(master, request->task->device, request->task, time);
                                mbs_free_request(request);
                                if (!mbs_is_requests_queue_empty(&master->queue)) {
                                    master->line = MBS_LINE_PREPARE_FRAME;
                                } else master->line = MBS_LINE_IDLE;
                            } else {
                                mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_poll_master> Queue pop operation failed");
                                master->line = MBS_LINE_IDLE;
                            }
                            mbs_free_frame(frame);
                        }
                    } else {
                        if (++request->errors >= MODBUS_MAX_REQUEST_ERRORS) {
                            request = mbs_pop_request(&master->queue);
                            if (request) {
                                mbs_report_task_error(master, request->task->device, request->task, MBS_ERR_BAD_DATA);
                                mbs_reset_or_erase_task(master, request->task->device, request->task, time);
                                mbs_free_request(request);
                                if (!mbs_is_requests_queue_empty(&master->queue)) {
                                    master->line = MBS_LINE_PREPARE_FRAME;
                                } else master->line = MBS_LINE_IDLE;
                            } else {
                                mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_poll_master> Queue pop operation failed");
                                master->line = MBS_LINE_IDLE;
                            }
                        } else master->line = MBS_LINE_IDLE;
                    }
                }
            }
            break;
        default:
            mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_poll_master> Unrecognized line status %d", master->line);
            master->line = MBS_LINE_IDLE;
            break;
    }
}
