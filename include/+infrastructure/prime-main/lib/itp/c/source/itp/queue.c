#include <itp/debug.h>
#include <itp/queue.h>
#include <itp/endpoint.h>
#include <itp/transmit.h>
#include <itp/protocol.h>
#include <itp/package.h>
#include <itp/stack_trace.h>
#include <itp/symbol.h>

#include <string.h>
#ifdef ITP_TRACE_BUFFERS
#include <stdio.h>
#endif // ITP_TRACE_BUFFERS

void itp_init_package_queue(itp_package_queue_tp queue, itp_size_t max) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_package_queue> Queue is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    queue->first = NULL;
    queue->max = max;
}

static itp_error_code_t itp_append_package(itp_package_queue_tp queue, itp_package_tp package) {
    #ifndef ITP_DISABLE_PRIORITY
    if (package->status == ITP_SYM_CTR ||
        package->status == ITP_SYM_ACK) {
        itp_package_tp last = queue->first;
        if (!last) {
            queue->first = package;
            return ITP_ERRC_NONE;
        }
        if (last->status != ITP_SYM_CTR &&
            last->status != ITP_SYM_ACK) {
            // Emplace
            queue->first = package;
            package->next = last;
            return ITP_ERRC_NONE;
        }
        while (last) {
            if (!last->next ||
                (last->next->status != ITP_SYM_CTR &&
                 last->next->status != ITP_SYM_ACK)) {
                // Emplace
                package->next = last->next;
                last->next = package;
                return ITP_ERRC_NONE;
            } else {
                last = last->next;
            }
        }
    } else
    #endif // ITP_DISABLE_PRIORITY
    {
        itp_package_tp last = queue->first;
        if (!last) {
            queue->first = package;
            return ITP_ERRC_NONE;
        }
        while (last) {
            if (!last->next) {
                last->next = package;
                return ITP_ERRC_NONE;
            } else last = last->next;
        }
    }
    return ITP_ERRC_OVERFLOW;
}

#ifndef ITP_DISABLE_PRIORITY
static itp_error_code_t itp_replace_package(itp_package_queue_tp queue, itp_package_tp package) {
    if (package->status == ITP_SYM_CTR ||
        package->status == ITP_SYM_ACK) {
        itp_package_tp previous = queue->first;
        if (!previous) {
            queue->first = package;
            return ITP_ERRC_NONE;
        }
        if (previous->status != ITP_SYM_CTR &&
            previous->status != ITP_SYM_ACK) {
            // Replace
            queue->first = package;
            package->next = previous->next;
            itp_free_package(previous);
            return ITP_ERRC_NONE;
        }
        itp_package_tp next = previous->next;
        while (next) {
            if (next->status != ITP_SYM_CTR &&
                next->status != ITP_SYM_ACK) {
                // Replace
                previous->next = package;
                package->next = next->next;
                itp_free_package(next);
                return ITP_ERRC_NONE;
            } else {
                previous = next;
                next = next->next;
            }
        }
    } else {
        itp_package_tp previous = queue->first;
        if (!previous) {
            queue->first = package;
            return ITP_ERRC_NONE;
        }
        if (previous->status != ITP_SYM_CTR &&
            previous->status != ITP_SYM_ACK) {
            // Replace
            queue->first = previous->next;
            itp_free_package(previous);
            itp_package_tp next = queue->first->next;
            if (next) {
                while (next->next)
                    next = next->next;
                next->next = package;
            } else queue->first->next = package;
            return ITP_ERRC_NONE;
        }
        itp_package_tp next = previous->next;
        while (next) {
            if (next->status != ITP_SYM_CTR &&
                next->status != ITP_SYM_ACK) {
                // Replace
                previous->next = next->next;
                itp_free_package(next);
                next = previous->next;
                if (next) {
                    while (next->next)
                        next = next->next;
                    next->next = package;
                } else previous->next = package;
                return ITP_ERRC_NONE;
            } else {
                previous = next;
                next = next->next;
            }
        }
    }
    return ITP_ERRC_OVERFLOW;
}
#endif // ITP_DISABLE_PRIORITY

itp_error_code_t itp_push_package(itp_package_queue_tp queue, itp_package_tp package) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_package> Queue is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!package) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_package> Package is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (itp_package_queue_size(queue) < queue->max) {
        return itp_append_package(queue, package);
    } else {
        #ifndef ITP_DISABLE_PRIORITY
        return itp_replace_package(queue, package);
        #else // ITP_DISABLE_PRIORITY
        return ITP_ERRC_OVERFLOW;
        #endif // ITP_DISABLE_PRIORITY
    }
}

itp_package_tp itp_pop_package(itp_package_queue_tp queue) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_pop_package> Queue is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (queue->first) {
        itp_package_tp first = queue->first;
        if (first->next) {
            queue->first = first->next;
            first->next = NULL;
        } else {
            queue->first = NULL;
        }
        return first;
    } else return NULL;
}

itp_error_code_t itp_bufferize_package(itp_package_queue_tp queue, itp_buffer_tp buffer, itp_endpoint_tp endpoint) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_bufferize_package> Queue is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_bufferize_package> Buffer is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!endpoint) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_bufferize_package> Endpoint is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_clear_buffer(buffer);
    if (!queue->first)
        return ITP_ERRC_NONE;

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!queue->first->data || !queue->first->size) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_bufferize_package> Package has no data");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_package_tp package = queue->first;
    itp_size_t package_size = package->size;
    if (!package->use_checksum &&
        endpoint->use_checksum) {
        package_size += 2;
    }
    if (buffer->size < package_size) {
        itp_error_code_t errc = itp_resize_buffer(buffer, package_size, 0);
        if (errc) return errc;
    }

    package = itp_pop_package(queue);
    if (package) {
        memcpy(buffer->data, package->data, package->size);
        buffer->length = package->size;
        // Дописываем или убираем из пакета контрольную сумму
        if (package->use_checksum ^ endpoint->use_checksum) {
            if (!package->use_checksum) {
                itp_append_checksum(buffer);
            } else itp_remove_checksum(buffer);
        }
        itp_free_package(package);
    }

    /*#ifdef ITP_TRACE_BUFFERS
    printf("Was write (%d->%d): ", buffer->data[2], buffer->data[3]);
    for (itp_size_t i = 0; i < buffer->length; i++) {
        if (i > 0) printf(", ");
        printf("0x%02x", (uint32_t)buffer->data[i]);
    }
    printf(" (%d) \r\n", buffer->length);
    #endif // ITP_TRACE_BUFFERS*/
    return ITP_ERRC_NONE;
}

uint8_t itp_package_queue_size(itp_package_queue_tp queue) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_package_queue_size> Queue is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    uint8_t count = 0;
    itp_package_tp last = queue->first;
    if (!last) return count;
    while (last) {
        count++;
        if (last->next) {
            last = last->next;
        } else break;
    }
    return count;
}

uint8_t itp_package_queue_is_empty(itp_package_queue_tp queue) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_package_queue_is_empty> Queue is null");
        itp_print_stack_trace(stdout);
        return 1;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    return (!queue->first) ? 1 : 0;
}

void itp_clear_package_queue(itp_package_queue_tp queue) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_clear_package_queue> Queue is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    while (!itp_package_queue_is_empty(queue)) {
        itp_package_tp package = itp_pop_package(queue);
        itp_free_package(package);
    }
}
