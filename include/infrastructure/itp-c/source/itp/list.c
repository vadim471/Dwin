#include <itp/list.h>
#include <itp/debug.h>
#include <itp/memory.h>
#include <itp/frame.h>
#include <itp/node.h>
#include <itp/stack_trace.h>

void itp_init_pending_node(itp_pending_node_tp pending, itp_node_tp node) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!pending) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_pending_node> Pending struct is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    pending->node = node;
    pending->next = NULL;
}

void itp_init_pending_list(itp_pending_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_pending_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    list->first = NULL;
}

itp_error_code_t itp_push_pending_node(itp_pending_list_tp list, itp_node_tp node) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_pending_node> List is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!node) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_pending_node> Node is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_pending_node_tp last = list->first;
    if (!last) {
        itp_pending_node_tp pending = itp_malloc(sizeof(itp_pending_node_t), "itp_push_pending_node:itp_pending_node");
        if (!pending) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_pending_node> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        itp_init_pending_node(pending, node);
        list->first = pending;
        return ITP_ERRC_NONE;
    }
    for (uint8_t count = 0;;) {
        if (last->node == node) {
            // Игнорируем одинаковые узлы
            return ITP_ERRC_NONE;
        }
        if (++count < ITP_MAX_PENDING_CONNECTIONS) {
            if (last->next) {
                last = last->next;
            } else break;
        } else {
            itp_debug_print(ITP_ERRC_OVERFLOW, "itp_push_pending_node> Pending connections list overflow");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OVERFLOW;
        }
    }
    if (last) {
        itp_pending_node_tp pending = itp_malloc(sizeof(itp_pending_node_t), "itp_push_pending_node:itp_pending_node");
        if (!pending) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_pending_node> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        itp_init_pending_node(pending, node);
        last->next = pending;
        return ITP_ERRC_NONE;
    } else {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_pending_node> Failed to add pending node to list");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
}

itp_pending_node_tp itp_erase_pending_node(itp_pending_list_tp list, itp_node_tp node) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_erase_pending_node> List is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (!node) return NULL;

    // Проверяем первый элемент
    itp_pending_node_tp last = list->first;
    if (!last) {
        return NULL;
    } else if (last->node == node) {
        list->first = last->next;
        itp_free(last);
        return list->first;
    }

    // Проверяем остальные элементы
    for (;;) {
        itp_pending_node_tp next = last->next;
        if (next) {
            if (next->node == node) {
                last->next = next->next;
                itp_free(next);
                return last->next;
            } else last = next;
        } else break;
    }
    return NULL;
}

uint8_t itp_pending_node_is_exist(itp_pending_list_tp list, itp_node_tp node) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_pending_node_is_exist> List is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (!node) return 0;

    itp_pending_node_tp last = list->first;
    while (last) {
        if (last->node == node) {
            return 1;
        } else last = last->next;
    }
    return 0;
}

uint8_t itp_pending_list_is_empty(itp_pending_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_pending_list_is_empty> List is null");
        itp_print_stack_trace(stdout);
        return 1;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    return (!list->first) ? 1 : 0;
}

void itp_clear_pending_list(itp_pending_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_clear_pending_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (list->first) {
        itp_pending_node_tp item = list->first;
        while (item) {
            itp_pending_node_tp next = item->next;
            itp_free_node(item->node);
            itp_free(item);
            item = next;
        }
        list->first = NULL;
    }
}

void itp_init_frame_item(itp_frame_item_tp item, itp_frame_tp frame) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!item) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_frame_item> Item is null");
        itp_print_stack_trace(stdout);
        return;
    }
    if (!frame) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_frame_item> Frame is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    item->frame = frame;
    item->time = 0;
    item->sender = 0;
    item->errors = 0;
    item->acknowledged = 0;
    item->processed = 0;
    item->wait_data = 0;
    item->next = NULL;
    itp_init_handler_wrapper(
        &item->handler, NULL,
        #ifdef ITP_EXTEND_CALLBACKS
        0,
        #endif // ITP_EXTEND_CALLBACKS
        0, 0
    );
}

void itp_free_frame_item(itp_frame_item_tp item) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!item) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_free_frame_item> Item is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_dispose_handler_wrapper(&item->handler);
    if (item->frame) itp_free_frame(item->frame);
    itp_free(item);
}

