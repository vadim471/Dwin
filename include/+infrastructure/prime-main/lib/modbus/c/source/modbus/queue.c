#include <modbus/debug.h>
#include <modbus/queue.h>
#include <modbus/list.h>
#include <modbus/memory.h>
#include <modbus/stack_trace.h>

#include <string.h>

void mbs_init_request(mbs_request_tp request) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!request) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_init_request> Request is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    memset(request, 0x0, sizeof(mbs_request_t));
}

mbs_request_tp mbs_create_request(mbs_task_tp task) {
    mbs_request_tp object = (mbs_request_tp)mbs_malloc(sizeof(mbs_request_t), "mbs_create_request:mbs_request");
    if (!object) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_create_request> Memory allocation failed");
        return NULL;
    }
    mbs_init_request(object);
    object->task = task;
    return object;
}

void mbs_free_request(mbs_request_tp request) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!request) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_free_request> Request is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_free(request);
}

void mbs_init_requests_queue(mbs_requests_queue_tp queue) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_init_requests_queue> Requests queue is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    queue->first = NULL;
}

mbs_error_code_t mbs_push_request(mbs_requests_queue_tp queue, mbs_task_tp task) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_push_request> Requests queue is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!task) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_push_request> Task is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    // Skip first request if in progress
    mbs_request_tp last = queue->first;
    if (last) {
        if (mbs_is_requests_queue_overflow(queue)) {
            return MBS_ERRC_OVERFLOW;
        }
        if (last->in_progress) {
            last = last->next;
        }
    }

    // Search task by prioriy
    while (last) {
        if (last->task->priority >= task->priority) {
            last = last->next;
        } else break;
    }

    // Insert before first task
    if (queue->first == last) {
        mbs_request_tp request = mbs_create_request(task);
        if (!request) return MBS_ERRC_MALLOC_FAILED;
        queue->first = request;
        request->next = last;
        return MBS_ERRC_NONE;
    }

    // Insert before target task
    mbs_request_tp current = queue->first;
    while (current) {
        if (current->next == last) {
            mbs_request_tp request = mbs_create_request(task);
            if (!request) return MBS_ERRC_MALLOC_FAILED;
            current->next = request;
            request->next = last;
            return MBS_ERRC_NONE;
        } else current = current->next;
    }

    // Unreachable code
    mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_push_request> Task not inserted");
    return MBS_ERRC_INTERNAL;
}

mbs_request_tp mbs_pop_request(mbs_requests_queue_tp queue) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_pop_request> Requests queue is null");
        mbs_print_stack_trace(stdout);
        return NULL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (queue->first) {
        mbs_request_tp first = queue->first;
        queue->first = first->next;
        return first;
    }
    return NULL;
}

mbs_request_tp mbs_erase_request(mbs_requests_queue_tp queue, mbs_task_tp task) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_erase_request> Requests queue is null");
        mbs_print_stack_trace(stdout);
        return NULL;
    }
    if (!task) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_erase_request> Task is null");
        mbs_print_stack_trace(stdout);
        return NULL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    // Проверяем первый элемент
    mbs_request_tp last = queue->first;
    if (!last) {
        return NULL;
    } else if (last->task == task) {
        queue->first = last->next;
        mbs_free_request(last);
        return queue->first;
    }

    // Проверяем остальные элементы
    for (;;) {
        mbs_request_tp next = last->next;
        if (next) {
            if (next->task == task) {
                last->next = next->next;
                mbs_free_request(next);
                return last->next;
            } else last = next;
        } else break;
    }
    return NULL;
}

mbs_size_t mbs_requests_queue_size(mbs_requests_queue_tp queue) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_requests_queue_size> Requests queue is null");
        mbs_print_stack_trace(stdout);
        return 0;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_size_t size = 0;
    mbs_request_tp next = queue->first;
    while (next) {
        size++;
        next = next->next;
    }
    return size;
}

uint8_t mbs_is_request_exist(mbs_requests_queue_tp queue, mbs_task_tp task) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_is_request_exist> Requests queue is null");
        mbs_print_stack_trace(stdout);
        return 0;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_request_tp next = queue->first;
    while (next) {
        if (next->task == task) {
            return 1;
        } else next = next->next;
    }
    return 0;
}

uint8_t mbs_is_requests_queue_overflow(mbs_requests_queue_tp queue) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_is_requests_queue_overflow> Requests queue is null");
        mbs_print_stack_trace(stdout);
        return 1;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    return (mbs_requests_queue_size(queue) >= MODBUS_MAX_QUEUE_SIZE) ? 1 : 0;
}

uint8_t mbs_is_requests_queue_empty(mbs_requests_queue_tp queue) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_is_requests_queue_empty> Requests queue is null");
        mbs_print_stack_trace(stdout);
        return 1;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    return (!queue->first) ? 1 : 0;
}

void mbs_clear_requests_queue(mbs_requests_queue_tp queue) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!queue) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_clear_requests_queue> Requests queue is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (queue->first) {
        mbs_request_tp item = queue->first;
        while (item) {
            mbs_request_tp next = item->next;
            mbs_free_request(item);
            item = next;
        }
        queue->first = NULL;
    }
}
