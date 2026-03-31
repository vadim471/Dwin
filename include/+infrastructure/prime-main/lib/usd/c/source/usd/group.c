#include <usd/debug.h>
#include <usd/group.h>
#include <usd/device.h>
#include <usd/memory.h>
#include <usd/stack_trace.h>

void usd_init_device_item(usd_device_item_tp item, usd_device_tp device, uint8_t weak_ptr) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!item) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_init_device_item> Device item is null");
        usd_print_stack_trace(stdout);
        return;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    item->device = device;
    item->next = NULL;
    item->weak_ptr = weak_ptr;
}

void usd_init_device_group(usd_device_group_tp group) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!group) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_init_device_group> Device group is null");
        usd_print_stack_trace(stdout);
        return;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    group->first = NULL;
}

usd_error_code_t usd_push_device(usd_device_group_tp group, usd_device_tp device, uint8_t weak_ptr) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!group) {
        usd_debug_print(USD_ERRC_INTERNAL, "usd_push_device> Group is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_INTERNAL;
    }
    if (!device) {
        usd_debug_print(USD_ERRC_INTERNAL, "usd_push_device> Device is null");
        usd_print_stack_trace(stdout);
        return USD_ERRC_INTERNAL;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    usd_device_item_tp last = group->first;
    if (!last) {
        usd_device_item_tp item = usd_malloc(sizeof(usd_device_item_t), "usd_push_device:usd_device_item");
        if (!item) {
            usd_debug_print(USD_ERRC_MALLOC_FAILED, "usd_push_device> Memory allocation failed");
            usd_print_stack_trace(stdout);
            return USD_ERRC_MALLOC_FAILED;
        }
        usd_init_device_item(item, device, weak_ptr);
        group->first = item;
        return USD_ERRC_NONE;
    }
    for (uint8_t count = 0;;) {
        if (last->device == device) {
            // Игнорируем одинаковые узлы
            return USD_ERRC_NONE;
        }
        if (++count < USD_MAX_DEVICES) {
            if (last->next) {
                last = last->next;
            } else break;
        } else {
            usd_debug_print(USD_ERRC_OVERFLOW, "usd_push_device> Devices group overflow");
            usd_print_stack_trace(stdout);
            return USD_ERRC_OVERFLOW;
        }
    }
    if (last) {
        usd_device_item_tp item = usd_malloc(sizeof(usd_device_item_t), "usd_push_device:usd_device_item");
        if (!item) {
            usd_debug_print(USD_ERRC_MALLOC_FAILED, "usd_push_device> Memory allocation failed");
            usd_print_stack_trace(stdout);
            return USD_ERRC_MALLOC_FAILED;
        }
        usd_init_device_item(item, device, weak_ptr);
        last->next = item;
        return USD_ERRC_NONE;
    } else {
        usd_debug_print(USD_ERRC_INTERNAL, "usd_push_device> Failed to add pending node to list");
        usd_print_stack_trace(stdout);
        return USD_ERRC_INTERNAL;
    }
}

usd_device_item_tp usd_erase_device(usd_device_group_tp group, usd_device_tp device) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!group) {
        usd_debug_print(USD_ERRC_INTERNAL, "usd_erase_device> Group is null");
        usd_print_stack_trace(stdout);
        return NULL;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (!device) return NULL;

    // Проверяем первый элемент
    usd_device_item_tp last = group->first;
    if (!last) {
        return NULL;
    } else if (last->device == device) {
        group->first = last->next;
        usd_free(last);
        return group->first;
    }

    // Проверяем остальные элементы
    for (;;) {
        usd_device_item_tp next = last->next;
        if (next) {
            if (next->device == device) {
                last->next = next->next;
                usd_free(next);
                return last->next;
            } else last = next;
        } else break;
    }
    return NULL;
}

uint8_t usd_device_is_exist(usd_device_group_tp group, usd_device_tp device) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!group) {
        usd_debug_print(USD_ERRC_INTERNAL, "usd_device_is_exist> Group is null");
        usd_print_stack_trace(stdout);
        return 0;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (!device) return 0;

    usd_device_item_tp last = group->first;
    while (last) {
        if (last->device == device) {
            return 1;
        } else last = last->next;
    }
    return 0;
}

usd_device_tp usd_find_device_by_id(usd_device_group_tp group, uint16_t id) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!group) {
        usd_debug_print(USD_ERRC_INTERNAL, "usd_find_device_by_id> Group is null");
        usd_print_stack_trace(stdout);
        return NULL;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (!id) return NULL;

    usd_device_item_tp last = group->first;
    while (last) {
        if (last->device->id == id) {
            return last->device;
        } else last = last->next;
    }
    return NULL;
}

usd_device_tp usd_find_device_by_parameter(usd_device_group_tp group, uint8_t key) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!group) {
        usd_debug_print(USD_ERRC_INTERNAL, "usd_find_device_by_parameter> Group is null");
        usd_print_stack_trace(stdout);
        return NULL;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (!key) return NULL;

    usd_device_item_tp last = group->first;
    while (last) {
        if (usd_is_has_parameter(last->device, key)) {
            return last->device;
        } else last = last->next;
    }
    return NULL;
}

uint8_t usd_group_is_empty(usd_device_group_tp group) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!group) {
        usd_debug_print(USD_ERRC_INTERNAL, "usd_group_is_empty> Group is null");
        usd_print_stack_trace(stdout);
        return 1;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    return (!group->first) ? 1 : 0;
}

void usd_clear_group(usd_device_group_tp group) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!group) {
        usd_debug_print(USD_ERRC_INTERNAL, "usd_clear_group> Group is null");
        usd_print_stack_trace(stdout);
        return;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    if (group->first) {
        usd_device_item_tp item = group->first;
        while (item) {
            usd_device_item_tp next = item->next;
            if (!item->weak_ptr)
                usd_free_device(item->device);
            usd_free(item);
            item = next;
        }
        group->first = NULL;
    }
}

void usd_free_group(usd_device_group_tp group) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!group) {
        usd_debug_print(USD_ERRC_INTERNAL, "usd_free_group> Group is null");
        usd_print_stack_trace(stdout);
        return;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    usd_clear_group(group);
    usd_free(group);
}