void itp_init_frame_list(itp_frame_list_tp list, itp_size_t max) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_frame_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    list->first = NULL;
    list->max = max;
}

itp_error_code_t itp_push_frame_item(itp_frame_list_tp list, itp_frame_item_tp item) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_frame_item> List is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!item) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_frame_item> Item is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (item->frame->status != ITP_SYM_REQ &&
        item->frame->status != ITP_SYM_DAT &&
        item->frame->status != ITP_SYM_CTR) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_frame_item> Frame with status %d found", (int)item->frame->status);
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_frame_item_tp last = list->first;
    if (!last) {
        list->first = item;
        return ITP_ERRC_NONE;
    }
    for (uint8_t count = 0;;) {
        if (last == item) {
            return ITP_ERRC_NONE;
        }
        if (++count < list->max) {
            if (last->next) {
                last = last->next;
            } else break;
        } else {
            itp_debug_print(ITP_ERRC_OVERFLOW, "itp_push_frame_item> List overflow");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OVERFLOW;
        }
    }
    if (last) {
        last->next = item;
        return ITP_ERRC_NONE;
    } else {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_frame_item> Failed to enlist frame");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
}

itp_frame_item_tp itp_erase_frame_item(itp_frame_list_tp list, itp_frame_tp frame) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_erase_frame_item> List is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    //if (!frame) return NULL;

    // Проверяем первый элемент
    itp_frame_item_tp last = list->first;
    if (!last) {
        //itp_debug_print(ITP_ERRC_GENERAL, "itp_erase_frame_item> List is empty");
        //itp_print_stack_trace(stdout);
        return NULL;
    } else if (last->frame == frame) {
        list->first = last->next;
        itp_free_frame_item(last);
        return list->first;
    }

    // Проверяем остальные элементы
    for (;;) {
        itp_frame_item_tp next = last->next;
        if (next) {
            if (next->frame == frame) {
                last->next = next->next;
                itp_free_frame_item(next);
                return last->next;
            } else last = next;
        } else break;
    }
    return NULL;
}

uint8_t itp_frame_list_is_empty(itp_frame_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_frame_list_is_empty> List is null");
        itp_print_stack_trace(stdout);
        return 1;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    return (!list->first) ? 1 : 0;
}

void itp_clear_frame_list(itp_frame_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_clear_frame_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (list->first) {
        itp_frame_item_tp item = list->first;
        while (item) {
            itp_frame_item_tp next = item->next;
            itp_free_frame_item(item);
            item = next;
        }
        list->first = NULL;
    }
}

void itp_init_handler_item(itp_handler_item_tp handler, uint8_t address, uint16_t command, itp_on_request_fp on_request
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
    ) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!handler) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_handler_item> Handler struct is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    handler->address = address;
    handler->command = command;
    handler->on_request = on_request;
    #ifdef ITP_EXTEND_CALLBACKS
    handler->on_request_eparam = eparam;
    #endif // ITP_EXTEND_CALLBACKS
    handler->next = NULL;
}

void itp_free_handler_item(itp_handler_item_tp handler) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!handler) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_free_handler_item> Handler struct is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_free_any_handler(
        (void*)handler->on_request
        #ifdef ITP_EXTEND_CALLBACKS
        , handler->on_request_eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
    itp_free(handler);
}

void itp_init_handler_list(itp_handler_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_handler_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    list->first = NULL;
}

