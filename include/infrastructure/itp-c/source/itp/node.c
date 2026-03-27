#include <itp/debug.h>
#include <itp/node.h>
#include <itp/protocol.h>
#include <itp/package.h>
#include <itp/symbol.h>
#include <itp/memory.h>
#include <itp/stack_trace.h>

#include <stdio.h>
#include <inttypes.h>

itp_error_code_t itp_init_node(itp_node_tp node, itp_endpoint_tp endpoint) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!node) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_init_node> Node is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    node->address = 0;
    node->endpoint = endpoint;
    node->on_error = NULL;
    #ifdef ITP_EXTEND_CALLBACKS
    node->on_error_eparam = 0;
    #endif // ITP_EXTEND_CALLBACKS
    itp_init_address_array(&node->subnodes);
    itp_init_package_queue(&node->queue_in, ITP_MAX_INPUT_PACKAGES);
    itp_init_package_queue(&node->queue_out, ITP_MAX_OUTPUT_PACKAGES);

    itp_error_code_t errc = itp_init_buffer(&node->input);
    if (!errc) errc = itp_init_buffer(&node->output);
    return errc;
}

itp_error_code_t itp_resolve_address(itp_node_tp node, uint8_t address) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!node) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_resolve_address> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_resolve_address> Address is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Check self node
    if (node->address == address) {
        return ITP_ERRC_NONE;
    }

    // Check subnodes
    if (node->subnodes.count > 0) {
        uint8_t* it = node->subnodes.data;
        uint8_t* end = it + node->subnodes.count;
        for (; it != end; ++it) {
            #ifdef ITP_ENABLE_INTERNAL_CHECKS
            if (!(*it)) {
                itp_debug_print(ITP_ERRC_INTERNAL, "itp_resolve_address> Null address found in subnodes array");
                itp_print_stack_trace(stdout);
                return ITP_ERRC_INTERNAL;
            }
            #endif // ITP_ENABLE_INTERNAL_CHECKS
            if (*it == address) {
                return ITP_ERRC_NONE;
            }
        }
    }
    return ITP_ERRC_ADDRESS_NOT_FOUND;
}

