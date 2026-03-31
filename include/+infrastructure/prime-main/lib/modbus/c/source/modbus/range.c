#include <modbus/debug.h>
#include <modbus/stack_trace.h>
#include <modbus/range.h>
#include <modbus/memory.h>

#ifdef MODBUS_EXTEND_CALLBACKS
#include <modbus/callback.h>
#endif // MODBUS_EXTEND_CALLBACKS

#include <string.h>

void mbs_init_ranged_handler(mbs_ranged_handler_tp handler) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!handler) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_init_ranged_handler> Handler is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    memset(handler, 0x0, sizeof(mbs_ranged_handler_t));
}

void mbs_free_ranged_handler(mbs_ranged_handler_tp handler) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!handler) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_free_ranged_handler> Handler is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    #ifdef MODBUS_EXTEND_CALLBACKS
    mbs_free_request_eparam(handler->eparam);
    #endif // MODBUS_EXTEND_CALLBACKS
    mbs_free(handler);
}

void mbs_init_range(mbs_range_tp range, uint8_t command) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!range) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_init_range> Range is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    range->command = command;
    range->first = NULL;
    range->next = NULL;
}

mbs_error_code_t mbs_push_ranged_handler(mbs_range_tp range, mbs_ranged_handler_tp handler) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!range) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_push_ranged_handler> Range is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!handler) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_push_ranged_handler> Handler is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_ranged_handler_tp last = range->first;
    if (!last) {
        range->first = handler;
        return MBS_ERRC_NONE;
    } else {
        if (range->first->first > handler->first) {
            handler->next = range->first;
            range->first = handler;
            return MBS_ERRC_NONE;
        } else if (range->first->first == handler->first) {
            if (range->first->last >= handler->last) {
                handler->next = range->first;
                range->first = handler;
                return MBS_ERRC_NONE;
            }
        }
    }

    mbs_ranged_handler_tp next;
    while (last) {
        next = last->next;
        if (next) {
            if (next->first > handler->first) {
                handler->next = next;
                last->next = handler;
                break;
            } else if (next->first == handler->first) {
                if (next->last >= handler->last) {
                    handler->next = next;
                    last->next = handler;
                    break;
                } else {
                    last = next;
                }
            } else {
                last = next;
            }
        } else {
            last->next = handler;
            break;
        }
    }
    return MBS_ERRC_NONE;
}

mbs_ranged_handler_tp mbs_erase_ranged_handler(mbs_range_tp range, mbs_ranged_handler_tp handler) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!range) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_erase_ranged_handler> Range is null");
        mbs_print_stack_trace(stdout);
        return NULL;
    }
    if (!handler) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_erase_ranged_handler> Handler is null");
        mbs_print_stack_trace(stdout);
        return NULL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    // Проверяем первый элемент
    mbs_ranged_handler_tp last = range->first;
    if (!last) {
        return NULL;
    } else if (last == handler) {
        range->first = last->next;
        mbs_free_ranged_handler(last);
        return range->first;
    }

    // Проверяем остальные элементы
    mbs_ranged_handler_tp next;
    for (;;) {
        next = last->next;
        if (next) {
            if (next == handler) {
                last->next = next->next;
                mbs_free_ranged_handler(next);
                return last->next;
            } else last = next;
        } else break;
    }
    return NULL;
}

uint8_t mbs_range_is_empty(mbs_range_tp range) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!range) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_range_is_empty> Range is null");
        mbs_print_stack_trace(stdout);
        return 1;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    return (!range->first) ? 1 : 0;
}

void mbs_clear_range(mbs_range_tp range) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!range) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_clear_range> Range is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (range->first) {
        mbs_ranged_handler_tp item = range->first;
        while (item) {
            mbs_ranged_handler_tp next = item->next;
            mbs_free_ranged_handler(item);
            item = next;
        }
        range->first = NULL;
    }
}

void mbs_free_range(mbs_range_tp range) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!range) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_free_range> Range is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_clear_range(range);
    mbs_free(range);
}

void mbs_init_ranges_list(mbs_ranges_list_tp list) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!list) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_init_ranges_list> List is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    list->first = NULL;
}