itp_error_code_t itp_push_handler_item(itp_handler_list_tp list, uint8_t address, uint16_t command, itp_on_request_fp on_request
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
    ) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_handler_item> List is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!command) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_handler_item> Wrong command");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!on_request) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_handler_item> Callback is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_handler_item_tp last = list->first;
    if (!last) {
        itp_handler_item_tp handler = itp_malloc(sizeof(itp_handler_item_t), "itp_push_handler_item:itp_handler_item");
        if (!handler) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_handler_item> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        itp_init_handler_item(
            handler, address, command, on_request
            #ifdef ITP_EXTEND_CALLBACKS
            , eparam
            #endif // ITP_EXTEND_CALLBACKS
        );
        list->first = handler;
        return ITP_ERRC_NONE;
    }
    for (uint8_t count = 0;;) {
        if (last->address == address &&
            last->command == command) {
            // 24.04.2020. Free old handler
            itp_free_any_handler(
                (void*)last->on_request
                #ifdef ITP_EXTEND_CALLBACKS
                , last->on_request_eparam
                #endif // ITP_EXTEND_CALLBACKS
            );
            // ----------------------------
            last->on_request = on_request;
            #ifdef ITP_EXTEND_CALLBACKS
            last->on_request_eparam = eparam;
            #endif // ITP_EXTEND_CALLBACKS
            return ITP_ERRC_NONE;
        }
        if (++count < ITP_MAX_REQUEST_HANDLERS) {
            if (last->next) {
                last = last->next;
            } else break;
        } else {
            itp_debug_print(ITP_ERRC_OVERFLOW, "itp_push_handler_item> Handlers list overflow");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OVERFLOW;
        }
    }
    if (last) {
        itp_handler_item_tp handler = itp_malloc(sizeof(itp_handler_item_t), "itp_push_handler_item:itp_handler_item");
        if (!handler) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_handler_item> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        itp_init_handler_item(
            handler, address, command, on_request
            #ifdef ITP_EXTEND_CALLBACKS
            , eparam
            #endif // ITP_EXTEND_CALLBACKS
        );
        last->next = handler;
        return ITP_ERRC_NONE;
    } else {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_handler_item> Failed to add handler to list");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
}

static uint8_t itp_erase_condition(itp_handler_item_tp item, uint8_t address, uint16_t command) {
    if (item->command == command) {
        if (item->address == 0) {
            return 1;
        } else if (address == 0) {
            return 1;
        } else if (item->address == address) {
            return 1;
        } else return 0;
    } else return 0;
}

itp_handler_item_tp itp_erase_handler_item(itp_handler_list_tp list, uint8_t address, uint16_t command) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_erase_handler_item> List is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (!command) return NULL;

    // Проверяем первый элемент
    itp_handler_item_tp last = list->first;
    if (!last) {
        //itp_debug_print(ITP_ERRC_GENERAL, "itp_erase_handler_item> List is empty");
        return NULL;
    } else if (itp_erase_condition(last, address, command)) {
        list->first = last->next;
        itp_free_handler_item(last);
        return list->first;
    }

    // Проверяем остальные элементы
    for (;;) {
        itp_handler_item_tp next = last->next;
        if (next) {
            if (itp_erase_condition(last, address, command)) {
                last->next = next->next;
                itp_free_handler_item(next);
                return last->next;
            } else last = next;
        } else break;
    }
    return NULL;
}

itp_handler_item_tp itp_find_handler(itp_handler_list_tp list, uint8_t address, uint16_t command) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_handler> List is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (!command) return NULL;

    // Проверяем цепочку элементов
    itp_handler_item_tp last = list->first;
    while (last) {
        #ifdef ITP_ENABLE_INTERNAL_CHECKS
        if (!last->on_request) {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_handler> Null pointer found in handlers list");
            itp_print_stack_trace(stdout);
            return NULL;
        }
        #endif // ITP_ENABLE_INTERNAL_CHECKS
        if ((last->address == 0) ||
            (last->address == address)) {
            if (last->command == command) {
                return last;
            }
        }
        last = last->next;
    }
    return NULL;
}

uint8_t itp_handler_list_is_empty(itp_handler_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_list_is_empty> List is null");
        itp_print_stack_trace(stdout);
        return 1;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    return (!list->first) ? 1 : 0;
}

void itp_clear_handler_list(itp_handler_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_clear_handler_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (list->first) {
        itp_handler_item_tp item = list->first;
        while (item) {
            itp_handler_item_tp next = item->next;
            itp_free_handler_item(item);
            item = next;
        }
        list->first = NULL;
    }
}

void itp_init_proxy_item(itp_proxy_item_tp item, itp_time_t time, itp_handler_tp handler) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!item) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_proxy_item> Item struct is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    item->time = time;
    item->address = 0;
    item->command = 0;
    item->order = 0;
    item->next = NULL;
    itp_init_handler_wrapper(
        &item->handler, (void*)handler,
        #ifdef ITP_EXTEND_CALLBACKS
        0,
        #endif // ITP_EXTEND_CALLBACKS
        1, 0
    );
}

void itp_free_proxy_item(itp_proxy_item_tp item) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!item) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_free_proxy_item> Item is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_dispose_handler_wrapper(&item->handler);
    itp_free(item);
}