itp_error_code_t itp_poll_node(
#ifdef ITP_TRACE_BUFFERS
uint8_t root_address,
#endif // ITP_TRACE_BUFFERS
itp_node_tp node, itp_time_t time) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!node) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_poll_node> Node is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_error_code_t error = ITP_ERRC_NONE;
    itp_transmit_status_t result;

    { // Output
        if (!itp_transmit_is_ready(&node->output)) {
            if (!itp_package_queue_is_empty(&node->queue_out)) {
                error = itp_bufferize_package(&node->queue_out, &node->output, node->endpoint);
                if (error) return error;
                node->output.time = time;
            } else goto PROCESS_INPUT;
        }
        if (itp_transmit_is_ready(&node->output)) {
            result = itp_transmit(&node->output, node->endpoint, &error);
            switch (result) {
                case ITP_TRANSMIT_OK:
                    #ifdef ITP_TRACE_BUFFERS
                    {
                        char* string = (char*)itp_malloc((int)node->output.length * 3, "itp_poll_node:debug");
                        char* it = string;
                        *it = 0;
                        for (itp_size_t i = 0; i < node->output.length; i++) {
                            if (i > 0) {
                                sprintf(it, " ");
                                it += 1;
                            }
                            sprintf(it, "%02x", node->output.data[i]);
                            it += 2;
                        }
                        if (node->output.length >= 9) {
                            uint8_t status = node->output.data[6] & 0x7F;
                            const char* sym =
                                (status == ITP_SYM_ACK) ? "ACK" :
                                (status == ITP_SYM_NAK) ? "NAK" :
                                (status == ITP_SYM_CTR) ? "CTR" :
                                (status == ITP_SYM_DAT) ? "DAT" :
                                (status == ITP_SYM_REQ) ? "REQ" :
                                "ERR";
                            itp_debug_print(ITP_ERRC_TRACE, "Write (%d, %d->%d, %s, 0x%02x%02x): %s (%d)", root_address, node->output.data[2], (int)node->output.data[3], sym, node->output.data[7] & 0xFF, node->output.data[8] & 0xFF, string, node->output.length);
                        } else if (node->output.length >= 4) {
                            itp_debug_print(ITP_ERRC_TRACE, "Write (%d, %d->%d): %s (%d)", root_address, node->output.data[2], node->output.data[3], string, node->output.length);
                        } else if (node->output.length >= 3) {
                            itp_debug_print(ITP_ERRC_TRACE, "Write (%d, %d): %s (%d)", root_address, node->output.data[2], string, node->output.length);
                        } else itp_debug_print(ITP_ERRC_TRACE, "Write (%d): %s (%d)", root_address, string, node->output.length);
                        itp_free(string);
                    }
                    #endif // ITP_TRACE_BUFFERS
                    break;
                case ITP_TRANSMIT_IDLE:
                    itp_debug_print(ITP_ERRC_WARNING, "itp_poll_node> itp_transmit() returned IDLE, possible transmission error");
                    itp_print_stack_trace(stdout);
                    break;
                case ITP_TRANSMIT_PENDING:
                    if (node->output.time + ITP_WAIT_SENDING >= time) break;
                    itp_debug_print(ITP_ERRC_TIMEOUT, "itp_poll_node> Send data failed with timeout");
                    node->output.time = time;
                    //itp_dump_buffer(&node->output);
                    error = ITP_ERRC_TIMEOUT;
                case ITP_TRANSMIT_ERROR:
                    // При ошибке передачи данных пытаемся ещё раз.
                    // Никаких счётчиков ошибок нет, цикл бесконечен.
                    // Такое может быть только при обрыве связи, очередь
                    // отправки заполнится и узел начнёт пропускать пакеты.
                    if (error == ITP_ERRC_NULL_POINTER) {
                        itp_debug_print(ITP_ERRC_TRACE, "itp_poll_node> Node %" PRIuPTR " with address %d, endpoint %" PRIuPTR, (size_t)node, (int)node->address, (size_t)node->endpoint);
                        itp_print_stack_trace(stdout);
                    }
                    itp_clear_buffer(&node->output);
                    return error;
                default:
                    itp_debug_print(ITP_ERRC_INTERNAL, "itp_poll_node> Unknown transmission state");
                    itp_print_stack_trace(stdout);
                    return ITP_ERRC_INTERNAL;
            }
        }
    }

    PROCESS_INPUT:
    { // Input
        itp_size_t size = node->input.complete;
        result = itp_receive(&node->input, node->endpoint, &error);
        switch (result) {
            case ITP_TRANSMIT_OK:
                { // Перекладываем пакет в очередь приёма
                    itp_package_tp package = itp_extract_package(
                        #ifdef ITP_TRACE_BUFFERS
                        root_address,
                        #endif // ITP_TRACE_BUFFERS
                        &node->input, &error
                    );
                    if (!package) return error;
                    error = itp_push_package(&node->queue_in, package);
                    if (error) {
                        itp_free_package(package);
                        return error;
                    }
                }
                break;
            case ITP_TRANSMIT_IDLE:
            case ITP_TRANSMIT_PENDING:
                if (node->input.complete > 0) {
                    if (!size) node->input.time = time;
                    if (node->input.time + ITP_WAIT_RECEIVING < time) {
                        itp_debug_print(ITP_ERRC_TIMEOUT, "itp_poll_node> Receiving timeout");
                        itp_debug_print(ITP_ERRC_TIMEOUT, "itp_poll_node> Complete: %d; Result: %d", node->input.complete, result);
                        #ifdef ITP_TRACE_OFFSET_ERRORS
                        {
                            char data_buffer[0x50];
                            char* string = data_buffer;
                            for (itp_size_t i = 0; i < 11 && i < node->input.complete; ++i) {
                                sprintf(string, " %02x", (int)node->input.data[i]);
                                string += 3;
                            }
                            itp_debug_print(ITP_ERRC_TRACE, "itp_poll_node> Head:%s", data_buffer);
                        }
                        #endif // ITP_TRACE_OFFSET_ERRORS
                        itp_flush_buffer(&node->input);
                        return ITP_ERRC_TIMEOUT;
                    }
                }
                break;
            case ITP_TRANSMIT_ERROR:
                itp_flush_buffer(&node->input);
                break;
            default:
                itp_debug_print(ITP_ERRC_INTERNAL, "itp_poll_node> Unknown receiving state");
                itp_print_stack_trace(stdout);
                return ITP_ERRC_INTERNAL;
        }
    }

    return ITP_ERRC_NONE;
}

void itp_free_node(itp_node_tp node) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!node) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_free_node> Node is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_dispose_buffer(&node->input);
    itp_dispose_buffer(&node->output);
    itp_clear_address_array(&node->subnodes);
    itp_clear_package_queue(&node->queue_in);
    itp_clear_package_queue(&node->queue_out);
    itp_free_any_handler(
        (void*)node->on_error
        #ifdef ITP_EXTEND_CALLBACKS
        , node->on_error_eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
    itp_free(node);
}
