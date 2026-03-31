#include <usd/debug.h>
#include <usd/parameter.h>
#include <usd/stack_trace.h>

usd_error_code_t usd_init_parameter(usd_parameter_tp parameter, uint8_t key, usd_variant_type_t type) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!object) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_init_parameter> Parameter is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (type == USD_VTYPE_DEC32) {
        parameter->type = type;
        parameter->value.dec32.value = 0;
        parameter->value.dec32.exponent = 0;
    } else if (type == USD_VTYPE_DEC32F) {
        parameter->type = type;
        parameter->value.dec32f.get = NULL;
        parameter->value.dec32f.set = NULL;
        parameter->value.dec32f.object = 0;
        parameter->value.dec32f.value = 0;
        parameter->value.dec32f.exponent = 0;
        parameter->value.dec32f.eparam = 0;
    } else if (type == USD_VTYPE_DEC64) {
        parameter->type = type;
        parameter->value.dec64.value = 0;
        parameter->value.dec64.exponent = 0;
    } else if (type == USD_VTYPE_DEC64F) {
        parameter->type = type;
        parameter->value.dec64f.get = NULL;
        parameter->value.dec64f.set = NULL;
        parameter->value.dec64f.object = 0;
        parameter->value.dec64f.value = 0;
        parameter->value.dec64f.exponent = 0;
        parameter->value.dec64f.eparam = 0;
    } else if (type == USD_VTYPE_BOOL) {
        parameter->type = type;
        parameter->value.bool.value = 0;
    } else if (type == USD_VTYPE_BOOLF) {
        parameter->type = type;
        parameter->value.boolf.get = NULL;
        parameter->value.boolf.set = NULL;
        parameter->value.boolf.object = 0;
        parameter->value.dec32f.value = 0;
        parameter->value.dec32f.eparam = 0;
    } else {
        usd_debug_print(USD_ERRC_BAD_ARGUMENT, "usd_init_parameter> Unknown parameter type");
        usd_print_stack_trace(stdout);
        return USD_ERRC_BAD_ARGUMENT;
    }
    parameter->key = key;
    parameter->read_only = 1;
    parameter->error = 0;
    return USD_ERRC_NONE;
}

usd_error_code_t usd_get_parameter_value(usd_parameter_tp parameter, int32_t* value, uint8_t* exponent) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_get_parameter_value> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (!value || !exponent) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_get_parameter_value> Argument is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }

    if (parameter->type == USD_VTYPE_DEC32) {
        *value = parameter->value.dec32.value;
        *exponent = parameter->value.dec32.exponent;
    } else if (parameter->type == USD_VTYPE_DEC32F) {
        return parameter->value.dec32f.get((usd_dec32f_param_tp)(&parameter->value.dec32f), value, exponent);
    } else if (parameter->type == USD_VTYPE_BOOL) {
        if (parameter->value.bool.value) {
            *value = 1;
        } else *value = 0;
        *exponent = 0;
    } else if (parameter->type == USD_VTYPE_BOOLF) {
        uint8_t result = 0;
        usd_error_code_t errc = parameter->value.boolf.get((usd_boolf_param_tp)(&parameter->value.boolf), &result);
        if (errc) return errc;
        if (result) {
            *value = 1;
        } else *value = 0;
        *exponent = 0;
    } else {
        usd_debug_print(USD_ERRC_NOT_SUPPORTED, "usd_get_parameter_value> Unknown parameter type");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NOT_SUPPORTED;
    }
    return USD_ERRC_NONE;
}

usd_error_code_t usd_set_parameter_value(usd_parameter_tp parameter, int32_t value, uint8_t exponent) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_set_parameter_value> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (parameter->type == USD_VTYPE_DEC32) {
        parameter->value.dec32.value = value;
        parameter->value.dec32.exponent = exponent;
    } else if (parameter->type == USD_VTYPE_DEC32F) {
        if (!parameter->value.dec32f.set) return USD_ERRC_NOT_SUPPORTED;
        return parameter->value.dec32f.set((usd_dec32f_param_tp)(&parameter->value.dec32f), value, exponent);
    } else if (parameter->type == USD_VTYPE_BOOL) {
        if (value || exponent) {
            parameter->value.bool.value = 1;
        } else parameter->value.bool.value = 0;
    } else if (parameter->type == USD_VTYPE_BOOLF) {
        if (!parameter->value.boolf.set) return USD_ERRC_NOT_SUPPORTED;
        uint8_t result = (value || exponent) ? 1 : 0;
        return parameter->value.boolf.set((usd_boolf_param_tp)(&parameter->value.boolf), result);
    } else {
        usd_debug_print(USD_ERRC_NOT_SUPPORTED, "usd_set_parameter_value> Unknown parameter type");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NOT_SUPPORTED;
    }
    return USD_ERRC_NONE;
}

usd_error_code_t usd_store_parameter_value(usd_parameter_tp parameter, int32_t value, uint8_t exponent) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_store_parameter_value> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (parameter->type == USD_VTYPE_DEC32) {
        parameter->value.dec32.value = value;
        parameter->value.dec32.exponent = exponent;
    } else if (parameter->type == USD_VTYPE_DEC32F) {
        parameter->value.dec32f.value = value;
        parameter->value.dec32f.exponent = exponent;
    } else if (parameter->type == USD_VTYPE_BOOL) {
        if (value || exponent) {
            parameter->value.bool.value = 1;
        } else parameter->value.bool.value = 0;
    } else if (parameter->type == USD_VTYPE_BOOLF) {
        uint8_t result = (value || exponent) ? 1 : 0;
        parameter->value.boolf.value = result;
    } else {
        usd_debug_print(USD_ERRC_NOT_SUPPORTED, "usd_store_parameter_value> Unknown parameter type");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NOT_SUPPORTED;
    }
    return USD_ERRC_NONE;
}