void itp_init_proxy_list(itp_proxy_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_proxy_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    list->first = NULL;
}

itp_error_code_t itp_push_proxy_item(itp_proxy_list_tp list, itp_time_t time, uint8_t address, uint16_t command, uint16_t order, itp_handler_tp handler) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_proxy_item> List is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!handler) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_proxy_item> Handler is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_proxy_item_tp last = list->first;
    if (!last) {
        itp_proxy_item_tp item = itp_malloc(sizeof(itp_proxy_item_t), "itp_push_proxy_item:itp_proxy_item");
        if (!item) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_proxy_item> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        itp_init_proxy_item(item, time, handler);
        item->address = address;
        item->command = command;
        item->order = order;
        list->first = item;
        return ITP_ERRC_NONE;
    }
    for (uint8_t count = 0;;) {
        if (++count < ITP_MAX_PROXY_HANDLERS) {
            if (last->next) {
                last = last->next;
            } else break;
        } else {
            itp_debug_print(ITP_ERRC_OVERFLOW, "itp_push_proxy_item> Proxy handlers list overflow");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OVERFLOW;
        }
    }
    if (last) {
        itp_proxy_item_tp item = itp_malloc(sizeof(itp_proxy_item_t), "itp_push_proxy_item:itp_proxy_item");
        if (!item) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_proxy_item> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        itp_init_proxy_item(item, time, handler);
        item->address = address;
        item->command = command;
        item->order = order;
        last->next = item;
        return ITP_ERRC_NONE;
    } else {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_proxy_item> Failed to add proxy handler to list");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
}

itp_proxy_item_tp itp_erase_proxy_item(itp_proxy_list_tp list, itp_proxy_item_tp item) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_erase_proxy_item> List is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (!item) return NULL;

    // Проверяем первый элемент
    itp_proxy_item_tp last = list->first;
    if (!last) {
        return NULL;
    } else if (last == item) {
        list->first = last->next;
        itp_free_proxy_item(last);
        return list->first;
    }

    // Проверяем остальные элементы
    for (;;) {
        itp_proxy_item_tp next = last->next;
        if (next) {
            if (next == item) {
                last->next = next->next;
                itp_free_proxy_item(next);
                return last->next;
            } else last = next;
        } else break;
    }
    return NULL;
}

itp_proxy_item_tp itp_find_proxy(itp_proxy_list_tp list, uint8_t address, uint16_t order) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_proxy> List is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Проверяем цепочку элементов
    itp_proxy_item_tp last = list->first;
    while (last) {
        #ifdef ITP_ENABLE_INTERNAL_CHECKS
        if (!last->handler.handler) {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_proxy> Null pointer found in handlers list");
            itp_print_stack_trace(stdout);
            return NULL;
        }
        #endif // ITP_ENABLE_INTERNAL_CHECKS
        if (last->address == address ||
            last->order == order) {
            return last;
        } else last = last->next;
    }
    return NULL;
}

uint8_t itp_proxy_list_is_empty(itp_proxy_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_proxy_list_is_empty> List is null");
        itp_print_stack_trace(stdout);
        return 1;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    return (!list->first) ? 1 : 0;
}

void itp_clear_proxy_list(itp_proxy_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_clear_proxy_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (list->first) {
        itp_proxy_item_tp item = list->first;
        while (item) {
            itp_proxy_item_tp next = item->next;
            itp_free_proxy_item(item);
            item = next;
        }
        list->first = NULL;
    }
}

void itp_init_listener_item(itp_listener_item_tp listener, uint8_t mark, itp_node_tp node) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!listener) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_listener_item> Listener struct is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    listener->node = node;
    listener->mark = mark;
    listener->next = NULL;
}

void itp_free_listener_item(itp_listener_item_tp listener) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!listener) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_free_listener_item> Listener struct is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (listener->node)
        itp_free_node(listener->node);
    itp_free(listener);
}

void itp_init_listener_list(itp_listener_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_listener_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    list->first = NULL;
}

