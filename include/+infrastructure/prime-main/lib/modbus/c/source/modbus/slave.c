#include <modbus/debug.h>
#include <modbus/stack_trace.h>
#include <modbus/slave.h>
#include <modbus/memory.h>
#include <modbus/protocol.h>
#include <modbus/frame.h>

#include <modbus/rtu.h>

//#include <string.h>
//#include <inttypes.h>

mbs_error_code_t mbs_init_slave_rtu(mbs_slave_tp slave, uint8_t id, mbs_endpoint_tp endpoint) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!slave) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_init_slave_rtu> Slave is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    slave->id = id;
    slave->line = MBS_LINE_IDLE;
    slave->endpoint = endpoint;
    slave->extract = &mbs_extract_rtu_frame;
    slave->serialize = &mbs_serialize_rtu_frame;
    mbs_init_buffer(&slave->buffer, &mbs_get_pdu_length_slave, &mbs_check_rtu_frame);
    mbs_init_ranges_list(&slave->ranges);
    return MBS_ERRC_NONE;
}

void mbs_dispose_slave(mbs_slave_tp slave) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!slave) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_dispose_slave> Slave is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    if (slave->endpoint) {
        slave->endpoint = NULL;
    }
    slave->buffer.complete = 0;
    slave->buffer.length = 0;
    slave->buffer.time = 0;
    mbs_clear_ranges_list(&slave->ranges);
}

void mbs_free_slave(mbs_slave_tp slave) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!slave) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_free_slave> Slave is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    mbs_dispose_slave(slave);
    mbs_free(slave);
}

static mbs_error_code_t mbs_serialize_error(mbs_slave_tp slave, mbs_byte_t command, mbs_byte_t error) {
    mbs_frame_t frame;
    mbs_init_frame(&frame);
    frame.slave = slave->id;
    frame.command = command;
    frame.error = error;
    return slave->serialize(&slave->buffer, &frame);
}

void mbs_poll_slave(mbs_slave_tp slave, mbs_time_t time) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!slave) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_poll_slave> Slave is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    if (!slave->endpoint) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_poll_slave> Endpoint is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    mbs_buffer_tp buffer = &slave->buffer;
    mbs_endpoint_tp endpoint = slave->endpoint;

    // Check line
    switch (slave->line) {
        case MBS_LINE_IDLE:
            {
                mbs_error_code_t errc;
                mbs_size_t size = buffer->complete;
                mbs_transmit_status_t result = mbs_receive(buffer, endpoint, time, &errc);
                if (result == MBS_TRANSMIT_ERROR) {
                    mbs_flush_buffer(buffer);
                } else if (result == MBS_TRANSMIT_PENDING ||
                           result == MBS_TRANSMIT_IDLE) {
                    if (buffer->complete > 0) {
                        if (!size) buffer->time = time;
                        if (buffer->time + MODBUS_WAIT_RECEIVING < time) {
                            mbs_debug_print(MBS_ERRC_TIMEOUT, "mbs_poll_slave> Receiving timeout");
                            mbs_flush_buffer(buffer);
                        }
                    }
                } else {
                    mbs_frame_tp frame = slave->extract(buffer, &errc);
                    mbs_flush_buffer(buffer);
                    if (frame) {
                        uint8_t command = frame->command;
                        if (command == MBS_CMD_WRITE_SINGLE_COIL) {
                            command = MBS_CMD_WRITE_MULTIPLE_COILS;
                        } else if (command == MBS_CMD_WRITE_SINGLE_REGISTER) {
                            command = MBS_CMD_WRITE_MULTIPLE_REGISTERS;
                        }
                        mbs_range_tp range = mbs_find_range(&slave->ranges, command);
                        if (range) {
                            mbs_frame_t response;
                            mbs_init_frame(&response);
                            response.slave = slave->id;
                            response.command = frame->command;
                            errc = mbs_call_on_request(slave, range, frame, &response);
                            if (errc) {
                                mbs_debug_print(errc, "mbs_poll_slave> Request failed, device failure will be sent");
                                response.error = MBS_ERR_SLAVE_DEVICE_FAILURE;
                            }
                            errc = slave->serialize(buffer, &response);
                            mbs_dispose_frame(&response);
                            if (errc) {
                                mbs_debug_print(errc, "mbs_poll_slave> Failed to serialize response");
                            } else {
                                slave->line = MBS_LINE_SEND_FRAME;
                            }
                        } else {
                            errc = mbs_serialize_error(slave, frame->command, MBS_ERR_ILLEGAL_FUNCTION);
                            if (errc) {
                                mbs_debug_print(errc, "mbs_poll_slave> Failed to serialize modbus error");
                            } else {
                                slave->line = MBS_LINE_SEND_FRAME;
                            }
                        }
                        mbs_free_frame(frame);
                    } else {
                        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_poll_slave> Failed to extract frame");
                        mbs_flush_buffer(buffer);
                    }
                }
            }
            break;
        case MBS_LINE_SEND_FRAME:
            {
                mbs_error_code_t errc = MBS_ERRC_NONE;
                mbs_transmit_status_t result = mbs_transmit(buffer, endpoint, time, &errc);
                if (result == MBS_TRANSMIT_ERROR) {
                    mbs_debug_print(MBS_ERRC_TRANSMIT_FAILED, "mbs_poll_slave> Request transmission failed");
                    slave->line = MBS_LINE_IDLE;
                } else if (result == MBS_TRANSMIT_PENDING) {
                    if (slave->buffer.time + MODBUS_WAIT_SENDING < time) {
                        mbs_debug_print(MBS_ERRC_TIMEOUT, "mbs_poll_slave> Failed to send data, timeout");
                        slave->line = MBS_LINE_IDLE;
                    }
                } else {
                    if (result == MBS_TRANSMIT_IDLE) {
                        mbs_debug_print(MBS_ERRC_WARNING, "mbs_poll_slave> Send request with no data");
                    }
                    mbs_flush_buffer(&slave->buffer);
                    slave->line = MBS_LINE_IDLE;
                    slave->buffer.time = time;
                }
            }
            break;
        default:
            mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_poll_slave> Unrecognized line status %d", slave->line);
            slave->line = MBS_LINE_IDLE;
            break;
    }
}
