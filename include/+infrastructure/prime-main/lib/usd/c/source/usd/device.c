#include <usd/debug.h>
#include <usd/device.h>
#include <usd/parameter.h>
#include <usd/memory.h>
#include <usd/stack_trace.h>

#include <stddef.h>
#include <string.h>

void usd_init_device(usd_device_tp object, uint16_t id) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!object) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_init_device> Object is null");
        usd_print_stack_trace(stdout);
        return;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    object->id = id;
    object->data = NULL;
    object->size = 0;
    object->count = 0;
}

usd_error_code_t usd_expand_device(usd_device_tp object, uint8_t size) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!object) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_expand_device> Object is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (object->data) {
        size_t new_size = object->size + size;
        if (new_size > 0xFF) {
            usd_debug_print(USD_ERRC_OUT_OF_BOUNDS, "usd_expand_device> Data array out of bounds");
            usd_print_stack_trace(stdout);
            return USD_ERRC_OUT_OF_BOUNDS;
        }
        if (object->size > 0) {
            usd_parameter_tp* data = object->data;
            object->data = usd_malloc((uint32_t)new_size * sizeof(usd_parameter_tp), "usd_expand_device:data:1");
            if (!object->data) {
                usd_debug_print(USD_ERRC_MALLOC_FAILED, "usd_expand_device> Failed to allocate memory for nodes object");
                usd_print_stack_trace(stdout);
                return USD_ERRC_MALLOC_FAILED;
            }
            memcpy(object->data, data, object->size * sizeof(usd_parameter_tp));
            memset(object->data + object->size, 0, size * sizeof(usd_parameter_tp));
            object->size = new_size;
            usd_free(data);
        } else {
            usd_free(object->data);
            object->data = usd_malloc((uint32_t)new_size * sizeof(usd_parameter_tp), "usd_expand_device:data:2");
            if (!object->data) {
                usd_debug_print(USD_ERRC_MALLOC_FAILED, "usd_expand_device> Failed to allocate memory for nodes object");
                usd_print_stack_trace(stdout);
                return USD_ERRC_MALLOC_FAILED;
            }
        }
    } else {
        object->data = usd_malloc((uint32_t)size * sizeof(usd_parameter_tp), "usd_expand_device:data:3");
        if (!object->data) {
            usd_debug_print(USD_ERRC_MALLOC_FAILED, "usd_expand_device> Failed to allocate memory for nodes object");
            usd_print_stack_trace(stdout);
            return USD_ERRC_MALLOC_FAILED;
        }
        object->size = size;
        if (object->count > 0) {
            object->count = 0;
        }
    }
    return USD_ERRC_NONE;
}

usd_error_code_t usd_add_parameter(usd_device_tp object, usd_parameter_tp parameter) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!object) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_add_parameter> Object is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    if (!parameter) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_add_parameter> Parameter is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    // Проверяем, нет ли уже узла с таким адресом в массиве
    for (uint8_t i = 0; i < object->count; ++i) {
        if (object->data[i] == parameter) {
            usd_debug_print(USD_ERRC_GENERAL, "usd_add_parameter> This parameter already added");
            usd_print_stack_trace(stdout);
            return USD_ERRC_GENERAL;
        #ifdef USD_ENABLE_NULLPTR_CHECKS
        } else if (!object->data[i]) {
            usd_debug_print(USD_ERRC_INTERNAL, "usd_add_parameter> Null pointer found in parameters object");
            usd_print_stack_trace(stdout);
            return USD_ERRC_INTERNAL;
        #endif // USD_ENABLE_NULLPTR_CHECKS
        } else if (object->data[i]->key == parameter->key) {
            usd_debug_print(USD_ERRC_GENERAL, "usd_add_parameter> Parameter with this key already added");
            usd_print_stack_trace(stdout);
            return USD_ERRC_GENERAL;
        }
    }

    // Добавляем в конец массива
    if (object->count >= object->size) {
        usd_error_code_t error = usd_expand_device(object, 1);
        if (error) return error;
    }
    object->data[object->count++] = parameter;
    return USD_ERRC_NONE;
}