itp_error_code_t itp_push_listener_item(itp_listener_list_tp list, uint8_t mark, itp_node_tp node) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_listener_item> List is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!node) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_listener_item> Node is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_listener_item_tp last = list->first;
    if (!last) {
        itp_listener_item_tp listener = itp_malloc(sizeof(itp_listener_item_t), "itp_push_listener_item:itp_listener_item");
        if (!listener) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_listener_item> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        itp_init_listener_item(listener, mark, node);
        list->first = listener;
        return ITP_ERRC_NONE;
    }
    for (uint8_t count = 0;;) {
        if (last->node == node) {
            return ITP_ERRC_NONE;
        }
        if (++count < ITP_MAX_CONNECTION_LISTENERS) {
            if (last->next) {
                last = last->next;
            } else break;
        } else {
            itp_debug_print(ITP_ERRC_OVERFLOW, "itp_push_listener_item> Listeners list overflow");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OVERFLOW;
        }
    }
    if (last) {
        itp_listener_item_tp listener = itp_malloc(sizeof(itp_listener_item_t), "itp_push_listener_item:itp_listener_item");
        if (!listener) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_listener_item> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        itp_init_listener_item(listener, mark, node);
        last->next = listener;
        return ITP_ERRC_NONE;
    } else {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_listener_item> Failed to add listener to list");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
}

itp_listener_item_tp itp_erase_listener_by_node(itp_listener_list_tp list, itp_node_tp node) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_erase_listener_by_node> List is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    if (!node) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_erase_listener_by_node> Node is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Проверяем первый элемент
    itp_listener_item_tp last = list->first;
    if (!last) {
        //itp_debug_print(ITP_ERRC_GENERAL, "itp_erase_listener_by_node> List is empty");
        return NULL;
    } else if (last->node == node) {
        list->first = last->next;
        itp_free_listener_item(last);
        return list->first;
    }

    // Проверяем остальные элементы
    for (;;) {
        itp_listener_item_tp next = last->next;
        if (next) {
            if (next->node == node) {
                last->next = next->next;
                itp_free_listener_item(next);
                return last->next;
            } else last = next;
        } else break;
    }
    return NULL;
}

itp_listener_item_tp itp_erase_listener_by_endpoint(itp_listener_list_tp list, itp_endpoint_tp endpoint) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_erase_listener_by_endpoint> List is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    if (!endpoint) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_erase_listener_by_endpoint> Endpoint is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Проверяем первый элемент
    itp_listener_item_tp last = list->first;
    if (!last) {
        //itp_debug_print(ITP_ERRC_GENERAL, "itp_erase_listener_by_endpoint> List is empty");
        return NULL;
    } else if (last->node->endpoint == endpoint) {
        list->first = last->next;
        itp_free_listener_item(last);
        return list->first;
    }

    // Проверяем остальные элементы
    for (;;) {
        itp_listener_item_tp next = last->next;
        if (next) {
            if (next->node->endpoint == endpoint) {
                last->next = next->next;
                itp_free_listener_item(next);
                return last->next;
            } else last = next;
        } else break;
    }
    return NULL;
}

itp_listener_item_tp itp_find_listener(itp_listener_list_tp list, uint8_t mark) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_listener> List is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (!mark) return NULL;

    // Проверяем цепочку элементов
    itp_listener_item_tp last = list->first;
    while (last) {
        #ifdef ITP_ENABLE_INTERNAL_CHECKS
        if (!last->node) {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_listener> Null pointer found in listeners list");
            itp_print_stack_trace(stdout);
            return NULL;
        }
        #endif // ITP_ENABLE_INTERNAL_CHECKS
        if (last->mark == mark) {
            return last;
        } else last = last->next;
    }
    return NULL;
}

uint8_t itp_listener_list_is_empty(itp_listener_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_listener_list_is_empty> List is null");
        itp_print_stack_trace(stdout);
        return 1;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    return (!list->first) ? 1 : 0;
}

void itp_clear_listener_list(itp_listener_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_clear_listener_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (list->first) {
        itp_listener_item_tp item = list->first;
        while (item) {
            itp_listener_item_tp next = item->next;
            itp_free_listener_item(item);
            item = next;
        }
        list->first = NULL;
    }
}

void itp_init_remote_listener_item(itp_remote_listener_item_tp listener, uint8_t address, uint16_t command) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!listener) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_remote_listener_item> Listener struct is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    listener->address = address;
    listener->command = command;
    listener->next = NULL;
}

void itp_free_remote_listener_item(itp_remote_listener_item_tp listener) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!listener) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_free_remote_listener_item> Listener struct is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_free(listener);
}