usd_error_code_t usd_get_parameter_value_64(usd_parameter_tp parameter, int64_t* value, uint8_t* exponent) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_get_parameter_value_64> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (!value || !exponent) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_get_parameter_value_64> Argument is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }

    if (parameter->type == USD_VTYPE_DEC32) {
        *value = (int64_t)parameter->value.dec32.value;
        *exponent = parameter->value.dec32.exponent;
    } else if (parameter->type == USD_VTYPE_DEC32F) {
        int32_t result = 0;
        usd_error_code_t errc = parameter->value.dec32f.get((usd_dec32f_param_tp)(&parameter->value.dec32f), &result, exponent);
        *value = (int64_t)result;
        return errc;
    } else if (parameter->type == USD_VTYPE_DEC64) {
        *value = parameter->value.dec64.value;
        *exponent = parameter->value.dec64.exponent;
    } else if (parameter->type == USD_VTYPE_DEC64F) {
        return parameter->value.dec64f.get((usd_dec64f_param_tp)(&parameter->value.dec64f), value, exponent);
    } else if (parameter->type == USD_VTYPE_BOOL) {
        if (parameter->value.bool.value) {
            *value = 1;
        } else *value = 0;
        *exponent = 0;
    } else if (parameter->type == USD_VTYPE_BOOLF) {
        uint8_t result = 0;
        usd_error_code_t errc = parameter->value.boolf.get((usd_boolf_param_tp)(&parameter->value.boolf), &result);
        if (errc) return errc;
        if (result) {
            *value = 1;
        } else *value = 0;
        *exponent = 0;
    } else {
        usd_debug_print(USD_ERRC_NOT_SUPPORTED, "usd_get_parameter_value_64> Unknown parameter type");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NOT_SUPPORTED;
    }
    return USD_ERRC_NONE;
}

usd_error_code_t usd_set_parameter_value_64(usd_parameter_tp parameter, int64_t value, uint8_t exponent) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_set_parameter_value_64> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (parameter->type == USD_VTYPE_DEC32) {
        parameter->value.dec32.value = (int32_t)value;
        parameter->value.dec32.exponent = exponent;
    } else if (parameter->type == USD_VTYPE_DEC32F) {
        if (!parameter->value.dec32f.set) return USD_ERRC_NOT_SUPPORTED;
        return parameter->value.dec32f.set((usd_dec32f_param_tp)(&parameter->value.dec32f), (int32_t)value, exponent);
    } else if (parameter->type == USD_VTYPE_DEC64) {
        parameter->value.dec64.value = value;
        parameter->value.dec64.exponent = exponent;
    } else if (parameter->type == USD_VTYPE_DEC64F) {
        if (!parameter->value.dec64f.set) return USD_ERRC_NOT_SUPPORTED;
        return parameter->value.dec64f.set((usd_dec64f_param_tp)(&parameter->value.dec64f), value, exponent);
    } else if (parameter->type == USD_VTYPE_BOOL) {
        if (value || exponent) {
            parameter->value.bool.value = 1;
        } else parameter->value.bool.value = 0;
    } else if (parameter->type == USD_VTYPE_BOOLF) {
        if (!parameter->value.boolf.set) return USD_ERRC_NOT_SUPPORTED;
        uint8_t result = (value || exponent) ? 1 : 0;
        return parameter->value.boolf.set((usd_boolf_param_tp)(&parameter->value.boolf), result);
    } else {
        usd_debug_print(USD_ERRC_NOT_SUPPORTED, "usd_set_parameter_value_64> Unknown parameter type");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NOT_SUPPORTED;
    }
    return USD_ERRC_NONE;
}

usd_error_code_t usd_store_parameter_value_64(usd_parameter_tp parameter, int64_t value, uint8_t exponent) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!device) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_store_parameter_value_64> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (parameter->type == USD_VTYPE_DEC32) {
        parameter->value.dec32.value = (int32_t)value;
        parameter->value.dec32.exponent = exponent;
    } else if (parameter->type == USD_VTYPE_DEC32F) {
        parameter->value.dec32f.value = (int32_t)value;
        parameter->value.dec32f.exponent = exponent;
    } else if (parameter->type == USD_VTYPE_DEC64) {
        parameter->value.dec64.value = value;
        parameter->value.dec64.exponent = exponent;
    } else if (parameter->type == USD_VTYPE_DEC64F) {
        parameter->value.dec64f.value = value;
        parameter->value.dec64f.exponent = exponent;
    } else if (parameter->type == USD_VTYPE_BOOL) {
        if (value || exponent) {
            parameter->value.bool.value = 1;
        } else parameter->value.bool.value = 0;
    } else if (parameter->type == USD_VTYPE_BOOLF) {
        uint8_t result = (value || exponent) ? 1 : 0;
        parameter->value.boolf.value = result;
    } else {
        usd_debug_print(USD_ERRC_NOT_SUPPORTED, "usd_store_parameter_value_64> Unknown parameter type");
        usd_print_stack_trace(stdout);
        return USD_ERRC_NOT_SUPPORTED;
    }
    return USD_ERRC_NONE;
}
