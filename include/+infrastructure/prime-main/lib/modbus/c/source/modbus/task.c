#include <modbus/debug.h>
#include <modbus/task.h>
#include <modbus/device.h>
#include <modbus/repeat.h>
#include <modbus/symbol.h>
#include <modbus/memory.h>
#include <modbus/stack_trace.h>

mbs_error_code_t mbs_read_coils(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_read_coils_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!device) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_read_coils> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    if (!on_response) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_read_coils> Handler is null");
        return MBS_ERRC_NULL_POINTER;
    }

    mbs_task_tp task = (mbs_task_tp)mbs_malloc(sizeof(mbs_task_t), "mbs_read_coils:mbs_task");
    if (!task) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_read_coils> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }

    mbs_error_code_t errc = mbs_push_task(&device->tasks, task);
    if (errc) {
        mbs_free(task);
        return errc;
    }

    mbs_init_task(task);
    task->device = device;
    task->command = MBS_CMD_READ_COILS;
    task->address = address;
    task->count = count;
    task->priority = priority;
    task->on_response = (void*)on_response;
    task->repeat_check = repeat_check;
    task->repeat_reset = repeat_reset;
    task->repeat_object = repeat_object;
    task->enabled = 1;
    #ifdef MODBUS_EXTEND_CALLBACKS
    task->on_response_eparam = on_response_eparam;
    #endif // MODBUS_EXTEND_CALLBACKS
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_read_coils_once(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_master_read_coils_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_once_t* repeat = (mbs_repeat_once_t*)mbs_malloc(sizeof(mbs_repeat_once_t), "mbs_read_coils_once:mbs_repeat_once_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_read_coils_once> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_once_init(repeat);
    mbs_error_code_t errc = mbs_read_coils(
        device, address, count,
        &mbs_repeat_once_check,
        &mbs_repeat_once_reset,
        repeat, 0, on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_read_coils_loop(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_time_t time,
    mbs_master_read_coils_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_cyclic_t* repeat = (mbs_repeat_cyclic_t*)mbs_malloc(sizeof(mbs_repeat_cyclic_t), "mbs_read_coils_loop:mbs_repeat_cyclic_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_read_coils_loop> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_cyclic_init(repeat, time);
    mbs_error_code_t errc = mbs_read_coils(
        device, address, count,
        &mbs_repeat_cyclic_check,
        &mbs_repeat_cyclic_reset,
        repeat, 0, on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_read_discrete_inputs(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_read_discrete_inputs_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!device) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_read_discrete_inputs> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    if (!on_response) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_read_discrete_inputs> Handler is null");
        return MBS_ERRC_NULL_POINTER;
    }

    mbs_task_tp task = (mbs_task_tp)mbs_malloc(sizeof(mbs_task_t), "mbs_read_discrete_inputs:mbs_task");
    if (!task) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_read_discrete_inputs> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }

    mbs_error_code_t errc = mbs_push_task(&device->tasks, task);
    if (errc) {
        mbs_free(task);
        return errc;
    }

    mbs_init_task(task);
    task->device = device;
    task->command = MBS_CMD_READ_DISCRETE_INPUTS;
    task->address = address;
    task->count = count;
    task->priority = priority;
    task->on_response = (void*)on_response;
    task->repeat_check = repeat_check;
    task->repeat_reset = repeat_reset;
    task->repeat_object = repeat_object;
    task->enabled = 1;
    #ifdef MODBUS_EXTEND_CALLBACKS
    task->on_response_eparam = on_response_eparam;
    #endif // MODBUS_EXTEND_CALLBACKS
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_read_discrete_inputs_once(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_master_read_discrete_inputs_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_once_t* repeat = (mbs_repeat_once_t*)mbs_malloc(sizeof(mbs_repeat_once_t), "mbs_read_discrete_inputs_once:mbs_repeat_once_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_read_discrete_inputs_once> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_once_init(repeat);
    mbs_error_code_t errc = mbs_read_discrete_inputs(
        device, address, count,
        &mbs_repeat_once_check,
        &mbs_repeat_once_reset,
        repeat, 0, on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_read_discrete_inputs_loop(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_time_t time,
    mbs_master_read_discrete_inputs_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_cyclic_t* repeat = (mbs_repeat_cyclic_t*)mbs_malloc(sizeof(mbs_repeat_cyclic_t), "mbs_read_discrete_inputs_loop:mbs_repeat_cyclic_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_read_discrete_inputs_loop> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_cyclic_init(repeat, time);
    mbs_error_code_t errc = mbs_read_discrete_inputs(
        device, address, count,
        &mbs_repeat_cyclic_check,
        &mbs_repeat_cyclic_reset,
        repeat, 0, on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_read_holding_registers(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_read_holding_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!device) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_read_holding_registers> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    if (!on_response) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_read_holding_registers> Handler is null");
        return MBS_ERRC_NULL_POINTER;
    }

    mbs_task_tp task = (mbs_task_tp)mbs_malloc(sizeof(mbs_task_t), "mbs_read_holding_registers:mbs_task");
    if (!task) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_read_holding_registers> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }

    mbs_error_code_t errc = mbs_push_task(&device->tasks, task);
    if (errc) {
        mbs_free(task);
        return errc;
    }

    mbs_init_task(task);
    task->device = device;
    task->command = MBS_CMD_READ_HOLDING_REGISTERS;
    task->address = address;
    task->count = count;
    task->priority = priority;
    task->on_response = (void*)on_response;
    task->repeat_check = repeat_check;
    task->repeat_reset = repeat_reset;
    task->repeat_object = repeat_object;
    task->enabled = 1;
    #ifdef MODBUS_EXTEND_CALLBACKS
    task->on_response_eparam = on_response_eparam;
    #endif // MODBUS_EXTEND_CALLBACKS
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_read_holding_registers_once(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_master_read_holding_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_once_t* repeat = (mbs_repeat_once_t*)mbs_malloc(sizeof(mbs_repeat_once_t), "mbs_read_holding_registers_once:mbs_repeat_once_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_read_holding_registers_once> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_once_init(repeat);
    mbs_error_code_t errc = mbs_read_holding_registers(
        device, address, count,
        &mbs_repeat_once_check,
        &mbs_repeat_once_reset,
        repeat, 0, on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_read_holding_registers_loop(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_time_t time,
    mbs_master_read_holding_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_cyclic_t* repeat = (mbs_repeat_cyclic_t*)mbs_malloc(sizeof(mbs_repeat_cyclic_t), "mbs_read_holding_registers_loop:mbs_repeat_cyclic_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_read_holding_registers_loop> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_cyclic_init(repeat, time);
    mbs_error_code_t errc = mbs_read_holding_registers(
        device, address, count,
        &mbs_repeat_cyclic_check,
        &mbs_repeat_cyclic_reset,
        repeat, 0, on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_read_input_registers(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_read_input_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!device) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_read_input_registers> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    if (!on_response) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_read_input_registers> Handler is null");
        return MBS_ERRC_NULL_POINTER;
    }

    mbs_task_tp task = (mbs_task_tp)mbs_malloc(sizeof(mbs_task_t), "mbs_read_input_registers:mbs_task");
    if (!task) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_read_input_registers> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }

    mbs_error_code_t errc = mbs_push_task(&device->tasks, task);
    if (errc) {
        mbs_free(task);
        return errc;
    }

    mbs_init_task(task);
    task->device = device;
    task->command = MBS_CMD_READ_INPUT_REGISTERS;
    task->address = address;
    task->count = count;
    task->priority = priority;
    task->on_response = (void*)on_response;
    task->repeat_check = repeat_check;
    task->repeat_reset = repeat_reset;
    task->repeat_object = repeat_object;
    task->enabled = 1;
    #ifdef MODBUS_EXTEND_CALLBACKS
    task->on_response_eparam = on_response_eparam;
    #endif // MODBUS_EXTEND_CALLBACKS
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_read_input_registers_once(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_master_read_input_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_once_t* repeat = (mbs_repeat_once_t*)mbs_malloc(sizeof(mbs_repeat_once_t), "mbs_read_input_registers_once:mbs_repeat_once_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_read_input_registers_once> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_once_init(repeat);
    mbs_error_code_t errc = mbs_read_input_registers(
        device, address, count,
        &mbs_repeat_once_check,
        &mbs_repeat_once_reset,
        repeat, 0, on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_read_input_registers_loop(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_time_t time,
    mbs_master_read_input_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_cyclic_t* repeat = (mbs_repeat_cyclic_t*)mbs_malloc(sizeof(mbs_repeat_cyclic_t), "mbs_read_input_registers_loop:mbs_repeat_cyclic_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_read_input_registers_loop> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_cyclic_init(repeat, time);
    mbs_error_code_t errc = mbs_read_input_registers(
        device, address, count,
        &mbs_repeat_cyclic_check,
        &mbs_repeat_cyclic_reset,
        repeat, 0, on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_read_input_registers_manual(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    uint8_t* trigger,
    mbs_master_read_input_registers_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    if (!trigger) {
        mbs_debug_print(MBS_ERRC_BAD_ARGUMENT, "mbs_read_input_registers_manual> Trigger is null");
        return MBS_ERRC_BAD_ARGUMENT;
    }
    mbs_repeat_manual_t* repeat = (mbs_repeat_manual_t*)mbs_malloc(sizeof(mbs_repeat_manual_t), "mbs_read_input_registers_manual:mbs_repeat_manual_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_read_input_registers_manual> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_manual_init(repeat, trigger);
    mbs_error_code_t errc = mbs_read_input_registers(
        device, address, count,
        &mbs_repeat_manual_check,
        &mbs_repeat_manual_reset,
        repeat, 0, on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_write_single_coil(
    mbs_device_tp device,
    uint16_t address,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_write_single_coil_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!device) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_write_single_coil> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    if (!on_request) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_write_single_coil> Handler is null");
        return MBS_ERRC_NULL_POINTER;
    }

    mbs_task_tp task = (mbs_task_tp)mbs_malloc(sizeof(mbs_task_t), "mbs_write_single_coil:mbs_task");
    if (!task) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_write_single_coil> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }

    mbs_error_code_t errc = mbs_push_task(&device->tasks, task);
    if (errc) {
        mbs_free(task);
        return errc;
    }

    mbs_init_task(task);
    task->device = device;
    task->command = MBS_CMD_WRITE_SINGLE_COIL;
    task->address = address;
    task->count = 1;
    task->priority = priority;
    task->on_request = (void*)on_request;
    task->on_response = (void*)on_response;
    task->repeat_check = repeat_check;
    task->repeat_reset = repeat_reset;
    task->repeat_object = repeat_object;
    task->enabled = 1;
    #ifdef MODBUS_EXTEND_CALLBACKS
    task->on_request_eparam = on_request_eparam;
    task->on_response_eparam = on_response_eparam;
    #endif // MODBUS_EXTEND_CALLBACKS
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_write_single_coil_once(
    mbs_device_tp device,
    uint16_t address,
    mbs_master_write_single_coil_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_once_t* repeat = (mbs_repeat_once_t*)mbs_malloc(sizeof(mbs_repeat_once_t), "mbs_write_single_coil_once:mbs_repeat_once_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_write_single_coil_once> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_once_init(repeat);
    mbs_error_code_t errc = mbs_write_single_coil(
        device, address,
        &mbs_repeat_once_check,
        &mbs_repeat_once_reset,
        repeat, 0,
        on_request,
        #ifdef MODBUS_EXTEND_CALLBACKS
        on_request_eparam,
        #endif // MODBUS_EXTEND_CALLBACKS
        on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_write_single_coil_loop(
    mbs_device_tp device,
    uint16_t address,
    mbs_time_t time,
    mbs_master_write_single_coil_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_cyclic_t* repeat = (mbs_repeat_cyclic_t*)mbs_malloc(sizeof(mbs_repeat_cyclic_t), "mbs_write_single_coil_loop:mbs_repeat_cyclic_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_write_single_coil_loop> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_cyclic_init(repeat, time);
    mbs_error_code_t errc = mbs_write_single_coil(
        device, address,
        &mbs_repeat_cyclic_check,
        &mbs_repeat_cyclic_reset,
        repeat, 0,
        on_request,
        #ifdef MODBUS_EXTEND_CALLBACKS
        on_request_eparam,
        #endif // MODBUS_EXTEND_CALLBACKS
        on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_write_single_register(
    mbs_device_tp device,
    uint16_t address,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_write_single_register_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!device) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_write_single_register> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    if (!on_request) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_write_single_register> Handler is null");
        return MBS_ERRC_NULL_POINTER;
    }

    mbs_task_tp task = (mbs_task_tp)mbs_malloc(sizeof(mbs_task_t), "mbs_write_single_register:mbs_task");
    if (!task) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_write_single_register> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }

    mbs_error_code_t errc = mbs_push_task(&device->tasks, task);
    if (errc) {
        mbs_free(task);
        return errc;
    }

    mbs_init_task(task);
    task->device = device;
    task->command = MBS_CMD_WRITE_SINGLE_REGISTER;
    task->address = address;
    task->count = 1;
    task->priority = priority;
    task->on_request = (void*)on_request;
    task->on_response = (void*)on_response;
    task->repeat_check = repeat_check;
    task->repeat_reset = repeat_reset;
    task->repeat_object = repeat_object;
    task->enabled = 1;
    #ifdef MODBUS_EXTEND_CALLBACKS
    task->on_request_eparam = on_request_eparam;
    task->on_response_eparam = on_response_eparam;
    #endif // MODBUS_EXTEND_CALLBACKS
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_write_single_register_once(
    mbs_device_tp device,
    uint16_t address,
    mbs_master_write_single_register_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_once_t* repeat = (mbs_repeat_once_t*)mbs_malloc(sizeof(mbs_repeat_once_t), "mbs_write_single_register_once:mbs_repeat_once_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_write_single_register_once> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_once_init(repeat);
    mbs_error_code_t errc = mbs_write_single_register(
        device, address,
        &mbs_repeat_once_check,
        &mbs_repeat_once_reset,
        repeat, 0,
        on_request,
        #ifdef MODBUS_EXTEND_CALLBACKS
        on_request_eparam,
        #endif // MODBUS_EXTEND_CALLBACKS
        on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_write_single_register_loop(
    mbs_device_tp device,
    uint16_t address,
    mbs_time_t time,
    mbs_master_write_single_register_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_cyclic_t* repeat = (mbs_repeat_cyclic_t*)mbs_malloc(sizeof(mbs_repeat_cyclic_t), "mbs_write_single_register_loop:mbs_repeat_cyclic_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_write_single_register_loop> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_cyclic_init(repeat, time);
    mbs_error_code_t errc = mbs_write_single_register(
        device, address,
        &mbs_repeat_cyclic_check,
        &mbs_repeat_cyclic_reset,
        repeat, 0,
        on_request,
        #ifdef MODBUS_EXTEND_CALLBACKS
        on_request_eparam,
        #endif // MODBUS_EXTEND_CALLBACKS
        on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_write_single_register_manual(
    mbs_device_tp device,
    uint16_t address,
    uint8_t* trigger,
    mbs_master_write_single_register_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    if (!trigger) {
        mbs_debug_print(MBS_ERRC_BAD_ARGUMENT, "mbs_write_single_register_manual> Trigger is null");
        return MBS_ERRC_BAD_ARGUMENT;
    }
    mbs_repeat_manual_t* repeat = (mbs_repeat_manual_t*)mbs_malloc(sizeof(mbs_repeat_manual_t), "mbs_write_single_register_manual:mbs_repeat_manual_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_write_single_register_manual> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_manual_init(repeat, trigger);
    mbs_error_code_t errc = mbs_write_single_register(
        device, address,
        &mbs_repeat_manual_check,
        &mbs_repeat_manual_reset,
        repeat, 0,
        on_request,
        #ifdef MODBUS_EXTEND_CALLBACKS
        on_request_eparam,
        #endif // MODBUS_EXTEND_CALLBACKS
        on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_write_multiple_coils(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_write_multiple_coils_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!device) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_write_multiple_coils> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    if (!on_request) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_write_multiple_coils> Handler is null");
        return MBS_ERRC_NULL_POINTER;
    }

    mbs_task_tp task = (mbs_task_tp)mbs_malloc(sizeof(mbs_task_t), "mbs_write_multiple_coils:mbs_task");
    if (!task) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_write_multiple_coils> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }

    mbs_error_code_t errc = mbs_push_task(&device->tasks, task);
    if (errc) {
        mbs_free(task);
        return errc;
    }

    mbs_init_task(task);
    task->device = device;
    task->command = MBS_CMD_WRITE_MULTIPLE_COILS;
    task->address = address;
    task->count = count;
    task->priority = priority;
    task->on_request = (void*)on_request;
    task->on_response = (void*)on_response;
    task->repeat_check = repeat_check;
    task->repeat_reset = repeat_reset;
    task->repeat_object = repeat_object;
    task->enabled = 1;
    #ifdef MODBUS_EXTEND_CALLBACKS
    task->on_request_eparam = on_request_eparam;
    task->on_response_eparam = on_response_eparam;
    #endif // MODBUS_EXTEND_CALLBACKS
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_write_multiple_coils_once(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_master_write_multiple_coils_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_once_t* repeat = (mbs_repeat_once_t*)mbs_malloc(sizeof(mbs_repeat_once_t), "mbs_write_multiple_coils_once:mbs_repeat_once_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_write_multiple_coils_once> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_once_init(repeat);
    mbs_error_code_t errc = mbs_write_multiple_coils(
        device, address, count,
        &mbs_repeat_once_check,
        &mbs_repeat_once_reset,
        repeat, 0,
        on_request,
        #ifdef MODBUS_EXTEND_CALLBACKS
        on_request_eparam,
        #endif // MODBUS_EXTEND_CALLBACKS
        on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_write_multiple_coils_loop(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_time_t time,
    mbs_master_write_multiple_coils_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_cyclic_t* repeat = (mbs_repeat_cyclic_t*)mbs_malloc(sizeof(mbs_repeat_cyclic_t), "mbs_write_multiple_coils_loop:mbs_repeat_cyclic_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_write_multiple_coils_loop> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_cyclic_init(repeat, time);
    mbs_error_code_t errc = mbs_write_multiple_coils(
        device, address, count,
        &mbs_repeat_cyclic_check,
        &mbs_repeat_cyclic_reset,
        repeat, 0,
        on_request,
        #ifdef MODBUS_EXTEND_CALLBACKS
        on_request_eparam,
        #endif // MODBUS_EXTEND_CALLBACKS
        on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_write_multiple_registers(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_repeat_check_fp repeat_check,
    mbs_repeat_reset_fp repeat_reset,
    void* repeat_object,
    uint8_t priority,
    mbs_master_write_multiple_registers_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {

    #ifdef MODBUS_ENABLE_NULLPTR_CHECKS
    if (!device) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_write_multiple_registers> Device is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_NULL_POINTER;
    }
    #endif // MODBUS_ENABLE_NULLPTR_CHECKS

    if (!on_request) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "mbs_write_multiple_registers> Handler is null");
        return MBS_ERRC_NULL_POINTER;
    }

    if (count > 127) {
        mbs_debug_print(MBS_ERRC_BAD_ARGUMENT, "mbs_write_multiple_registers> Registers count must be <= 127");
        return MBS_ERRC_BAD_ARGUMENT;
    }

    mbs_task_tp task = (mbs_task_tp)mbs_malloc(sizeof(mbs_task_t), "mbs_write_multiple_registers:mbs_task");
    if (!task) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_write_multiple_registers> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }

    mbs_error_code_t errc = mbs_push_task(&device->tasks, task);
    if (errc) {
        mbs_free(task);
        return errc;
    }

    mbs_init_task(task);
    task->device = device;
    task->command = MBS_CMD_WRITE_MULTIPLE_REGISTERS;
    task->address = address;
    task->count = count;
    task->priority = priority;
    task->on_request = (void*)on_request;
    task->on_response = (void*)on_response;
    task->repeat_check = repeat_check;
    task->repeat_reset = repeat_reset;
    task->repeat_object = repeat_object;
    task->enabled = 1;
    #ifdef MODBUS_EXTEND_CALLBACKS
    task->on_request_eparam = on_request_eparam;
    task->on_response_eparam = on_response_eparam;
    #endif // MODBUS_EXTEND_CALLBACKS
    return MBS_ERRC_NONE;
}

mbs_error_code_t mbs_write_multiple_registers_once(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_master_write_multiple_registers_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_once_t* repeat = (mbs_repeat_once_t*)mbs_malloc(sizeof(mbs_repeat_once_t), "mbs_write_multiple_registers_once:mbs_repeat_once_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_write_multiple_registers_once> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_once_init(repeat);
    mbs_error_code_t errc = mbs_write_multiple_registers(
        device, address, count,
        &mbs_repeat_once_check,
        &mbs_repeat_once_reset,
        repeat, 0,
        on_request,
        #ifdef MODBUS_EXTEND_CALLBACKS
        on_request_eparam,
        #endif // MODBUS_EXTEND_CALLBACKS
        on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}

mbs_error_code_t mbs_write_multiple_registers_loop(
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    mbs_time_t time,
    mbs_master_write_multiple_registers_fp on_request,
    #ifdef MODBUS_EXTEND_CALLBACKS
    size_t on_request_eparam,
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_master_result_fp on_response
    #ifdef MODBUS_EXTEND_CALLBACKS
    , size_t on_response_eparam
    #endif // MODBUS_EXTEND_CALLBACKS
) {
    mbs_repeat_cyclic_t* repeat = (mbs_repeat_cyclic_t*)mbs_malloc(sizeof(mbs_repeat_cyclic_t), "mbs_write_multiple_registers_loop:mbs_repeat_cyclic_t");
    if (!repeat) {
        mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_write_multiple_registers_loop> Memory allocation failed");
        return MBS_ERRC_MALLOC_FAILED;
    }
    mbs_repeat_cyclic_init(repeat, time);
    mbs_error_code_t errc = mbs_write_multiple_registers(
        device, address, count,
        &mbs_repeat_cyclic_check,
        &mbs_repeat_cyclic_reset,
        repeat, 0,
        on_request,
        #ifdef MODBUS_EXTEND_CALLBACKS
        on_request_eparam,
        #endif // MODBUS_EXTEND_CALLBACKS
        on_response
        #ifdef MODBUS_EXTEND_CALLBACKS
        , on_response_eparam
        #endif // MODBUS_EXTEND_CALLBACKS
    );
    if (errc) mbs_free(repeat);
    return errc;
}