void itp_init_remote_listener_list(itp_remote_listener_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_remote_listener_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    list->first = NULL;
}

itp_error_code_t itp_push_remote_listener_item(itp_remote_listener_list_tp list, uint8_t address, uint16_t command) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_remote_listener_item> List is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_remote_listener_item> Address is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_remote_listener_item_tp last = list->first;
    if (!last) {
        itp_remote_listener_item_tp listener = itp_malloc(sizeof(itp_remote_listener_item_t), "itp_push_remote_listener_item:itp_remote_listener_item");
        if (!listener) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_remote_listener_item> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        itp_init_remote_listener_item(listener, address, command);
        list->first = listener;
        return ITP_ERRC_NONE;
    }
    for (uint8_t count = 0;;) {
        if (++count < ITP_MAX_COMMAND_LISTENERS) {
            if (last->address == address &&
                last->command == command) {
                return ITP_ERRC_NONE;
            } else if (last->next) {
                last = last->next;
            } else break;
        } else {
            itp_debug_print(ITP_ERRC_OVERFLOW, "itp_push_remote_listener_item> Listeners list overflow");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OVERFLOW;
        }
    }
    if (last) {
        itp_remote_listener_item_tp listener = itp_malloc(sizeof(itp_remote_listener_item_t), "itp_push_remote_listener_item:itp_remote_listener_item");
        if (!listener) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_remote_listener_item> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        itp_init_remote_listener_item(listener, address, command);
        last->next = listener;
        return ITP_ERRC_NONE;
    } else {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_push_remote_listener_item> Failed to add listener to list");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
}

itp_remote_listener_item_tp itp_erase_remote_listeners_by_address(itp_remote_listener_list_tp list, uint8_t address) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_erase_remote_listeners_by_address> List is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_erase_remote_listeners_by_address> Address is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_remote_listener_item_tp last;

CHECK_FIRST_REMOTE_ADDRESS:
    { // Проверяем первый элемент
        last = list->first;
        if (!last) {
            //itp_debug_print(ITP_ERRC_GENERAL, "itp_erase_remote_listeners_by_address> List is empty");
            return NULL;
        } else if (last->address == address) {
            list->first = last->next;
            itp_free_remote_listener_item(last);
            goto CHECK_FIRST_REMOTE_ADDRESS;
            //return list->first;
        }
    }

    // Проверяем остальные элементы
    for (;;) {
        itp_remote_listener_item_tp next = last->next;
        if (next) {
            if (next->address == address) {
                last->next = next->next;
                itp_free_remote_listener_item(next);
                continue;
            } else last = next;
        } else break;
    }
    return NULL;
}

itp_remote_listener_item_tp itp_erase_remote_listeners_by_command(itp_remote_listener_list_tp list, uint16_t command) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_erase_remote_listeners_by_command> List is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_remote_listener_item_tp last;

CHECK_FIRST_REMOTE_COMMAND:
    { // Проверяем первый элемент
        last = list->first;
        if (!last) {
            //itp_debug_print(ITP_ERRC_GENERAL, "itp_erase_remote_listeners_by_command> List is empty");
            return NULL;
        } else if (last->command == command || command == 0) {
            list->first = last->next;
            itp_free_remote_listener_item(last);
            goto CHECK_FIRST_REMOTE_COMMAND;
            //return list->first;
        }
    }

    // Проверяем остальные элементы
    for (;;) {
        itp_remote_listener_item_tp next = last->next;
        if (next) {
            if (next->command == command || command == 0) {
                last->next = next->next;
                itp_free_remote_listener_item(next);
                continue;
            } else last = next;
        } else break;
    }
    return NULL;
}

void itp_dump_remote_listener_list(itp_remote_listener_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_dump_remote_listener_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (list->first) {
        itp_remote_listener_item_tp item = list->first;
        while (item) {
            itp_debug_print(ITP_ERRC_TRACE, "itp_dump_remote_listener_list> Found listener 0x%04x for address %d", item->command, (int)item->address);
            item = item->next;
        }
    }
}

uint8_t itp_remote_listener_list_is_empty(itp_remote_listener_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_remote_listener_list_is_empty> List is null");
        itp_print_stack_trace(stdout);
        return 1;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    return (!list->first) ? 1 : 0;
}