void usd_clear_device(usd_device_tp object) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!object) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_clear_device> Object is null");
        usd_print_stack_trace(stdout);
        return;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (object->data) {
        usd_free(object->data);
        object->data = NULL;
    }
    object->count = 0;
    object->size = 0;
}

void usd_dispose_device(usd_device_tp object) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!object) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_dispose_device> Object is null");
        usd_print_stack_trace(stdout);
        return;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (object->data) {
        for (size_t i = 0; i < object->count; ++i)
            usd_free(object->data[i]);
        usd_free(object->data);
    }
}

void usd_free_device(usd_device_tp object) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!object) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_free_device> Object is null");
        usd_print_stack_trace(stdout);
        return;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    usd_dispose_device(object);
    usd_free(object);
}

usd_error_code_t usd_create_dec32_parameter(usd_device_tp object, uint8_t key, uint8_t read_only) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!object) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_create_dec32_parameter> Object is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (usd_is_has_parameter(object, key)) {
        usd_debug_print(USD_ERRC_DUPLICATE_KEY, "usd_create_dec32_parameter> Parameter %d already exists", key);
        return USD_ERRC_DUPLICATE_KEY;
    }

    usd_parameter_tp parameter = (usd_parameter_tp)usd_malloc(sizeof(usd_parameter_t), "usd_create_dec32_parameter:usd_parameter");
    if (!parameter) {
        usd_debug_print(USD_ERRC_MALLOC_FAILED, "usd_create_dec32_parameter> Memory allocation failed");
        return USD_ERRC_MALLOC_FAILED;
    }

    usd_error_code_t errc = usd_init_parameter(parameter, key, USD_VTYPE_DEC32);
    if (errc) {
        usd_free(parameter);
        return errc;
    }
    parameter->read_only = read_only;

    errc = usd_add_parameter(object, parameter);
    if (errc) {
        usd_free(parameter);
        return errc;
    }
    return USD_ERRC_NONE;
}

usd_error_code_t usd_bind_dec32_parameter(usd_device_tp device, uint8_t key, void* object, size_t eparam, usd_get_dec32_parameter_fp read, usd_set_dec32_parameter_fp write, uint8_t read_only) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_bind_dec32_parameter> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (usd_is_has_parameter(device, key)) {
        usd_debug_print(USD_ERRC_DUPLICATE_KEY, "usd_bind_dec32_parameter> Parameter %d already exists", key);
        return USD_ERRC_DUPLICATE_KEY;
    }

    usd_parameter_tp parameter = (usd_parameter_tp)usd_malloc(sizeof(usd_parameter_t), "usd_bind_dec32_parameter:usd_parameter");
    if (!parameter) {
        usd_debug_print(USD_ERRC_MALLOC_FAILED, "usd_bind_dec32_parameter> Memory allocation failed");
        return USD_ERRC_MALLOC_FAILED;
    }

    usd_error_code_t errc = usd_init_parameter(parameter, key, USD_VTYPE_DEC32F);
    if (errc) {
        usd_free(parameter);
        return errc;
    }
    parameter->read_only = read_only;
    parameter->value.dec32f.get = read;
    parameter->value.dec32f.set = write;
    parameter->value.dec32f.object = object;
    parameter->value.dec32f.eparam = eparam;

    errc = usd_add_parameter(device, parameter);
    if (errc) {
        usd_free(parameter);
        return errc;
    }
    return USD_ERRC_NONE;
}

