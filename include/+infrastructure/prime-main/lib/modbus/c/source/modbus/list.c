#include <modbus/debug.h>
#include <modbus/list.h>
#include <modbus/memory.h>
#include <modbus/callback.h>
#include <modbus/stack_trace.h>

#include <string.h>

void mbs_init_task(mbs_task_tp task) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!task) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_init_task> Task is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    memset(task, 0x0, sizeof(mbs_task_t));
}

void mbs_free_task(mbs_task_tp task) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!task) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_free_task> Task is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (task->repeat_object)
        mbs_free(task->repeat_object);
    #ifdef MODBUS_EXTEND_CALLBACKS
    mbs_free_request_eparam(task->on_request_eparam);
    mbs_free_response_eparam(task->on_response_eparam);
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_free(task);
}

void mbs_init_tasks_list(mbs_tasks_list_tp list) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!list) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_init_tasks_list> Tasks list is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    list->first = NULL;
}

mbs_error_code_t mbs_push_task(mbs_tasks_list_tp list, mbs_task_tp task) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!list) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_push_task> Tasks list is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!task) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_push_task> Task is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_task_tp last = list->first;
    if (!last) {
        list->first = task;
        return MBS_ERRC_NONE;
    }
    for (uint8_t count = 0;;) {
        if (++count < MODBUS_MAX_TASKS_COUNT) {
            if (last->next) {
                last = last->next;
            } else break;
        } else {
            mbs_debug_print(MBS_ERRC_OVERFLOW, "mbs_push_task> Tasks list overflow");
            mbs_print_stack_trace(stdout);
            return MBS_ERRC_OVERFLOW;
        }
    }
    if (last) {
        last->next = task;
        return MBS_ERRC_NONE;
    } else {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_push_task> Failed to add task to list");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
}

mbs_task_tp mbs_erase_task(mbs_tasks_list_tp list, mbs_task_tp task) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!list) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_erase_task> Tasks list is null");
        mbs_print_stack_trace(stdout);
        return NULL;
    }
    if (!task) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_erase_task> Task is null");
        mbs_print_stack_trace(stdout);
        return NULL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    // Проверяем первый элемент
    mbs_task_tp last = list->first;
    if (!last) {
        return NULL;
    } else if (last == task) {
        list->first = last->next;
        mbs_free_task(last);
        return list->first;
    }

    // Проверяем остальные элементы
    for (;;) {
        mbs_task_tp next = last->next;
        if (next) {
            if (next == task) {
                last->next = next->next;
                mbs_free_task(next);
                return last->next;
            } else last = next;
        } else break;
    }
    return NULL;
}

uint8_t mbs_tasks_list_is_empty(mbs_tasks_list_tp list) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!list) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_tasks_list_is_empty> Tasks list is null");
        mbs_print_stack_trace(stdout);
        return 1;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    return (!list->first) ? 1 : 0;
}

uint8_t mbs_tasks_list_is_empty_conditional(
    mbs_tasks_list_tp list,
    mbs_task_condition_fp condition
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t condition_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!list) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_tasks_list_is_empty_conditional> Tasks list is null");
        mbs_print_stack_trace(stdout);
        return 1;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_task_tp task = list->first;
    while (task) {
        if (condition(
            #ifdef MODBUS_EXTEND_CALLBACKS
            condition_eparam,
            #endif // MODBUS_EXTEND_CALLBACKS
            task
        )) {
            return 0;
        }
        task = task->next;
    }
    return 1;
}

void mbs_clear_tasks_list(mbs_tasks_list_tp list) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!list) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_clear_tasks_list> Tasks list is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (list->first) {
        mbs_task_tp item = list->first;
        while (item) {
            mbs_task_tp next = item->next;
            mbs_free_task(item);
            item = next;
        }
        list->first = NULL;
    }
}