void itp_clear_remote_listener_list(itp_remote_listener_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_clear_remote_listener_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (list->first) {
        itp_remote_listener_item_tp item = list->first;
        while (item) {
            itp_remote_listener_item_tp next = item->next;
            itp_free_remote_listener_item(item);
            item = next;
        }
        list->first = NULL;
    }
}

void itp_init_paired_buffer(itp_paired_buffer_tp buffer, itp_root_tp root, uint8_t address, itp_endpoint_tp endpoint) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_paired_buffer> Buffer is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    buffer->root = root;
    buffer->address = address;
    buffer->endpoint = endpoint;
    buffer->length = 0;
    buffer->lock_read = 0;
    buffer->lock_write = 0;
    buffer->next = NULL;
}

void itp_free_paired_buffer(itp_paired_buffer_tp buffer) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_free_paired_buffer> Buffer is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_free(buffer);
}

void itp_init_buffer_list(itp_buffer_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_buffer_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    list->first = NULL;
}

itp_error_code_t itp_emplace_paired_buffer(itp_buffer_list_tp list, itp_root_tp root, uint8_t address, itp_endpoint_tp endpoint) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_emplace_paired_buffer> List is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_emplace_paired_buffer> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_paired_buffer_tp last = list->first;
    if (!last) {
        itp_paired_buffer_tp buffer = itp_malloc(sizeof(itp_paired_buffer_t), "itp_emplace_paired_buffer:itp_paired_buffer_t");
        if (!buffer) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_emplace_paired_buffer> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        itp_init_paired_buffer(buffer, root, address, endpoint);
        list->first = buffer;
        return ITP_ERRC_NONE;
    }
    for (uint8_t count = 0;;) {
        if (last->address == address) {
            itp_debug_print(ITP_ERRC_WARNING, "itp_emplace_paired_buffer> Duplicate address %d", address);
            return ITP_ERRC_NONE;
        }
        if (++count < ITP_MAX_REMOTE_ENDPOINTS) {
            if (last->next) {
                last = last->next;
            } else break;
        } else {
            itp_debug_print(ITP_ERRC_OVERFLOW, "itp_emplace_paired_buffer> Buffer list overflow");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OVERFLOW;
        }
    }
    if (last) {
        itp_paired_buffer_tp buffer = itp_malloc(sizeof(itp_paired_buffer_t), "itp_emplace_paired_buffer:itp_paired_buffer_t");
        if (!buffer) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_emplace_paired_buffer> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        itp_init_paired_buffer(buffer, root, address, endpoint);
        last->next = buffer;
        return ITP_ERRC_NONE;
    } else {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_emplace_paired_buffer> Failed to add buffer to list");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
}

itp_paired_buffer_tp itp_erase_paired_buffer(itp_buffer_list_tp list, uint8_t address) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_erase_paired_buffer> List is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Проверяем первый элемент
    itp_paired_buffer_tp last = list->first;
    if (!last) {
        //itp_debug_print(ITP_ERRC_GENERAL, "itp_erase_paired_buffer> List is empty");
        return NULL;
    } else if (last->address == address) {
        list->first = last->next;
        itp_free_paired_buffer(last);
        return list->first;
    }

    // Проверяем остальные элементы
    for (;;) {
        itp_paired_buffer_tp next = last->next;
        if (next) {
            if (next->address == address) {
                last->next = next->next;
                itp_free_paired_buffer(next);
                return last->next;
            } else last = next;
        } else break;
    }
    return NULL;
}

itp_paired_buffer_tp itp_find_buffer(itp_buffer_list_tp list, uint8_t address) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_buffer> List is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Проверяем цепочку элементов
    itp_paired_buffer_tp last = list->first;
    while (last) {
        if (last->address == address) {
            return last;
        } else last = last->next;
    }
    return NULL;
}

uint8_t itp_buffer_list_is_empty(itp_buffer_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_buffer_list_is_empty> List is null");
        itp_print_stack_trace(stdout);
        return 1;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    return (!list->first) ? 1 : 0;
}

void itp_clear_buffer_list(itp_buffer_list_tp list) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!list) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_clear_buffer_list> List is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (list->first) {
        itp_paired_buffer_tp item = list->first;
        while (item) {
            itp_paired_buffer_tp next = item->next;
            itp_free_paired_buffer(item);
            item = next;
        }
        list->first = NULL;
    }
}
