#include <itp/debug.h>
#include <itp/array.h>
#include <itp/node.h>
#include <itp/memory.h>
#include <itp/stack_trace.h>

#include <string.h>

void itp_init_node_array(itp_node_array_tp array) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!array) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_node_array> Array is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    array->data = NULL;
    array->size = 0;
    array->count = 0;
}

itp_error_code_t itp_expand_node_array(itp_node_array_tp array, uint8_t size) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!array) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_expand_node_array> Array is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (array->data) {
        itp_size_t new_size = array->size + size;
        if (new_size > 0xFF) {
            itp_debug_print(ITP_ERRC_OUT_OF_BOUNDS, "itp_expand_node_array> Children array out of bounds");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OUT_OF_BOUNDS;
        }
        if (array->size > 0) {
            itp_node_tp* data = array->data;
            array->data = itp_malloc((uint32_t)new_size * sizeof(itp_node_tp), "itp_expand_node_array:data:1");
            if (!array->data) {
                array->data = data;
                itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_expand_node_array> Failed to allocate memory for nodes array");
                itp_print_stack_trace(stdout);
                return ITP_ERRC_MALLOC_FAILED;
            }
            memcpy(array->data, data, array->size * sizeof(itp_node_tp));
            memset(array->data + array->size, 0, size * sizeof(itp_node_tp));
            array->size = (uint8_t)new_size;
            itp_free(data);
        } else {
            itp_free(array->data);
            array->data = itp_malloc((uint32_t)size * sizeof(itp_node_tp), "itp_expand_node_array:data:2");
            if (!array->data) {
                itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_expand_node_array> Failed to allocate memory for nodes array");
                itp_print_stack_trace(stdout);
                return ITP_ERRC_MALLOC_FAILED;
            }
            memset(array->data, 0, size * sizeof(itp_node_tp));
            array->size = size;
        }
    } else {
        array->data = itp_malloc((uint32_t)size * sizeof(itp_node_tp), "itp_expand_node_array:data:3");
        if (!array->data) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_expand_node_array> Failed to allocate memory for nodes array");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        array->size = size;
        if (array->count > 0) {
            array->count = 0;
        }
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_add_node_to_array(itp_node_array_tp array, itp_node_tp node) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!array) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_add_node_to_array> Array is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!node) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_add_node_to_array> Node is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Проверяем, нет ли уже узла с таким адресом в массиве
    for (uint8_t i = 0; i < array->count; ++i) {
        if (array->data[i] == node) {
            itp_debug_print(ITP_ERRC_GENERAL, "itp_add_node_to_array> This node already added");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_GENERAL;
        #ifdef ITP_ENABLE_INTERNAL_CHECKS
        } else if (!array->data[i]) {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_add_node_to_array> Null pointer found in nodes array");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_INTERNAL;
        #endif // ITP_ENABLE_INTERNAL_CHECKS
        } else if (array->data[i]->address == node->address) {
            itp_debug_print(ITP_ERRC_GENERAL, "itp_add_node_to_array> Node with this address already added");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_GENERAL;
        }
    }

    // Добавляем в конец массива
    if (array->count >= array->size) {
        itp_error_code_t error = itp_expand_node_array(array, 1);
        if (error) return error;
    }
    array->data[array->count++] = node;
    return ITP_ERRC_NONE;
}

void itp_clear_node_array(itp_node_array_tp array) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!array) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_clear_node_array> Array is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (array->data) {
        uint16_t count = (uint16_t)array->count;
        for (uint16_t i = 0; i < count; i++)
            itp_free_node(array->data[i]);
        itp_free(array->data);
        array->data = NULL;
    }
    array->count = 0;
    array->size = 0;
}

void itp_init_address_array(itp_address_array_tp array) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!array) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_address_array> Array is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    array->data = NULL;
    array->size = 0;
    array->count = 0;
}