usd_error_code_t usd_create_dec64_parameter(usd_device_tp object, uint8_t key, uint8_t read_only) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!object) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_create_dec64_parameter> Object is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (usd_is_has_parameter(object, key)) {
        usd_debug_print(USD_ERRC_DUPLICATE_KEY, "usd_create_dec64_parameter> Parameter %d already exists", key);
        return USD_ERRC_DUPLICATE_KEY;
    }

    usd_parameter_tp parameter = (usd_parameter_tp)usd_malloc(sizeof(usd_parameter_t), "usd_create_dec64_parameter:usd_parameter");
    if (!parameter) {
        usd_debug_print(USD_ERRC_MALLOC_FAILED, "usd_create_dec64_parameter> Memory allocation failed");
        return USD_ERRC_MALLOC_FAILED;
    }

    usd_error_code_t errc = usd_init_parameter(parameter, key, USD_VTYPE_DEC64);
    if (errc) {
        usd_free(parameter);
        return errc;
    }
    parameter->read_only = read_only;

    errc = usd_add_parameter(object, parameter);
    if (errc) {
        usd_free(parameter);
        return errc;
    }
    return USD_ERRC_NONE;
}

usd_error_code_t usd_bind_dec64_parameter(usd_device_tp device, uint8_t key, void* object, size_t eparam, usd_get_dec64_parameter_fp read, usd_set_dec64_parameter_fp write, uint8_t read_only) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_bind_dec32_parameter> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (usd_is_has_parameter(device, key)) {
        usd_debug_print(USD_ERRC_DUPLICATE_KEY, "usd_bind_dec32_parameter> Parameter %d already exists", key);
        return USD_ERRC_DUPLICATE_KEY;
    }

    usd_parameter_tp parameter = (usd_parameter_tp)usd_malloc(sizeof(usd_parameter_t), "usd_bind_dec32_parameter:usd_parameter");
    if (!parameter) {
        usd_debug_print(USD_ERRC_MALLOC_FAILED, "usd_bind_dec32_parameter> Memory allocation failed");
        return USD_ERRC_MALLOC_FAILED;
    }

    usd_error_code_t errc = usd_init_parameter(parameter, key, USD_VTYPE_DEC64F);
    if (errc) {
        usd_free(parameter);
        return errc;
    }
    parameter->read_only = read_only;
    parameter->value.dec64f.get = read;
    parameter->value.dec64f.set = write;
    parameter->value.dec64f.object = object;
    parameter->value.dec64f.eparam = eparam;

    errc = usd_add_parameter(device, parameter);
    if (errc) {
        usd_free(parameter);
        return errc;
    }
    return USD_ERRC_NONE;
}

usd_error_code_t usd_create_bool_parameter(usd_device_tp object, uint8_t key, uint8_t read_only) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!object) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_create_bool_parameter> Object is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (usd_is_has_parameter(object, key)) {
        usd_debug_print(USD_ERRC_DUPLICATE_KEY, "usd_create_bool_parameter> Parameter %d already exists", key);
        return USD_ERRC_DUPLICATE_KEY;
    }

    usd_parameter_tp parameter = (usd_parameter_tp)usd_malloc(sizeof(usd_parameter_t), "usd_create_bool_parameter:usd_parameter");
    if (!parameter) {
        usd_debug_print(USD_ERRC_MALLOC_FAILED, "usd_create_bool_parameter> Memory allocation failed");
        return USD_ERRC_MALLOC_FAILED;
    }

    usd_error_code_t errc = usd_init_parameter(parameter, key, USD_VTYPE_BOOL);
    if (errc) {
        usd_free(parameter);
        return errc;
    }
    parameter->read_only = read_only;

    errc = usd_add_parameter(object, parameter);
    if (errc) {
        usd_free(parameter);
        return errc;
    }
    return USD_ERRC_NONE;
}