mbs_error_code_t mbs_push_range(mbs_ranges_list_tp list, mbs_range_tp range) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!list) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_push_range> List is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    if (!range) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_push_range> Range is null");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    mbs_range_tp last = list->first;
    if (!last) {
        list->first = range;
        return MBS_ERRC_NONE;
    }
    while (last) {
        if (last == range) {
            return MBS_ERRC_NONE;
        } else if (last->next) {
            last = last->next;
        } else break;
    }
    if (last) {
        last->next = range;
        return MBS_ERRC_NONE;
    } else {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_push_range> Failed to add range to list");
        mbs_print_stack_trace(stdout);
        return MBS_ERRC_INTERNAL;
    }
}

mbs_range_tp mbs_find_range(mbs_ranges_list_tp list, mbs_byte_t command) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!list) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_find_range> List is null");
        mbs_print_stack_trace(stdout);
        return NULL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    // Проверяем существующие диапазоны
    mbs_range_tp last = list->first;
    while (last) {
        if (last->command != command) {
            last = last->next;
        } else return last;
    }
    return NULL;
}

mbs_range_tp mbs_get_or_create_range(mbs_ranges_list_tp list, mbs_byte_t command, mbs_error_code_t* error) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!list) {
        if (error) *error = MBS_ERRC_INTERNAL;
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_get_or_create_range> List is null");
        mbs_print_stack_trace(stdout);
        return NULL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    // Проверяем список на пустоту
    if (!list->first) {
        mbs_range_tp range = (mbs_range_tp)mbs_malloc(sizeof(mbs_range_t), "mbs_get_or_create_range:range");
        if (!range) {
            if (error) *error = MBS_ERRC_MALLOC_FAILED;
            mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_get_or_create_range> Memory allocation failed");
            return NULL;
        }
        mbs_init_range(range, command);
        list->first = range;
        return range;
    }

    // Проверяем существующие диапазоны
    mbs_range_tp last = list->first;
    while (last) {
        if (last->command == command) {
            return last;
        } else if (last->next) {
            last = last->next;
        } else {
            mbs_range_tp range = (mbs_range_tp)mbs_malloc(sizeof(mbs_range_t), "mbs_get_or_create_range:range");
            if (!range) {
                if (error) *error = MBS_ERRC_MALLOC_FAILED;
                mbs_debug_print(MBS_ERRC_MALLOC_FAILED, "mbs_get_or_create_range> Memory allocation failed");
                return NULL;
            }
            mbs_init_range(range, command);
            last->next = range;
            return range;
        }
    }

    // Недостижимый код
    if (error) *error = MBS_ERRC_INTERNAL;
    mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_get_or_create_range> Failed to create range");
    return NULL;
}

mbs_range_tp mbs_erase_range(mbs_ranges_list_tp list, mbs_range_tp range) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!list) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_erase_task> Ranges list is null");
        mbs_print_stack_trace(stdout);
        return NULL;
    }
    if (!range) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_erase_task> Range is null");
        mbs_print_stack_trace(stdout);
        return NULL;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    // Проверяем первый элемент
    mbs_range_tp last = list->first;
    if (!last) {
        return NULL;
    } else if (last == range) {
        list->first = last->next;
        mbs_free_range(last);
        return list->first;
    }

    // Проверяем остальные элементы
    for (;;) {
        mbs_range_tp next = last->next;
        if (next) {
            if (next == range) {
                last->next = next->next;
                mbs_free_range(next);
                return last->next;
            } else last = next;
        } else break;
    }
    return NULL;
}

uint8_t mbs_ranges_list_is_empty(mbs_ranges_list_tp list) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!list) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_ranges_list_is_empty> Ranges list is null");
        mbs_print_stack_trace(stdout);
        return 1;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    return (!list->first) ? 1 : 0;
}

void mbs_clear_ranges_list(mbs_ranges_list_tp list) {

    #ifdef MODBUS_ENABLE_INTERNAL_CHECKS
    if (!list) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "mbs_clear_ranges_list> Ranges list is null");
        mbs_print_stack_trace(stdout);
        return;
    }
    #endif // MODBUS_ENABLE_INTERNAL_CHECKS

    if (list->first) {
        mbs_range_tp item = list->first;
        while (item) {
            mbs_range_tp next = item->next;
            mbs_free_range(item);
            item = next;
        }
        list->first = NULL;
    }
}
