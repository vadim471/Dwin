#include <modbus/debug.h>
#include <modbus/device.h>
#include <modbus/master.h>
#include <modbus/symbol.h>
#include <modbus/memory.h>
#include <modbus/stack_trace.h>

void mbs_init_device(mbs_device_tp device, uint8_t address) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!device) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_init_device> Device is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    device->address = address;
    mbs_init_tasks_list(&device->tasks);
}

void mbs_free_device(mbs_master_tp master, mbs_device_tp device) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_free_device> Master is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    if (!device) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_free_device> Device is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_cancel_tasks(master, device);
    mbs_free(device);
}

mbs_error_code_t mbs_cancel_tasks(mbs_master_tp master, mbs_device_tp device) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_cancel_tasks> Master is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!device) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_cancel_tasks> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (device->tasks.first) {
        mbs_task_tp task = device->tasks.first;
        while (task) {
            mbs_task_tp next = task->next;
            if (task->enabled) {
                mbs_report_task_error(master, device, task, MBS_ERR_CANCELLED);
            }
            mbs_free_task(task);
            task = next;
        }
        device->tasks.first = NULL;
    }
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_cancel_tasks_conditional(
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_task_condition_fp condition
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t condition_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_cancel_tasks_conditional> Master is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!device) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_cancel_tasks_conditional> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (device->tasks.first) {
        mbs_task_tp task = device->tasks.first;
        mbs_task_tp* prev = &device->tasks.first;
        while (task) {
            mbs_task_tp next = task->next;
            if (condition(
                #ifdef MODBUS_EXTEND_CALLBACKS
                condition_eparam,
                #endif // MODBUS_EXTEND_CALLBACKS
                task
            )) {
                if (task->enabled) {
                    mbs_report_task_error(master, device, task, MBS_ERR_CANCELLED);
                }
                mbs_free_task(task);
                *prev = next;
            } else {
                prev = &task->next;
            }
            task = next;
        }
        device->tasks.first = NULL;
    }
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_enable_tasks(mbs_device_tp device) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!device) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_enable_tasks> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (device->tasks.first) {
        mbs_task_tp task = device->tasks.first;
        while (task) {
            if (!task->enabled)
                task->enabled = 1;
            task = task->next;
        }
    }
    return MBS_ERRC_NONE;
}

void mbs_reset_or_erase_task(mbs_master_tp master, mbs_device_tp device, mbs_task_tp task, mbs_time_t time) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_reset_or_erase_task> Master is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    if (!device) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_reset_or_erase_task> Device is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    if (!task) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_reset_or_erase_task> Task is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (task->repeat_reset) {
        mbs_error_code_t errc = task->repeat_reset(task->repeat_object, time);
        if (!errc) {
            mbs_repeat_status_t status = task->repeat_check(task->repeat_object, time, &errc);
            if (status == MBS_RS_IDLE ||
                status == MBS_RS_ERROR) {
                mbs_erase_task(&device->tasks, task);
            }
            return;
        }
    }
    mbs_erase_task(&device->tasks, task);
}

mbs_error_code_t mbs_poll_device(mbs_master_tp master, mbs_device_tp device, mbs_time_t time) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!master) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_poll_device> Master is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!device) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_poll_device> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_error_code_t error;
    mbs_repeat_status_t status;
    mbs_task_tp last = device->tasks.first;
    while (last) {
        if (!last->enabled ||
            mbs_is_request_exist(&master->queue, last)) {
            last = last->next;
            continue;
        }
        status = last->repeat_check(
            last->repeat_object, time, &error
        );
        switch (status) {
            case MBS_RS_IDLE:
                last = mbs_erase_task(&device->tasks, last);
                continue;
            case MBS_RS_PENDING:
                // Nothing need to do
                break;
            case MBS_RS_READY:
                error = mbs_push_request(&master->queue, last);
                if (error) return error;
                break;
            case MBS_RS_ERROR:
                mbs_debug_print(error, "mbs_poll_device> Repeat object returned %d", error);
                mbs_print_stack_trace(stdout);
                return error;
            default:
                mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_poll_device> Repeat object returned unknown status");
                mbs_print_stack_trace(stdout);
                return MBS_ERRC_INTERNAL;
        }
        last = last->next;
    }
    return MBS_ERRC_NONE;
}