itp_error_code_t itp_expand_address_array(itp_address_array_tp array, uint8_t size) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!array) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_expand_address_array> Node is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (array->data) {
        itp_size_t new_size = array->size + size;
        if (new_size > 0xFF) {
            itp_debug_print(ITP_ERRC_OUT_OF_BOUNDS, "itp_expand_address_array> Subnodes array out of bounds");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OUT_OF_BOUNDS;
        }
        if (array->size > 0) {
            uint8_t* data = array->data;
            array->data = itp_malloc((uint32_t)new_size * sizeof(uint8_t), "itp_expand_address_array:data:1");
            if (!array->data) {
                array->data = data;
                itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_expand_address_array> Failed to allocate memory for nodes array");
                itp_print_stack_trace(stdout);
                return ITP_ERRC_MALLOC_FAILED;
            }
            memcpy(array->data, data, array->size * sizeof(uint8_t));
            memset(array->data + array->size, 0, size * sizeof(uint8_t));
            array->size = (uint8_t)new_size;
            itp_free(data);
        } else {
            itp_free(array->data);
            array->data = itp_malloc((uint32_t)size * sizeof(uint8_t), "itp_expand_address_array:data:2");
            if (!array->data) {
                itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_expand_address_array> Failed to allocate memory for nodes array");
                itp_print_stack_trace(stdout);
                return ITP_ERRC_MALLOC_FAILED;
            }
            memset(array->data, 0, size * sizeof(uint8_t));
            array->size = size;
        }
    } else {
        array->data = itp_malloc((uint32_t)size * sizeof(uint8_t), "itp_expand_address_array:data:3");
        if (!array->data) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_expand_address_array> Failed to allocate memory for nodes array");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        array->size = size;
        if (array->count > 0) {
            array->count = 0;
        }
    }
    return ITP_ERRC_NONE;
}

uint8_t itp_is_array_contains(itp_address_array_tp array, uint8_t address) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!array) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_is_array_contains> Array is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_is_array_contains> Address is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Проверяем, нет ли уже такого адреса в массиве
    for (uint8_t i = 0; i < array->count; ++i)
        if (array->data[i] == address) return 1;
    return 0;
}

itp_error_code_t itp_add_address_to_array(itp_address_array_tp array, uint8_t address) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!array) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_add_address_to_array> Array is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_add_address_to_array> Address is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Проверяем, нет ли уже такого адреса в массиве
    for (uint8_t i = 0; i < array->count; ++i) {
        if (array->data[i] == address) {
            itp_debug_print(ITP_ERRC_GENERAL, "itp_add_address_to_array> Address %d already added", (int)address);
            itp_print_stack_trace(stdout);
            return ITP_ERRC_GENERAL;
        #ifdef ITP_ENABLE_INTERNAL_CHECKS
        } else if (!array->data[i]) {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_add_address_to_array> Null address found in array");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_INTERNAL;
        #endif // ITP_ENABLE_INTERNAL_CHECKS
        }
    }

    // Добавляем в конец массива
    if (array->count >= array->size) {
        itp_error_code_t error = itp_expand_address_array(array, 1);
        if (error) return error;
    }
    array->data[array->count++] = address;
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_remove_address_from_array(itp_address_array_tp array, uint8_t address) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!array) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_remove_address_from_array> Array is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_remove_address_from_array> Address is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    for (uint8_t i = 0; i < array->count; ++i) {
        if (array->data[i] == address) {
            if (i < array->count - 1)
                memcpy(array->data + i, array->data + i + 1, array->count - i - 1);
            array->count--;
            return ITP_ERRC_NONE;
        #ifdef ITP_ENABLE_INTERNAL_CHECKS
        } else if (!array->data[i]) {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_remove_address_from_array> Null address found in array");
            itp_print_stack_trace(stdout);
        #endif // ITP_ENABLE_INTERNAL_CHECKS
        }
    }

    return ITP_ERRC_ADDRESS_NOT_FOUND;
}

void itp_clear_address_array(itp_address_array_tp array) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!array) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_clear_address_array> Array is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (array->data) {
        itp_free(array->data);
        array->data = NULL;
    }
    array->count = 0;
    array->size = 0;
}