usd_error_code_t usd_bind_bool_parameter(usd_device_tp device, uint8_t key, void* object, size_t eparam, usd_get_bool_parameter_fp read, usd_set_bool_parameter_fp write, uint8_t read_only) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_bind_bool_parameter> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (usd_is_has_parameter(device, key)) {
        usd_debug_print(USD_ERRC_DUPLICATE_KEY, "usd_bind_bool_parameter> Parameter %d already exists", key);
        return USD_ERRC_DUPLICATE_KEY;
    }

    usd_parameter_tp parameter = (usd_parameter_tp)usd_malloc(sizeof(usd_parameter_t), "usd_bind_bool_parameter:usd_parameter");
    if (!parameter) {
        usd_debug_print(USD_ERRC_MALLOC_FAILED, "usd_bind_bool_parameter> Memory allocation failed");
        return USD_ERRC_MALLOC_FAILED;
    }

    usd_error_code_t errc = usd_init_parameter(parameter, key, USD_VTYPE_BOOLF);
    if (errc) {
        usd_free(parameter);
        return errc;
    }
    parameter->read_only = read_only;
    parameter->value.boolf.get = read;
    parameter->value.boolf.set = write;
    parameter->value.boolf.object = object;
    parameter->value.boolf.eparam = eparam;

    errc = usd_add_parameter(device, parameter);
    if (errc) {
        usd_free(parameter);
        return errc;
    }
    return USD_ERRC_NONE;
}

uint8_t usd_is_has_parameter(usd_device_tp device, uint8_t key) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_is_has_parameter> Device is null");
        usd_print_stack_trace(stdout);
        return 0;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    for (size_t i = 0; i < device->count; ++i) {
        if (device->data[i]->key == key) {
            return 1U;
        }
    }
    return 0U;
}

usd_parameter_tp usd_find_parameter_by_key(usd_device_tp device, uint8_t key) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_find_parameter_by_key> Device is null");
        usd_print_stack_trace(stdout);
        return NULL;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    for (size_t i = 0; i < device->count; ++i) {
        if (device->data[i]->key == key) {
            return device->data[i];
        }
    }
    return NULL;
}

usd_error_code_t usd_get_parameter(usd_device_tp device, uint8_t key, int32_t* value, uint8_t* exponent) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_get_parameter> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (!value || !exponent) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_get_parameter> Argument is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }

    for (size_t i = 0; i < device->count; ++i) {
        if (device->data[i]->key == key) {
            return usd_get_parameter_value(device->data[i], value, exponent);
        }
    }
    usd_debug_print(USD_ERRC_NOT_FOUND, "usd_get_parameter> Parameter %d not found", key);
    return USD_ERRC_NOT_FOUND;
}

usd_error_code_t usd_get_parameter_64(usd_device_tp device, uint8_t key, int64_t* value, uint8_t* exponent) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_get_parameter_64> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (!value || !exponent) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_get_parameter_64> Argument is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }

    for (size_t i = 0; i < device->count; ++i) {
        if (device->data[i]->key == key) {
            return usd_get_parameter_value_64(device->data[i], value, exponent);
        }
    }
    usd_debug_print(USD_ERRC_NOT_FOUND, "usd_get_parameter_64> Parameter %d not found", key);
    return USD_ERRC_NOT_FOUND;
}

usd_error_code_t usd_set_parameter(usd_device_tp device, uint8_t key, int32_t value, uint8_t exponent) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_set_parameter> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    for (size_t i = 0; i < device->count; ++i) {
        if (device->data[i]->key == key) {
            return usd_set_parameter_value(device->data[i], value, exponent);
        }
    }
    usd_debug_print(USD_ERRC_NOT_FOUND, "usd_set_parameter> Parameter %d not found", key);
    return USD_ERRC_NOT_FOUND;
}

usd_error_code_t usd_set_parameter_64(usd_device_tp device, uint8_t key, int64_t value, uint8_t exponent) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_set_parameter_64> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    for (size_t i = 0; i < device->count; ++i) {
        if (device->data[i]->key == key) {
            return usd_set_parameter_value_64(device->data[i], value, exponent);
        }
    }
    usd_debug_print(USD_ERRC_NOT_FOUND, "usd_set_parameter_64> Parameter %d not found", key);
    return USD_ERRC_NOT_FOUND;
}
