#include <itp/root.h>
#include <itp/debug.h>
#include <itp/node.h>
#include <itp/memory.h>
#include <itp/callback.h>
#include <itp/protocol.h>
#include <itp/endpoint.h>
#include <itp/command.h>
#include <itp/package.h>
#include <itp/frame.h>
#include <itp/stack_trace.h>

#include <string.h>
#include <inttypes.h>

void itp_on_connect_local_listener(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    itp_root_tp root,
    uint8_t address,
    uint16_t error
);

void itp_on_connect_remote_listener(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    itp_root_tp root,
    uint8_t address,
    uint16_t error
);

static itp_error_code_t itp_process_control(itp_root_tp root, itp_node_tp node, itp_frame_tp frame, itp_time_t time);

static void itp_send_custom_result(itp_root_tp root, uint8_t from, uint16_t command, uint16_t order, uint8_t status, uint16_t error) {
    itp_error_code_t result;
    itp_package_tp package = itp_serialize_result(root->address, from, command, order, status, error, &result);
    if (package) {
        itp_node_tp node = itp_find_node(root, from);
        if (node) {
            result = itp_push_package(&node->queue_out, package);
            if (result) {
                itp_free_package(package);
                itp_debug_print(ITP_ERRC_GENERAL, "itp_send_custom_result> Failed to push package");
                itp_print_stack_trace(stdout);
            }
        } else {
            itp_free_package(package);
            itp_debug_print(ITP_ERRC_ADDRESS_NOT_FOUND, "itp_send_custom_result> Node for address %d not found", from);
            itp_print_stack_trace(stdout);
        }
    } else {
        itp_debug_print(result, "itp_send_custom_result> Serialization failed");
        itp_print_stack_trace(stdout);
    }
}

static void itp_send_result(itp_root_tp root, itp_frame_tp frame, uint8_t status, uint16_t error) {
    itp_error_code_t result;
    // Make sure result contain same address as request
    itp_package_tp package = itp_serialize_result(/*root->address*/frame->to, frame->from, frame->command, frame->order, status, error, &result);
    if (package) {
        itp_node_tp node = itp_find_node(root, frame->from);
        if (node) {
            result = itp_push_package(&node->queue_out, package);
            if (result) {
                itp_free_package(package);
                itp_debug_print(ITP_ERRC_GENERAL, "itp_send_result> Failed to push package");
                itp_print_stack_trace(stdout);
            }
        } else {
            itp_free_package(package);
            itp_debug_print(ITP_ERRC_ADDRESS_NOT_FOUND, "itp_send_result> Node for address %d not found", frame->from);
            itp_print_stack_trace(stdout);
            //itp_dump_frame(frame);
        }
    } else {
        itp_debug_print(result, "itp_send_result> Serialization failed");
        itp_print_stack_trace(stdout);
    }
}

static itp_error_code_t itp_send_frame_to_node(itp_node_tp node, itp_frame_tp frame) {
    itp_error_code_t result;
    itp_package_tp package = itp_serialize_frame(frame, &result);
    if (package) {
        result = itp_push_package(&node->queue_out, package);
        if (result) {
            itp_debug_print(ITP_ERRC_GENERAL, "itp_send_frame> Failed to push package");
            itp_print_stack_trace(stdout);
            itp_free_package(package);
            return result;
        }
    } else {
        itp_debug_print(result, "itp_send_frame> Serialization failed");
        itp_print_stack_trace(stdout);
        return result;
    }
    return ITP_ERRC_NONE;
}

static itp_error_code_t itp_send_frame(itp_root_tp root, itp_frame_tp frame) {
    itp_node_tp node = itp_find_node(root, frame->to);
    if (!node) {
        itp_debug_print(ITP_ERRC_ADDRESS_NOT_FOUND, "itp_send_frame> Node for address %d not found", frame->from);
        itp_print_stack_trace(stdout);
        return ITP_ERRC_ADDRESS_NOT_FOUND;
    }
    return itp_send_frame_to_node(node, frame);
}

static itp_frame_item_tp itp_find_frame_by_command(itp_root_tp root, uint8_t status, uint16_t command) {
    itp_frame_item_tp last = root->frames.first;
    while (last) {
        #ifdef ITP_ENABLE_INTERNAL_CHECKS
        if (!last->frame) {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_frame_by_command> Null pointer found in frames list");
            itp_print_stack_trace(stdout);
            return NULL;
        }
        #endif // ITP_ENABLE_INTERNAL_CHECKS
        if (last->frame->status == status &&
            last->frame->command == command) {
            return last;
        } else last = last->next;
    }
    return NULL;
}

uint16_t itp_enumerate_frame(itp_root_tp root) {
    if (root->order >= 0xFFFF) {
        root->order = 0x0001;
    } else root->order++;
    return root->order;
}

itp_error_code_t itp_init_root(itp_root_tp root, itp_on_create_fp on_create
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
    ) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_init_root> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    root->address = 0;
    root->root = 0;
    root->order = 0;
    root->time = 0;
    root->parent = NULL;
    root->on_create = on_create;
    root->on_connect = NULL;
    root->on_reset = NULL;
    root->on_update_firmware = NULL;
    #ifdef ITP_EXTEND_CALLBACKS
    root->on_create_eparam = eparam;
    root->on_connect_eparam = 0;
    root->on_reset_eparam = 0;
    root->on_update_firmware_eparam = 0;
    #endif // ITP_EXTEND_CALLBACKS
    itp_init_node_array(&root->children);
    itp_init_listener_list(&root->listeners);
    itp_init_remote_listener_list(&root->remote_listeners);
    itp_init_pending_list(&root->pending);
    itp_init_handler_list(&root->handlers);
    itp_init_frame_list(&root->frames, ITP_MAX_PENDING_FRAMES);
    itp_init_address_array(&root->suspend);
    itp_init_proxy_list(&root->proxy);
    itp_init_buffer_list(&root->buffers);
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_set_parent(itp_root_tp root, itp_endpoint_tp endpoint, itp_on_error_fp on_error
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
    ) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_set_parent> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (root->parent) {
        itp_free_node(root->parent);
        root->parent = NULL;
    }

    // Set endpoint
    if (endpoint) {
        root->parent = itp_malloc(sizeof(itp_node_t), "itp_set_parent:itp_node");
        if (!root->parent) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_init_root> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
        itp_error_code_t errc = itp_init_node(root->parent, endpoint);
        if (errc) {
            itp_free(root->parent);
            root->parent = NULL;
            return errc;
        }
        root->parent->on_error = on_error;
        #ifdef ITP_EXTEND_CALLBACKS
        root->parent->on_error_eparam = eparam;
        #endif // ITP_EXTEND_CALLBACKS
    } else {
        itp_debug_print(ITP_ERRC_TRACE, "itp_init_root> Parent set to null");
    }
    return ITP_ERRC_NONE;
}

uint8_t itp_get_free_address(itp_root_tp root, itp_error_code_t* error) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_get_free_address> Root is null");
        itp_print_stack_trace(stdout);
        if (error) *error = ITP_ERRC_INTERNAL;
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Self address
    uint8_t all[0x100];
    memset(all, 0x0, sizeof(all));
    all[root->address] = 1;

    // Add child nodes
    if (root->children.count > 0) {
        uint8_t *ait, *aend;
        itp_node_tp* it = root->children.data;
        itp_node_tp* end = it + root->children.count;
        for (; it != end; ++it) {
            #ifdef ITP_ENABLE_INTERNAL_CHECKS
            if (!(*it)) {
                itp_debug_print(ITP_ERRC_INTERNAL, "itp_get_free_address> Null pointer found in nodes array");
                itp_print_stack_trace(stdout);
                if (error) *error = ITP_ERRC_INTERNAL;
                return 0;
            }
            #endif // ITP_ENABLE_INTERNAL_CHECKS
            all[(*it)->address] = 1;

            // Add child subnodes
            if ((*it)->subnodes.count > 0) {
                ait = (*it)->subnodes.data;
                aend = ait + (*it)->subnodes.count;
                for (; ait != aend; ++ait) {
                    #ifdef ITP_ENABLE_INTERNAL_CHECKS
                    if (!(*ait)) {
                        itp_debug_print(ITP_ERRC_INTERNAL, "itp_get_free_address> Null address found in subnodes array");
                        itp_print_stack_trace(stdout);
                        if (error) *error = ITP_ERRC_INTERNAL;
                        return 0;
                    }
                    #endif // ITP_ENABLE_INTERNAL_CHECKS
                    all[*ait] = 1;
                }
            }
        }
    }

    // Add pending nodes
    itp_pending_node_tp last = root->pending.first;
    while (last) {
        #ifdef ITP_ENABLE_INTERNAL_CHECKS
        if (!last->node) {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_get_free_address> Null node found in pending list");
            itp_print_stack_trace(stdout);
            if (error) *error = ITP_ERRC_INTERNAL;
            return 0;
        }
        #endif // ITP_ENABLE_INTERNAL_CHECKS
        all[last->node->address] = 1;
        last = last->next;
    }

    // Add suspend nodes
    if (root->suspend.count > 0) {
        uint8_t* it = root->suspend.data;
        uint8_t* end = it + root->suspend.count;
        for (; it != end; ++it) {
            #ifdef ITP_ENABLE_INTERNAL_CHECKS
            if (!(*it)) {
                itp_debug_print(ITP_ERRC_INTERNAL, "itp_get_free_address> Null address found in suspend array");
                itp_print_stack_trace(stdout);
                continue;
            }
            #endif // ITP_ENABLE_INTERNAL_CHECKS
            all[*it] = 1;
        }
    }

    // Get empty address
    for (uint16_t i = 1; i < 0x100; i++) {
        if (!all[i]) {
            if (error) *error = ITP_ERRC_NONE;
            return (uint8_t)i;
        }
    }
    if (error) *error = ITP_ERRC_ADDRESS_NOT_FOUND;
    return 0;
}

uint8_t itp_is_local_child(itp_root_tp root, uint8_t address) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_is_local_child> Root is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    for (uint8_t i = 0; i < root->children.count; ++i) {
        if (root->children.data[i]->address == address) {
            return 1;
        }
    }
    return 0;
}

static itp_error_code_t itp_connect_node(itp_root_tp root, itp_time_t time, itp_endpoint_tp endpoint, uint8_t address, itp_handler_tp handler) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_connect_node> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!endpoint) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_connect_node> Endpoint is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Выделяем память под структуру
    itp_node_tp node = itp_malloc(sizeof(itp_node_t), "itp_connect_node:itp_node");
    if (!node) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_connect_node> Node struct memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }

    itp_error_code_t error;
    error = itp_init_node(node, endpoint);
    if (error) {
        itp_free(node);
        return error;
    }
    node->address = address;

    // Добавляем во временный список
    error = itp_push_pending_node(&root->pending, node);
    if (error) return error;

    // Создаём запрос
    itp_frame_tp frame = itp_create_frame(ITP_CMD_SET_ADDRESS);
    if (!frame) return ITP_ERRC_MALLOC_FAILED;
    frame->status = ITP_SYM_CTR;
    frame->order = itp_enumerate_frame(root);
    frame->from = root->address;
    frame->to = 0; // Согласно протоколу
    if (root->root) {
        itp_frame_write_value_1b(frame, &root->root);
    } else itp_frame_write_value_1b(frame, &root->address);
    itp_frame_write_value_1b(frame, &address);

    // Добавляем в список запросов
    itp_frame_item_tp item = itp_malloc(sizeof(itp_frame_item_t), "itp_connect_node:itp_frame_item");
    if (!item) {
        itp_free_frame(frame);
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_connect_node> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    itp_init_frame_item(item, frame);
    item->sender = root->address;
    item->processed = 1;
    if (handler) handler->node = node;
    itp_init_handler_wrapper(
        &item->handler, (void*)handler,
        #ifdef ITP_EXTEND_CALLBACKS
        0,
        #endif // ITP_EXTEND_CALLBACKS
        1, 0
    );
    item->time = time;
    error = itp_push_frame_item(&root->frames, item);
    if (error) {
        itp_free(item);
        itp_debug_print(error, "itp_connect_node> Failed to push frame");
        itp_print_stack_trace(stdout);
        return error;
    }

    // Добавляем в очередь отправки
    error = itp_send_frame_to_node(node, frame);
    if (error) {
        itp_erase_frame_item(&root->frames, item->frame);
        return error;
    }
    return ITP_ERRC_NONE;
}

static itp_error_code_t itp_reconnect_node(itp_root_tp root, itp_node_tp node, itp_time_t time, itp_handler_tp handler) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_reconnect_node> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!node) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_reconnect_node> Node is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Создаём запрос
    itp_frame_tp frame = itp_create_frame(ITP_CMD_SET_ADDRESS);
    if (!frame) return ITP_ERRC_MALLOC_FAILED;
    frame->status = ITP_SYM_CTR;
    frame->order = itp_enumerate_frame(root);
    frame->from = root->address;
    frame->to = 0; // Согласно протоколу
    if (root->root) {
        itp_frame_write_value_1b(frame, &root->root);
    } else itp_frame_write_value_1b(frame, &root->address);
    itp_frame_write_value_1b(frame, &node->address);

    // Добавляем в список запросов
    itp_frame_item_tp item = itp_malloc(sizeof(itp_frame_item_t), "itp_reconnect_node:itp_frame_item");
    if (!item) {
        itp_free_frame(frame);
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_reconnect_node> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    itp_init_frame_item(item, frame);
    item->sender = root->address;
    item->processed = 1;
    if (handler) handler->node = node;
    itp_init_handler_wrapper(
        &item->handler, (void*)handler,
        #ifdef ITP_EXTEND_CALLBACKS
        0,
        #endif // ITP_EXTEND_CALLBACKS
        1, 0
    );
    item->time = time;
    itp_error_code_t error = itp_push_frame_item(&root->frames, item);
    if (error) {
        itp_free(item);
        itp_debug_print(error, "itp_reconnect_node> Failed to push frame");
        itp_print_stack_trace(stdout);
        return error;
    }

    // Добавляем в очередь отправки
    error = itp_send_frame_to_node(node, frame);
    if (error) {
        itp_erase_frame_item(&root->frames, item->frame);
        return error;
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_connect_local_node(itp_root_tp root, itp_time_t time, itp_endpoint_tp endpoint, uint8_t address, itp_on_connect_fp on_connect
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {
    itp_handler_tp handler = itp_create_handler(ITP_HND_LOCAL);
    if (!handler) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_connect_local_node> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    handler->address = address;
    handler->on_result = (void*)on_connect;
    #ifdef ITP_EXTEND_CALLBACKS
    handler->on_result_eparam = eparam;
    #endif // ITP_EXTEND_CALLBACKS
    itp_error_code_t error = itp_connect_node(root, time, endpoint, address, handler);
    if (error) itp_free(handler);
    return error;
}

itp_error_code_t itp_reconnect_local_node(itp_root_tp root, itp_time_t time, uint8_t address, itp_on_connect_fp on_connect
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {
    itp_node_tp node = itp_find_node(root, address);
    if (!node) {
        itp_debug_print(ITP_ERRC_BAD_ARGUMENT, "itp_reconnect_local_node> Child node with address %d not found", address);
        itp_print_stack_trace(stdout);
        return ITP_ERRC_BAD_ARGUMENT;
    }
    itp_handler_tp handler = itp_create_handler(ITP_HND_RECONNECT);
    if (!handler) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_reconnect_local_node> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    handler->address = address;
    handler->on_result = (void*)on_connect;
    #ifdef ITP_EXTEND_CALLBACKS
    handler->on_result_eparam = eparam;
    #endif // ITP_EXTEND_CALLBACKS
    itp_error_code_t error = itp_reconnect_node(root, node, time, handler);
    if (error) itp_free(handler);
    return error;
}

static itp_error_code_t itp_connect_remote_node_impl(itp_root_tp root, itp_time_t time, uint8_t host, uint8_t address, uint8_t mark, const char* path, itp_on_connect_fp on_connect
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
    ) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_connect_remote_node_impl> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Ищем нужный узел
    itp_node_tp node = itp_find_node(root, host);
    if (!node) {
        itp_debug_print(ITP_ERRC_ADDRESS_NOT_FOUND, "itp_connect_remote_node_impl> Node for address %d not found", host);
        itp_print_stack_trace(stdout);
        return ITP_ERRC_ADDRESS_NOT_FOUND;
    }

    // Создаём запрос
    itp_frame_tp frame = itp_create_frame(ITP_CMD_CREATE_NODE);
    frame->status = ITP_SYM_CTR;
    frame->order = itp_enumerate_frame(root);
    frame->from = root->address;
    frame->to = host;
    itp_frame_write_value_1b(frame, &address);
    if (!mark) {
        uint8_t type = ITP_CON_FORWARD;
        itp_frame_write_value_1b(frame, &type);
        itp_frame_write_string(frame, path);
    } else {
        uint8_t type = ITP_CON_REVERSE;
        itp_frame_write_value_1b(frame, &type);
        itp_frame_write_value_1b(frame, &mark);
    }

    // Добавляем в список запросов
    itp_frame_item_tp item = itp_malloc(sizeof(itp_frame_item_t), "itp_connect_remote_node_impl:itp_frame_item");
    if (!item) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_connect_remote_node_impl> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    itp_init_frame_item(item, frame);
    item->sender = root->address;

    // Создаём составной обработчик
    itp_handler_tp handler = itp_create_handler(ITP_HND_REMOTE);
    if (!handler) {
        itp_free(item);
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_connect_remote_node_impl> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    handler->address = address;
    handler->node = node;
    handler->on_result = (void*)on_connect;
    #ifdef ITP_EXTEND_CALLBACKS
    handler->on_result_eparam = eparam;
    #endif // ITP_EXTEND_CALLBACKS

    // Инициализируем структуры
    itp_init_handler_wrapper(
        &item->handler, (void*)handler,
        #ifdef ITP_EXTEND_CALLBACKS
        0,
        #endif // ITP_EXTEND_CALLBACKS
        1, 0
    );
    item->acknowledged = 1;
    item->processed = 1;
    item->time = time;
    itp_error_code_t error = itp_push_frame_item(&root->frames, item);
    if (error) {
        itp_free(item);
        itp_free_frame(frame);
        return error;
    }

    // Добавляем в очередь отправки
    error = itp_send_frame_to_node(node, frame);
    if (error) {
        itp_erase_frame_item(&root->frames, item->frame);
        return error;
    }

    // Добавляем адрес во временный список
    // Игнорируем результат операции
    itp_add_address_to_array(&root->suspend, address);
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_connect_remote_node(itp_root_tp root, itp_time_t time, uint8_t host, uint8_t address, const char* path, itp_on_connect_fp on_connect
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {
    return itp_connect_remote_node_impl(
        root, time, host, address, 0, path, on_connect
        #ifdef ITP_EXTEND_CALLBACKS
        , eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
}

static itp_error_code_t itp_connect_remote_listener(itp_root_tp root, itp_time_t time, uint8_t host, uint8_t address, uint8_t mark, itp_on_connect_fp on_connect
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {
    if (!mark) {
        itp_debug_print(ITP_ERRC_BAD_ARGUMENT, "itp_connect_remote_listener> Listener mark not found");
        return ITP_ERRC_BAD_ARGUMENT;
    }
    return itp_connect_remote_node_impl(
        root, time, host, address, mark, NULL, on_connect
        #ifdef ITP_EXTEND_CALLBACKS
        , eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
}

itp_error_code_t itp_connect_remote_endpoint(itp_root_tp root, itp_time_t time, uint8_t address, const char* path, itp_on_connect_endpoint_fp on_connect
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_connect_remote_endpoint> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Ищем нужный узел
    itp_node_tp node = itp_find_node(root, address);
    if (!node) {
        itp_debug_print(ITP_ERRC_ADDRESS_NOT_FOUND, "itp_connect_remote_endpoint> Node for address %d not found", address);
        itp_print_stack_trace(stdout);
        return ITP_ERRC_ADDRESS_NOT_FOUND;
    }

    // Создаём запрос
    itp_frame_tp frame = itp_create_frame(ITP_CMD_CREATE_NODE);
    frame->status = ITP_SYM_CTR;
    frame->order = itp_enumerate_frame(root);
    frame->from = root->address;
    frame->to = address;
    itp_frame_write_value_1b(frame, &address);
    uint8_t type = ITP_CON_CHANNEL;
    itp_frame_write_value_1b(frame, &type);
    itp_frame_write_string(frame, path);

    // Добавляем в список запросов
    itp_frame_item_tp item = itp_malloc(sizeof(itp_frame_item_t), "itp_connect_remote_endpoint:itp_frame_item");
    if (!item) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_connect_remote_endpoint> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    itp_init_frame_item(item, frame);
    item->sender = root->address;

    // Создаём составной обработчик
    itp_handler_tp handler = itp_create_handler(ITP_HND_CHANNEL);
    if (!handler) {
        itp_free(item);
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_connect_remote_endpoint> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    handler->address = address;
    handler->node = node;
    handler->on_result = (void*)on_connect;
    #ifdef ITP_EXTEND_CALLBACKS
    handler->on_result_eparam = eparam;
    #endif // ITP_EXTEND_CALLBACKS

    // Инициализируем структуры
    itp_init_handler_wrapper(
        &item->handler, (void*)handler,
        #ifdef ITP_EXTEND_CALLBACKS
        0,
        #endif // ITP_EXTEND_CALLBACKS
        1, 0
    );
    item->acknowledged = 1;
    item->processed = 1;
    item->time = time;
    itp_error_code_t error = itp_push_frame_item(&root->frames, item);
    if (error) {
        itp_free(item);
        itp_free_frame(frame);
        return error;
    }

    // Добавляем в очередь отправки
    error = itp_send_frame_to_node(node, frame);
    if (error) {
        itp_erase_frame_item(&root->frames, item->frame);
        return error;
    }

    // Добавляем адрес во временный список
    // Игнорируем результат операции
    itp_add_address_to_array(&root->suspend, address);
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_request_connect(itp_root_tp root, itp_time_t time, itp_on_result_fp on_result
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_request_connect> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (!root->parent) {
        itp_debug_print(ITP_ERRC_GENERAL, "itp_request_connect> Parent node not set");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_GENERAL;
    }

    { // Check handler
        itp_frame_item_tp request = itp_find_frame_by_command(root, ITP_SYM_CTR, ITP_CMD_REQUEST_CONNECT);
        if (request) {
            itp_debug_print(ITP_ERRC_GENERAL, "itp_request_connect> Connection request already queued");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_GENERAL;
        }
    }

    // Создаём запрос
    itp_frame_tp frame = itp_create_frame(ITP_CMD_REQUEST_CONNECT);
    frame->status = ITP_SYM_CTR;
    frame->order = itp_enumerate_frame(root);
    frame->from = 0;
    frame->to = 0;
    itp_byte_t value = 0;
    itp_frame_write_value_1b(frame, &value);
    itp_frame_write_value_1b(frame, &value);

    // Добавляем в список запросов
    itp_frame_item_tp item = itp_malloc(sizeof(itp_frame_item_t), "itp_request_connect:itp_frame_item");
    if (!item) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_request_connect> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    itp_init_frame_item(item, frame);
    item->sender = 0;

    // Создаём обработчик
    itp_init_handler_wrapper(
        &item->handler, (void*)on_result,
        #ifdef ITP_EXTEND_CALLBACKS
        eparam,
        #endif // ITP_EXTEND_CALLBACKS
        0, 0
    );
    item->acknowledged = 1;
    item->processed = 1;
    item->time = time;
    itp_error_code_t error = itp_push_frame_item(&root->frames, item);
    if (error) {
        itp_free(item);
        itp_free_frame(frame);
        return error;
    }

    // Добавляем в очередь отправки
    error = itp_send_frame_to_node(root->parent, frame);
    if (error) {
        itp_erase_frame_item(&root->frames, item->frame);
        return error;
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_signal_reset(itp_root_tp root, uint8_t address) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_signal_reset> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Ищем нужный узел
    itp_node_tp node = NULL;
    if (address == 0) {
        node = root->parent;
    } else if (root->parent &&
        root->parent->address == address) {
        node = root->parent;
    } else {
        node = itp_find_node(root, address);
    }
    if (!node) {
        itp_debug_print(ITP_ERRC_ADDRESS_NOT_FOUND, "itp_signal_reset> Node for address %d not found", address);
        itp_print_stack_trace(stdout);
        return ITP_ERRC_ADDRESS_NOT_FOUND;
    }

    // Создаём запрос
    itp_frame_tp frame = itp_create_frame(ITP_CMD_RESET);
    frame->status = ITP_SYM_CTR;
    frame->order = itp_enumerate_frame(root);
    frame->from = root->address;
    frame->to = address;

    // Добавляем в очередь отправки
    itp_error_code_t error = itp_send_frame_to_node(node, frame);
    itp_free_frame(frame);
    return error;
}

itp_error_code_t itp_signal_update_firmware(itp_root_tp root, uint8_t address, uint16_t major, uint16_t minor, uint32_t build) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_signal_update_firmware> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Ищем нужный узел
    itp_node_tp node = NULL;
    if (address == 0) {
        node = root->parent;
    } else if (root->parent &&
        root->parent->address == address) {
        node = root->parent;
    } else {
        node = itp_find_node(root, address);
    }
    if (!node) {
        itp_debug_print(ITP_ERRC_ADDRESS_NOT_FOUND, "itp_signal_update_firmware> Node for address %d not found", address);
        itp_print_stack_trace(stdout);
        return ITP_ERRC_ADDRESS_NOT_FOUND;
    }

    // Создаём запрос
    itp_frame_tp frame = itp_create_frame(ITP_CMD_UPDATE_FIRMWARE);
    frame->status = ITP_SYM_CTR;
    frame->order = itp_enumerate_frame(root);
    frame->from = root->address;
    frame->to = address;

    // Записываем версию прошивки
    itp_error_code_t result = itp_frame_write_value_2b(frame, &major);
    if (!result) result = itp_frame_write_value_2b(frame, &minor);
    if (!result) result = itp_frame_write_value_4b(frame, &build);
    if (result) return result;

    // Добавляем в очередь отправки
    result = itp_send_frame_to_node(node, frame);
    itp_free_frame(frame);
    return result;
}

itp_error_code_t itp_generate_update_firmware(itp_byte_t* buffer, itp_size_t* length, uint16_t major, uint16_t minor, uint32_t build, uint8_t use_checksum) {

    if (!buffer || !length) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_generate_update_firmware> Buffer is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }

    // Создаём запрос
    itp_frame_tp frame = itp_create_frame(ITP_CMD_UPDATE_FIRMWARE);
    frame->status = ITP_SYM_CTR;
    frame->order = 0;
    frame->from = 0;
    frame->to = 0;

    // Записываем версию прошивки
    itp_error_code_t result = itp_frame_write_value_2b(frame, &major);
    if (!result) result = itp_frame_write_value_2b(frame, &minor);
    if (!result) result = itp_frame_write_value_4b(frame, &build);
    if (result) return result;

    // Сериализуем кадр
    itp_package_tp package = itp_serialize_frame(frame, &result);
    itp_free_frame(frame);
    if (package) {

        // Копируем данные в буфер
        if (package->size > *length) {
            itp_debug_print(ITP_ERRC_OVERFLOW, "itp_generate_update_firmware> Insufficient buffer");
            itp_print_stack_trace(stdout);
            return result;
        }
        memcpy(buffer, package->data, package->size);
        *length = package->size;
        itp_free_package(package);
        return ITP_ERRC_NONE;

    } else {
        itp_debug_print(result, "itp_generate_update_firmware> Serialization failed");
        itp_print_stack_trace(stdout);
        return result;
    }
}

static itp_byte_t itp_get_listener_mark(itp_listener_list_tp listeners) {
    itp_byte_t range[0x100];
    memset(range, 0x0, sizeof(itp_byte_t) * 0x100);
    itp_listener_item_tp last = listeners->first;
    while (last) {
        range[last->mark] = last->mark;
        #ifdef ITP_ENABLE_INTERNAL_CHECKS
        if (!last->node) continue;
        #endif // ITP_ENABLE_INTERNAL_CHECKS
        last = last->next;
    }
    for (uint16_t i = 1; i < 0x100; ++i)
        if (!range[i]) return i;
    return 0;
}

// TODO set handler instead of root.on_connect
itp_error_code_t itp_set_connection_listener(itp_root_tp root, itp_endpoint_tp endpoint) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_set_connection_listener> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!endpoint) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_set_connection_listener> Endpoint is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    itp_node_tp node = itp_malloc(sizeof(itp_node_t), "itp_set_connection_listener:itp_node_t");
    if (!node) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_set_connection_listener> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    itp_error_code_t errc = itp_init_node(node, endpoint);
    if (errc) {
        itp_free(node);
        return errc;
    }
    errc = itp_push_listener_item(&root->listeners, itp_get_listener_mark(&root->listeners), node);
    if (errc) {
        itp_free_node(node);
    }
    return errc;
}

itp_node_tp itp_find_node(itp_root_tp root, uint8_t address) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_node> Root is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_node> Wrong address");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (root->children.count > 0) {
        itp_node_tp* it = root->children.data;
        itp_node_tp* end = it + root->children.count;
        for (; it != end; ++it) {

            #ifdef ITP_ENABLE_INTERNAL_CHECKS
            if (!(*it)) {
                itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_node> Null pointer found in nodes array");
                itp_print_stack_trace(stdout);
                return NULL;
            }
            #endif // ITP_ENABLE_INTERNAL_CHECKS

            if (!itp_resolve_address(*it, address)) {
                return *it;
            }
        }
    }
    return root->parent;
}

itp_frame_item_tp itp_find_frame(itp_root_tp root, itp_frame_tp frame) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_frame> Root is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    if (!frame) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_frame> Frame is null");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Выясняем, кто инициатор сообщения
    uint8_t origin = 0;
    if (frame->status == ITP_SYM_REQ ||
        frame->status == ITP_SYM_CTR) {
        origin = frame->from;
    } else origin = root->address;

    // Проверяем цепочку элементов
    itp_frame_item_tp last = root->frames.first;
    while (last) {
        #ifdef ITP_ENABLE_INTERNAL_CHECKS
        if (!last->frame) {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_find_frame> Null pointer found in frames list");
            itp_print_stack_trace(stdout);
            return NULL;
        }
        #endif // ITP_ENABLE_INTERNAL_CHECKS
        if (last->frame->from == origin &&
            last->frame->order == frame->order) {
            return last;
        } else last = last->next;
    }
    return NULL;
}

static itp_error_code_t itp_rewrite_listener_package(itp_root_tp root, itp_byte_t mark, itp_package_tp package) {
    itp_error_code_t errc = ITP_ERRC_NONE;
    itp_frame_tp frame = itp_parse_package(package, &errc);
    if (frame) {
        uint16_t result = ITP_ERR_NONE;
        if (frame->length != 2) {
            itp_debug_print(ITP_ERR_BAD_DATA, "itp_rewrite_listener_package> Bad data");
            result = ITP_ERR_BAD_DATA;
        } else {
            frame->data[0] = root->address;
            frame->data[1] = mark;
            itp_package_tp pack = itp_serialize_frame(frame, &errc);
            if (pack) {
                itp_byte_t* pd = package->data;
                itp_size_t ps = package->size;
                package->data = pack->data;
                package->size = pack->size;
                pack->data = pd;
                pack->size = ps;
                itp_free_package(pack);
                itp_debug_print(ITP_ERRC_TRACE, "itp_rewrite_listener_package> Added host address %d", root->address);
            } else {
                itp_debug_print(errc, "itp_rewrite_listener_package> Serialization failed");
                result = ITP_ERR_INTERNAL;
            }
        }
        if (result)
            itp_send_result(root, frame, ITP_SYM_ACK, result);
        itp_free_frame(frame);
    } else {
        itp_debug_print(ITP_ERRC_GENERAL, "itp_rewrite_listener_package> Unable to send CTR-NAK because of parsing error");
        itp_print_stack_trace(stdout);
    }
    return ITP_ERRC_NONE;
}

static itp_error_code_t itp_append_root_address(itp_root_tp root, itp_package_tp package) {
    itp_error_code_t error = ITP_ERRC_NONE;
    itp_frame_tp frame = itp_parse_package(package, &error);
    if (frame) {
        uint16_t result = ITP_ERR_NONE;
        itp_byte_t* array;
        uint16_t length = 0;
        if (!itp_frame_read_array_1b(frame, (void**)&array, &length)) {
            itp_byte_t* out = itp_malloc((length + 1) * sizeof(itp_byte_t), "itp_append_root_address:data");
            if (out) {
                memcpy(out, array, length);
                out[length] = root->address;
                itp_dispose_frame(frame);
                if (!itp_frame_write_array_1b(frame, out, length + 1)) {
                    itp_package_tp pack = itp_serialize_frame(frame, &error);
                    if (pack) {
                        itp_byte_t* pd = package->data;
                        itp_size_t ps = package->size;
                        package->data = pack->data;
                        package->size = pack->size;
                        pack->data = pd;
                        pack->size = ps;
                        itp_free_package(pack);
                    } else result = ITP_ERR_INTERNAL;
                } else result = ITP_ERR_INTERNAL;
                itp_free(out);
            } else result = ITP_ERR_INTERNAL;
            itp_free(array);
        } else result = ITP_ERR_BAD_DATA;
        if (result)
            itp_send_result(root, frame, ITP_SYM_ACK, result);
        itp_free_frame(frame);
    } else {
        itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> Unable to send CTR-NAK because of parsing error");
        itp_print_stack_trace(stdout);
    }
    return error;
}

static itp_node_tp itp_resolve_node(itp_root_tp root, itp_node_tp node, itp_package_tp package) {
    itp_error_code_t error;
    // Проверяем адреса дочерних узлов, если нужно передавать пакеты вниз
    if (root->children.count > 0) {
        itp_node_tp* it = root->children.data;
        itp_node_tp* end = it + root->children.count;
        for (; it != end; ++it) {
            #ifdef ITP_ENABLE_INTERNAL_CHECKS
            if (!(*it)) {
                itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_queue> Null pointer found in nodes array");
                itp_print_stack_trace(stdout);
                continue;
            }
            #endif // ITP_ENABLE_INTERNAL_CHECKS
            if (*it == node) continue;
            error = itp_resolve_address(*it, package->address);
            if (!error) return *it;
        }
    }
    // Передача пакета вверх по сети
    if (root->parent) return root->parent;
    return NULL;
}

static void itp_unlock_paired_buffer(itp_root_tp root, itp_frame_tp request) {
    if (request->command != ITP_CMD_REQUEST_DATA &&
        request->command != ITP_CMD_TRANSMIT_DATA) {
        return;
    }
    itp_paired_buffer_tp buffer = itp_find_buffer(&root->buffers, request->to);
    if (request->command == ITP_CMD_REQUEST_DATA) {
        if (buffer->lock_read) {
            buffer->lock_read = 0;
        }
    } else {
        if (buffer->lock_write) {
            buffer->lock_write = 0;
        }
    }
}

static itp_error_code_t itp_read_update_version(itp_frame_tp frame, uint16_t* major, uint16_t* minor, uint32_t* build) {
    itp_error_code_t errc;
    errc = itp_frame_read_value_2b(frame, major);
    if (errc) {
        itp_debug_print(errc, "itp_read_update_version> Failed to read major version");
        return errc;
    }
    errc = itp_frame_read_value_2b(frame, minor);
    if (errc) {
        itp_debug_print(errc, "itp_read_update_version> Failed to read minor version");
        return errc;
    }
    errc = itp_frame_read_value_4b(frame, build);
    if (errc) {
        itp_debug_print(errc, "itp_read_update_version> Failed to read build version");
        return errc;
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_process_queue(itp_root_tp root, itp_node_tp node, itp_byte_t listener, itp_time_t time) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_queue> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!node) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_queue> Node is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_error_code_t error = ITP_ERRC_NONE;
    while (!itp_package_queue_is_empty(&node->queue_in)) {
        itp_package_tp package = itp_pop_package(&node->queue_in);
        if (!package) {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_queue> Queue not empty, but pop operation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_INTERNAL;
        }
        if (package->address == root->address) {
            // Пакет адресован этому узлу
            itp_frame_tp frame = itp_parse_package(package, &error);
            if (frame) {
                itp_free_package(package);
                package = NULL;
                itp_frame_item_tp request;
                #ifdef ITP_ENABLE_INTERNAL_CHECKS
                FIND_REQUEST:
                #endif // ITP_ENABLE_INTERNAL_CHECKS
                request = itp_find_frame(root, frame);
                if (request) {

                    #ifdef ITP_ENABLE_INTERNAL_CHECKS
                    if (!request->frame) {
                        itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_queue> Null frame found in request");
                        itp_print_stack_trace(stdout);
                        itp_erase_frame_item(&root->frames, NULL);
                        goto FIND_REQUEST;
                    }
                    if (request->frame->status != ITP_SYM_REQ &&
                        request->frame->status != ITP_SYM_DAT &&
                        request->frame->status != ITP_SYM_CTR) {
                        itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_queue> Frame %d found in request", request->frame->status);
                        itp_print_stack_trace(stdout);
                        itp_erase_frame_item(&root->frames, request->frame);
                        goto FIND_REQUEST;
                    }
                    #endif // ITP_ENABLE_INTERNAL_CHECKS

                    switch (frame->status) {
                        case ITP_SYM_ACK:
                            if (request->frame->status == ITP_SYM_DAT) {
                                if (request->processed) {
                                    //itp_time_t delta = time - request->time;
                                    //itp_debug_print(ITP_ERRC_TRACE, "itp_process_queue> [%d -> %d] Received ACK on DAT in %" PRIu64 "ms", frame->from, root->address, delta);
                                    request->errors = 0;
                                    // Ответ был отправлен, цепочка завершена
                                    itp_call_handler(&request->handler, root, frame->error, frame);
                                    itp_erase_frame_item(&root->frames, request->frame);
                                } else {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> ACK on 0x%04x received, but DAT was not sent", frame->command);
                                    itp_print_stack_trace(stdout);
                                }
                            } else if (request->frame->status == ITP_SYM_REQ) {
                                if (!request->processed) {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> ACK on 0x%04x received, but REQ was not sent", frame->command);
                                    itp_print_stack_trace(stdout);
                                } else if (request->acknowledged) {
                                    // Игнорируем второе подтверждение
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> ACK on 0x%04x received, but already acknowledged", frame->command);
                                    itp_print_stack_trace(stdout);
                                } else {
                                    //itp_time_t delta = time - request->time;
                                    //itp_debug_print(ITP_ERRC_TRACE, "itp_process_queue> [%d -> %d] Received ACK on REQ in %" PRIu64 "ms", frame->from, root->address, delta);
                                    request->errors = 0;
                                    request->acknowledged = 1;
                                    request->time = time;
                                    // Подтверждение получено, ожидаем данные или удаляем запрос
                                    if (!request->wait_data || frame->error) {
                                        itp_call_handler(&request->handler, root, frame->error, frame);
                                        itp_erase_frame_item(&root->frames, request->frame);
                                    }
                                }
                            } else if (request->frame->status == ITP_SYM_CTR) {
                                if (request->processed) {
                                    //itp_time_t delta = time - request->time;
                                    //itp_debug_print(ITP_ERRC_TRACE, "itp_process_queue> [%d -> %d] Received ACK on CTR in %" PRIu64 "ms", frame->from, root->address, delta);
                                    // Пришло подтверждение на CTR команду
                                    if (!request->wait_data || frame->error) {
                                        itp_unlock_paired_buffer(root, request->frame);
                                        itp_call_handler(&request->handler, root, frame->error, frame);
                                        itp_erase_frame_item(&root->frames, request->frame);
                                        // После получения ACK есть риск, что слушатель будет удалён, поэтому прерываем цикл
                                        if (listener) {
                                            itp_free_frame(frame);
                                            return ITP_ERRC_NONE;
                                        }
                                    } else request->acknowledged = 1;
                                } else {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> ACK on 0x%04x received, but CTR was not sent", frame->command);
                                    itp_print_stack_trace(stdout);
                                }
                            }
                            itp_free_frame(frame);
                            break;
                        case ITP_SYM_NAK:
                            if (request->frame->status == ITP_SYM_DAT) {
                                if (request->processed) {
                                    if (++request->errors >= ITP_MAX_FRAME_ERRORS) {
                                        itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> NAK received %d times", ITP_MAX_FRAME_ERRORS);
                                        itp_call_handler(&request->handler, root, frame->error ? frame->error : ITP_ERR_TRANSMIT_FAILED, NULL);
                                        itp_erase_frame_item(&root->frames, request->frame);
                                    } else {
                                        if (frame->error == ITP_ERR_BUSY) {
                                            // Снимаем флаг отправки, если устройство занято. Это позволит отложить запрос на ITP_WAIT_BUSY
                                            request->processed = 0;
                                            request->time = time;
                                        } else {
                                            // Повторяем отправку, пока не превышено количество попыток
                                            if (itp_send_frame(root, request->frame)) {
                                                itp_erase_frame_item(&root->frames, request->frame);
                                            } else request->time = time;
                                        }
                                    }
                                } else {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> NAK received, but DAT was not sent");
                                    itp_print_stack_trace(stdout);
                                }
                            } else if (request->frame->status == ITP_SYM_REQ) {
                                if (!request->processed) {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> NAK received, but REQ was not sent");
                                    itp_print_stack_trace(stdout);
                                } else if (request->acknowledged) {
                                    itp_call_handler(&request->handler, root, frame->error ? frame->error : ITP_ERR_TRANSMIT_FAILED, NULL);
                                    itp_erase_frame_item(&root->frames, request->frame);
                                } else if (++request->errors >= ITP_MAX_FRAME_ERRORS) {
                                    itp_call_handler(&request->handler, root, frame->error ? frame->error : ITP_ERR_TRANSMIT_FAILED, NULL);
                                    itp_erase_frame_item(&root->frames, request->frame);
                                } else {
                                    if (frame->error == ITP_ERR_BUSY) {
                                        // Снимаем флаг отправки, если устройство занято. Это позволит отложить запрос на ITP_WAIT_BUSY
                                        request->processed = 0;
                                        request->time = time;
                                    } else {
                                        // Повторяем отправку, пока не превышено количество попыток
                                        if (itp_send_frame(root, request->frame)) {
                                            itp_erase_frame_item(&root->frames, request->frame);
                                        } else request->time = time;
                                    }
                                }
                            } else if (request->frame->status == ITP_SYM_CTR) {
                                if (request->processed) {
                                    // Пришла ошибка CTR команды
                                    itp_unlock_paired_buffer(root, request->frame);
                                    itp_call_handler(&request->handler, root, frame->error ? frame->error : ITP_ERR_TRANSMIT_FAILED, NULL);
                                    itp_erase_frame_item(&root->frames, request->frame);
                                } else {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> NAK received, but CTR was not sent");
                                    itp_print_stack_trace(stdout);
                                }
                            }
                            itp_free_frame(frame);
                            break;
                        case ITP_SYM_DAT:
                            if (request->frame->status == ITP_SYM_DAT) {
                                itp_free_frame(frame);
                                itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> DAT on 0x%04x received after DAT request", frame->command);
                                itp_print_stack_trace(stdout);
                                itp_call_handler(&request->handler, root, ITP_ERR_WRONG_SEQUENCE, NULL);
                                itp_erase_frame_item(&root->frames, request->frame);
                            } else if (request->frame->status == ITP_SYM_REQ) {
                                if (!request->processed) {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> DAT on 0x%04x received, but REQ was not sent", frame->command);
                                    itp_print_stack_trace(stdout);
                                } else if (request->acknowledged) {
                                    //itp_time_t delta = time - request->time;
                                    //itp_debug_print(ITP_ERRC_TRACE, "itp_process_queue> [%d -> %d] Received DAT on REQ in %" PRIu64 "ms", frame->from, root->address, delta);
                                    itp_call_handler(&request->handler, root, frame->error, frame);
                                    itp_send_result(root, frame, ITP_SYM_ACK, ITP_ERR_NONE);
                                    itp_erase_frame_item(&root->frames, request->frame);
                                    itp_free_frame(frame);
                                } else {
                                    itp_free_frame(frame);
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> DAT on 0x%04x received with no ACK", frame->command);
                                    itp_print_stack_trace(stdout);
                                    itp_send_result(root, frame, ITP_SYM_ACK, ITP_ERR_WRONG_SEQUENCE);
                                    itp_call_handler(&request->handler, root, ITP_ERR_WRONG_SEQUENCE, NULL);
                                    itp_erase_frame_item(&root->frames, request->frame);
                                }
                            } else if (request->frame->status == ITP_SYM_CTR) {
                                itp_unlock_paired_buffer(root, request->frame);
                                if (request->acknowledged) {
                                    itp_call_handler(&request->handler, root, ITP_ERR_NONE, frame);
                                    itp_send_result(root, frame, ITP_SYM_ACK, ITP_ERR_NONE);
                                    itp_free_frame(frame);
                                    itp_erase_frame_item(&root->frames, request->frame);
                                } else {
                                    itp_free_frame(frame);
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> DAT on 0x%04x received after CTR request with no ACK", frame->command);
                                    itp_print_stack_trace(stdout);
                                    itp_call_handler(&request->handler, root, ITP_ERR_WRONG_SEQUENCE, NULL);
                                    itp_erase_frame_item(&root->frames, request->frame);
                                }
                            }
                            break;
                        case ITP_SYM_CTR:
                            {
                                itp_free_frame(frame);
                                itp_send_result(root, request->frame, ITP_SYM_ACK, ITP_ERR_WRONG_SEQUENCE);
                                if (request->frame->status == ITP_SYM_DAT) {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> CTR received after DAT request");
                                } else if (request->frame->status == ITP_SYM_REQ) {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> CTR received after REQ request");
                                } else if (request->frame->status == ITP_SYM_CTR) {
                                    if (frame->command == ITP_CMD_SET_ADDRESS) {
                                        itp_call_handler(&request->handler, root, ITP_ERR_NONE, NULL);
                                        itp_erase_frame_item(&root->frames, request->frame);
                                        itp_free_package(package);
                                        return ITP_ERRC_NONE;
                                    } else {
                                        itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> CTR received after CTR request");
                                    }
                                } else {
                                    itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_queue> CTR received after 0x%02x request", request->frame->status);
                                }
                                itp_call_handler(&request->handler, root, ITP_ERR_WRONG_SEQUENCE, NULL);
                                itp_erase_frame_item(&root->frames, request->frame);
                            }
                            break;
                        case ITP_SYM_REQ:
                            {
                                itp_free_frame(frame);
                                itp_send_result(root, request->frame, ITP_SYM_ACK, ITP_ERR_WRONG_SEQUENCE);
                                if (request->frame->status == ITP_SYM_DAT) {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> REQ received after DAT request");
                                } else if (request->frame->status == ITP_SYM_REQ) {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> REQ received after REQ request");
                                } else if (request->frame->status == ITP_SYM_CTR) {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> REQ received after CTR request");
                                } else {
                                    itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_queue> REQ received after 0x%02x request", request->frame->status);
                                }
                                itp_call_handler(&request->handler, root, ITP_ERR_WRONG_SEQUENCE, NULL);
                                itp_erase_frame_item(&root->frames, request->frame);
                            }
                            break;
                        default:
                            {
                                itp_free_frame(frame);
                                itp_send_result(root, request->frame, ITP_SYM_ACK, ITP_ERR_WRONG_STATUS);
                                if (request->frame->status == ITP_SYM_DAT) {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> Unknown SYM received after DAT request");
                                } else if (request->frame->status == ITP_SYM_REQ) {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> Unknown SYM received after REQ request");
                                } else if (request->frame->status == ITP_SYM_CTR) {
                                    itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> Unknown SYM received after CTR request");
                                } else {
                                    itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_queue> Unknown SYM received after 0x%02x request", request->frame->status);
                                }
                                itp_call_handler(&request->handler, root, ITP_ERR_WRONG_STATUS, NULL);
                                itp_erase_frame_item(&root->frames, request->frame);
                            }
                            break;
                    }
                } else {
                    switch (frame->status) {
                        case ITP_SYM_REQ:
                            {
                                itp_handler_item_tp handler = itp_find_handler(&root->handlers, frame->from, frame->command);
                                if (handler) {
                                    #ifdef ITP_ENABLE_INTERNAL_CHECKS
                                    if (!handler->on_request) {
                                        itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_queue> Handler function pointer is null");
                                        itp_print_stack_trace(stdout);
                                        itp_send_result(root, frame, ITP_SYM_ACK, ITP_ERR_INTERNAL);
                                        itp_free_frame(frame);
                                        continue;
                                    }
                                    #endif // ITP_ENABLE_INTERNAL_CHECKS
                                    uint16_t result = handler->on_request(
                                        #ifdef ITP_EXTEND_CALLBACKS
                                        handler->on_request_eparam,
                                        #endif // ITP_EXTEND_CALLBACKS
                                        root, frame
                                    );
                                    itp_send_result(root, frame, ITP_SYM_ACK, result);
                                    itp_free_frame(frame);
                                } else {
                                    itp_debug_print(ITP_ERRC_WRONG_COMMAND, "itp_process_queue> Command 0x%04x handler not found on node %d, received from %d (node %d)", frame->command, (int)root->address, (int)frame->from, (int)node->address);
                                    itp_send_result(root, frame, ITP_SYM_ACK, ITP_ERR_WRONG_COMMAND);
                                    itp_free_frame(frame);
                                }
                            }
                            break;
                        case ITP_SYM_CTR:
                            // Обрабатываем управляющие пакеты прямо здесь
                            itp_process_control(root, node, frame, time);
                            itp_free_frame(frame);
                            break;
                        case ITP_SYM_DAT:
                            itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> DAT received, but request 0x%04x not found", frame->command);
                            itp_send_result(root, frame, ITP_SYM_ACK, ITP_ERR_WRONG_SEQUENCE);
                            itp_free_frame(frame);
                            break;
                        case ITP_SYM_ACK:
                            // Игнорируем пакеты, возможно был таймаут
                            itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> ACK received, but request 0x%04x not found (timeout?)", frame->command);
                            itp_free_frame(frame);
                            break;
                        case ITP_SYM_NAK:
                            // Игнорируем пакеты, возможно был таймаут
                            itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> NAK received, but request 0x%04x not found (timeout?)", frame->command);
                            itp_free_frame(frame);
                            break;
                        default:
                            itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> Frame with unknown status received");
                            itp_print_stack_trace(stdout);
                            itp_send_result(root, frame, ITP_SYM_ACK, ITP_ERR_WRONG_STATUS);
                            itp_free_frame(frame);
                            break;
                    }
                }
            } else {
                if (error == ITP_ERRC_CRC_FAILED) {
                    itp_send_custom_result(
                        root,
                        itp_get_package_from(package, NULL),
                        itp_get_package_command(package, NULL),
                        itp_get_package_order(package, NULL),
                        ITP_SYM_NAK,
                        ITP_ERR_CRC_FAILED
                    );
                } else {
                    itp_debug_print(error, "itp_process_queue> Failed to parse package");
                }
                itp_free_package(package);
                package = NULL;
            }
        } else {
            // Пакет адресован другому узлу
            switch (package->status) {
                // Конфигурирование сети
                case ITP_SYM_CTR:
                    {
                        uint16_t command = itp_get_package_command(package, &error);
                        switch (command) {
                            case ITP_CMD_CREATE_NODE:
                                {
                                    // Перехватываем пакет на создание узла
                                    itp_frame_tp frame = itp_parse_package(package, &error);
                                    if (frame) {
                                        uint8_t address;
                                        if (!itp_frame_read_value_1b(frame, &address)) {
                                            itp_frame_read_value_1b(frame, NULL);
                                            itp_handler_tp handler = itp_create_handler(ITP_HND_PROXY);
                                            if (!handler) {
                                                itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_process_queue> Memory allocation failed (command 0x02)");
                                                itp_print_stack_trace(stdout);
                                            } else {
                                                handler->node = itp_resolve_node(root, node, package);
                                                if (handler->node) {
                                                    handler->address = address;
                                                    if (!itp_push_proxy_item(&root->proxy, time, frame->from, frame->command, frame->order, handler)) {
                                                        // Добавляем адрес во временный список
                                                        itp_add_address_to_array(&root->suspend, address);
                                                    } else itp_free(handler);
                                                } else itp_free(handler);
                                            }
                                        } else {
                                            itp_send_result(root, frame, ITP_SYM_ACK, ITP_ERR_BAD_DATA);
                                            itp_free_package(package);
                                            package = NULL;
                                        }
                                        itp_free_frame(frame);
                                    } else {
                                        itp_debug_print(ITP_ERRC_PARSE_FAILED, "itp_process_queue> Unable to send NAK cause of parse error");
                                        itp_print_stack_trace(stdout);
                                        itp_free_package(package);
                                        package = NULL;
                                    }
                                }
                                break;
                            case ITP_CMD_TRACE_ROUTE:
                                if (itp_append_root_address(root, package)) {
                                    itp_free_package(package);
                                    package = NULL;
                                }
                                break;
                            case ITP_CMD_SET_ADDRESS:
                                {
                                    // Перехватываем пакет на задание адреса
                                    // По протоколу эта команда никогда не проксируется
                                    itp_frame_tp frame = itp_parse_package(package, &error);
                                    if (frame) {
                                        itp_process_control(root, node, frame, time);
                                        itp_free_frame(frame);
                                    } else {
                                        itp_debug_print(ITP_ERRC_PARSE_FAILED, "itp_process_queue> Unable to send NAK cause of parse error");
                                        itp_print_stack_trace(stdout);
                                        itp_free_package(package);
                                        package = NULL;
                                    }
                                    itp_free_package(package);
                                    package = NULL;
                                }
                                break;
                            case ITP_CMD_GET_CHILDREN:
                                break;
                            case ITP_CMD_RESET:
                                if (package->address == 0x00) {
                                    if (!root->parent ||
                                        root->parent == node) {
                                        itp_free_package(package);
                                        package = NULL;
                                        itp_debug_print(ITP_ERRC_TRACE, "itp_process_queue> Reset received");
                                        if (root->on_reset) {
                                            root->on_reset(
                                                #ifdef ITP_EXTEND_CALLBACKS
                                                root->on_reset_eparam,
                                                #endif // ITP_EXTEND_CALLBACKS
                                                root
                                            );
                                        }
                                    }
                                }
                                break;
                            case ITP_CMD_UPDATE_FIRMWARE:
                                if (listener || !package->address) {
                                    itp_frame_tp frame = itp_parse_package(package, &error);
                                    if (frame) {
                                        uint16_t major, minor;
                                        uint32_t build;
                                        error = itp_read_update_version(frame, &major, &minor, &build);
                                        if (!error) {
                                            itp_debug_print(ITP_ERRC_TRACE, "itp_process_queue> Update firmware received");
                                            if (root->on_update_firmware) {
                                                root->on_update_firmware(
                                                    #ifdef ITP_EXTEND_CALLBACKS
                                                    root->on_update_firmware_eparam,
                                                    #endif // ITP_EXTEND_CALLBACKS
                                                    root, major, minor, build
                                                );
                                            }
                                        } else {
                                            itp_debug_print(error, "itp_process_queue> Unable to rise update signal cause of read error");
                                        }
                                    } else {
                                        itp_debug_print(ITP_ERRC_PARSE_FAILED, "itp_process_queue> Unable to rise update signal cause of parse error");
                                    }
                                    itp_free_package(package);
                                    package = NULL;
                                }
                                break;
                            case ITP_CMD_LISTEN_REMOTE:
                                break;
                            case ITP_CMD_TRANSMIT_DATA:
                                break;
                            case ITP_CMD_REQUEST_DATA:
                                break;
                            case ITP_CMD_REQUEST_CONNECT:
                                if (listener) {
                                    if (!root->parent) {
                                        // Root node get connect request from listener
                                        uint8_t address = itp_get_free_address(root, &error);
                                        if (error) {
                                            itp_debug_print(error, "itp_process_queue> Failed to get free address");
                                            itp_free_package(package);
                                            return error;
                                        }
                                        error = itp_connect_local_node(
                                            root, time, node->endpoint, address, &itp_on_connect_local_listener
                                            #ifdef ITP_EXTEND_CALLBACKS
                                            , 0
                                            #endif // ITP_EXTEND_CALLBACKS
                                        );
                                        if (error) {
                                            itp_debug_print(error, "itp_process_queue> Failed to connect local node");
                                            itp_free_package(package);
                                            return error;
                                        } else {
                                            itp_debug_print(error, "itp_process_queue> Trying to connect node %d by listener request", (int)address);
                                        }
                                        itp_free_package(package);
                                        package = NULL;
                                    } else {
                                        // Non-root node get connect request from listener
                                        error = itp_rewrite_listener_package(root, listener, package);
                                        if (error) {
                                            itp_debug_print(error, "itp_process_queue> Failed to process listener request");
                                            itp_free_package(package);
                                            return error;
                                        }
                                    }
                                } else if (!root->parent) {
                                    // Parent node get connect request from network
                                    uint8_t host, mark;
                                    itp_frame_tp frame = itp_parse_package(package, &error);
                                    if (!error) error = itp_frame_read_value_1b(frame, &host);
                                    if (!error) error = itp_frame_read_value_1b(frame, &mark);
                                    itp_free_frame(frame);
                                    itp_free_package(package);
                                    package = NULL;
                                    if (error) {
                                        itp_debug_print(error, "itp_process_queue> Failed to parse frame");
                                        return error;
                                    }
                                    // Auto-reconnect
                                    if (node->address && !host) {
                                        itp_debug_print(ITP_ERRC_TRACE, "itp_process_queue> Node with valid address %d requested connect", node->address);
                                        itp_error_code_t errc = itp_reconnect_node(root, node, time, NULL);
                                        if (errc) {
                                            itp_debug_print(errc, "itp_process_queue> Failed to reconnect node");
                                        }
                                    } else {
                                        uint8_t address = itp_get_free_address(root, &error);
                                        if (error) {
                                            itp_debug_print(error, "itp_process_queue> Failed to get free address");
                                            return error;
                                        }
                                        error = itp_connect_remote_listener(
                                            root, time, host, address, mark, &itp_on_connect_remote_listener
                                            #ifdef ITP_EXTEND_CALLBACKS
                                            , 0
                                            #endif // ITP_EXTEND_CALLBACKS
                                        );
                                        if (error) {
                                            itp_debug_print(error, "itp_process_queue> Failed to connect remote node");
                                            return error;
                                        } else {
                                            itp_debug_print(error, "itp_process_queue> Trying to connect node %d by remote request", (int)address);
                                        }
                                    }
                                }
                                break;
                            case ITP_CMD_REQUEST_DISCONNECT:
                                break;
                            default:
                                itp_debug_print(ITP_ERRC_WARNING, "itp_process_queue> Unknown CTR command %d. New protocol version?", command);
                                itp_print_stack_trace(stdout);
                                break;
                        }
                    }
                    break;
                case ITP_SYM_ACK:
                    {
                        uint16_t command = itp_get_package_command(package, &error);
                        switch (command) {
                            case ITP_CMD_CREATE_NODE:
                                { // Ищем обработчик в списке, вызываем и удаляем
                                    uint16_t order = itp_get_package_order(package, &error);
                                    itp_proxy_item_tp proxy = itp_find_proxy(&root->proxy, package->address, order);
                                    if (proxy) {
                                        itp_call_handler(&proxy->handler, root, ITP_ERR_NONE, NULL);
                                        itp_erase_proxy_item(&root->proxy, proxy);
                                    }
                                }
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                case ITP_SYM_NAK:
                    {
                        uint16_t command = itp_get_package_command(package, &error);
                        switch (command) {
                            case ITP_CMD_CREATE_NODE:
                                { // Ищем обработчик в списке, вызываем и удаляем
                                    uint16_t order = itp_get_package_order(package, &error);
                                    itp_proxy_item_tp proxy = itp_find_proxy(&root->proxy, package->address, order);
                                    if (proxy) {
                                        itp_call_handler(&proxy->handler, root, ITP_ERR_UNKNOWN, NULL);
                                        itp_erase_proxy_item(&root->proxy, proxy);
                                    }
                                }
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                default:
                    break;
            }
            if (package) {
                itp_node_tp child = itp_resolve_node(root, node, package);
                if (child) {
                    error = itp_push_package(&child->queue_out, package);
                    if (error) itp_free_package(package);
                } else if (!root->address) {
                    itp_debug_print(ITP_ERRC_ADDRESS_NOT_FOUND, "itp_process_queue> Requested address %d, but node not initialized", package->address);
                    itp_debug_print(ITP_ERRC_TRACE, "itp_process_queue> Command 0x%04x", itp_get_package_command(package, NULL));
                    itp_frame_tp frame = itp_parse_package(package, &error);
                    if (frame) {
                        itp_signal_reset(root, 0);
                        itp_send_result(root, frame, ITP_SYM_NAK, ITP_ERR_BUSY);
                        itp_free_frame(frame);
                    } else {
                        itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> Unable to send NAK because of parsing error");
                        itp_print_stack_trace(stdout);
                    }
                    itp_free_package(package);
                } else {
                    itp_debug_print(ITP_ERRC_ADDRESS_NOT_FOUND, "itp_process_queue> Root node reached, but address %d not found", package->address);
                    itp_debug_print(ITP_ERRC_TRACE, "itp_process_queue> Command 0x%04x", itp_get_package_command(package, NULL));
                    itp_frame_tp frame = itp_parse_package(package, &error);
                    if (frame) {
                        itp_send_result(root, frame, ITP_SYM_ACK, ITP_ERR_WRONG_ADDRESS);
                        itp_free_frame(frame);
                    } else {
                        itp_debug_print(ITP_ERRC_GENERAL, "itp_process_queue> Unable to send NAK because of parsing error");
                        itp_print_stack_trace(stdout);
                    }
                    itp_free_package(package);
                }
                package = NULL;
            }
        }
        if (package) {
            itp_debug_print(ITP_ERRC_WARNING, "itp_process_queue> End of code reached, but package (adr %d) not deleted (root %d)", package->address, root->address);
            itp_print_stack_trace(stdout);
            itp_free_package(package);
        }
    }
    return ITP_ERRC_NONE;
}

void itp_process_frames(itp_root_tp root, itp_time_t time) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_frames> Root is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Проверяем цепочку элементов
    itp_frame_item_tp last = root->frames.first;
    while (last) {

        #ifdef ITP_ENABLE_INTERNAL_CHECKS
        if (!last->frame) {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_frames> Null pointer found in frames list");
            itp_print_stack_trace(stdout);
            last = itp_erase_frame_item(&root->frames, last->frame);
            continue;
        }
        #endif // ITP_ENABLE_INTERNAL_CHECKS

        if (last->frame->status == ITP_SYM_REQ) {
            if (last->processed) {
                // Проверяем таймауты
                if (last->acknowledged) {
                    if (last->time + ITP_WAIT_DATA < time) {

                        itp_debug_print(ITP_ERRC_TRACE, "itp_process_frames> REQ frame 0x%04x DAT timeout", last->frame->command);
                        itp_debug_print(ITP_ERRC_TRACE, "itp_process_frames> Node: %d; Address: %d; Delta: %" PRIu64 "ms", root->address, last->frame->to, time - last->time);

                        if (++last->errors >= 1) { // No repeats for acknowledged frames
                            itp_call_handler(&last->handler, root, ITP_ERR_TIMEOUT, NULL);
                            last = itp_erase_frame_item(&root->frames, last->frame);
                            continue;
                        } else {
                            // Повторяем отправку, пока не превышено количество попыток
                            itp_debug_print(ITP_ERRC_TRACE, "itp_process_frames> Repeat DAT frame");
                            if (itp_send_frame(root, last->frame)) {
                                last = itp_erase_frame_item(&root->frames, last->frame);
                                continue;
                            }
                            last->time = time;
                        }
                    }
                } else {
                    if (last->time + ITP_WAIT_ACKNOWLEDGE < time) {
                        if (++last->errors >= ITP_MAX_FRAME_ERRORS) {
                            itp_call_handler(&last->handler, root, ITP_ERR_TIMEOUT, NULL);
                            last = itp_erase_frame_item(&root->frames, last->frame);
                            continue;
                        } else {

                            // Повторяем отправку, пока не превышено количество попыток
                            itp_debug_print(ITP_ERRC_TRACE, "itp_process_frames> REQ frame 0x%04x ACK timeout, repeat", last->frame->command);
                            itp_debug_print(ITP_ERRC_TRACE, "itp_process_frames> Node: %d; Address: %d; Delta: %" PRIu64 "ms", root->address, last->frame->to, time - last->time);

                            if (itp_send_frame(root, last->frame)) {
                                last = itp_erase_frame_item(&root->frames, last->frame);
                                continue;
                            }
                            last->time = time;
                        }
                    }
                }
            } else {
                if (!last->time || last->time + ITP_WAIT_BUSY < time) {
                    itp_error_code_t error = itp_send_frame(root, last->frame);
                    if (error) {
                        itp_debug_print(error, "itp_process_frames> Failed to send REQ frame");
                        itp_print_stack_trace(stdout);
                        itp_call_handler(&last->handler, root, ITP_ERR_INTERNAL, NULL);
                        last = itp_erase_frame_item(&root->frames, last->frame);
                        continue;
                    } else {
                        last->errors = 0;
                        last->processed = 1;
                        last->time = time;
                    }
                }
            }
        } else if (last->frame->status == ITP_SYM_DAT) {
            if (last->processed) {
                if (last->time + ITP_WAIT_ACKNOWLEDGE < time) {
                    if (++last->errors >= ITP_MAX_FRAME_ERRORS) {
                        itp_call_handler(&last->handler, root, ITP_ERR_TIMEOUT, NULL);
                        last = itp_erase_frame_item(&root->frames, last->frame);
                        continue;
                    } else {
                        // Повторяем отправку, пока не превышено количество попыток
                        itp_debug_print(ITP_ERRC_TRACE, "itp_process_frames> DAT frame 0x%04x ACK timeout, repeat", last->frame->command);
                        itp_debug_print(ITP_ERRC_TRACE, "itp_process_frames> Node: %d; Address: %d; Delta: %" PRIu64 "ms", root->address, last->frame->to, time - last->time);
                        //itp_print_stack_trace(stdout);
                        if (itp_send_frame(root, last->frame)) {
                            last = itp_erase_frame_item(&root->frames, last->frame);
                            continue;
                        }
                        last->time = time;
                    }
                }
            } else {
                if (!last->time || last->time + ITP_WAIT_BUSY < time) {
                    itp_error_code_t error = itp_send_frame(root, last->frame);
                    if (error) {
                        itp_debug_print(error, "itp_process_frames> Failed to send DAT frame");
                        itp_print_stack_trace(stdout);
                        itp_call_handler(&last->handler, root, ITP_ERR_INTERNAL, NULL);
                        last = itp_erase_frame_item(&root->frames, last->frame);
                        continue;
                    } else {
                        last->errors = 0;
                        last->processed = 1;
                        last->time = time;
                    }
                }
            }
        } else if (last->frame->status == ITP_SYM_CTR) {
            if (last->processed) {
                // Проверяем таймауты
                if (last->acknowledged) {
                    if (last->time + ITP_WAIT_CONTROL < time) {
                        itp_call_handler(&last->handler, root, ITP_ERR_TIMEOUT, NULL);
                        last = itp_erase_frame_item(&root->frames, last->frame);
                        continue;
                    }
                } else {
                    if (last->time + ITP_WAIT_ACKNOWLEDGE < time) {
                        itp_call_handler(&last->handler, root, ITP_ERR_TIMEOUT, NULL);
                        last = itp_erase_frame_item(&root->frames, last->frame);
                        continue;
                    }
                }
            } else {
                if (last->frame->to) {
                    itp_error_code_t error = itp_send_frame(root, last->frame);
                    if (error) {
                        itp_debug_print(error, "itp_process_frames> Failed to send CTR frame");
                        itp_print_stack_trace(stdout);
                        itp_call_handler(&last->handler, root, ITP_ERR_INTERNAL, NULL);
                        last = itp_erase_frame_item(&root->frames, last->frame);
                        continue;
                    } else {
                        last->errors = 0;
                        last->processed = 1;
                        last->time = time;
                    }
                } else {
                    itp_debug_print(ITP_ERR_INTERNAL, "itp_process_frames> Failed to send CTR frame, wrong address");
                    itp_print_stack_trace(stdout);
                    itp_call_handler(&last->handler, root, ITP_ERR_INTERNAL, NULL);
                    last = itp_erase_frame_item(&root->frames, last->frame);
                    continue;
                }
            }
        } else {
            // В очереди не должно быть неизвестных кадров
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_frames> Unknown status in frames queue: %d", (int)last->frame->status);
            itp_print_stack_trace(stdout);
            last = itp_erase_frame_item(&root->frames, last->frame);
            continue;
        }

        last = last->next;
    }

    // Проверяем список перехваченных запросов
    itp_proxy_item_tp proxy = root->proxy.first;
    while (proxy) {
        if (proxy->time + ITP_WAIT_PROXY < time) {
            itp_call_handler(&proxy->handler, root, ITP_ERR_TIMEOUT, NULL);
            proxy = itp_erase_proxy_item(&root->proxy, proxy);
        } else proxy = proxy->next;
    }
}

static itp_error_code_t itp_get_children_list(itp_root_tp root, uint8_t** out_array, uint16_t* out_length) {
    uint16_t length = 0;
    if (root->children.count > 0) {
        itp_node_tp* it = root->children.data;
        itp_node_tp* end = it + root->children.count;
        for (; it != end; ++it) {
            #ifdef ITP_ENABLE_INTERNAL_CHECKS
            if (!(*it)) {
                itp_debug_print(ITP_ERRC_INTERNAL, "itp_get_children_list> Null pointer found in nodes array");
                itp_print_stack_trace(stdout);
                continue;
            }
            #endif // ITP_ENABLE_INTERNAL_CHECKS
            length += (*it)->subnodes.count + 1;
        }
    }
    *out_array = NULL;
    *out_length = 0;
    if (length > 0) {
        uint8_t* array = itp_malloc(length, "itp_get_children_list:data");
        if (array) {
            length = 0;
            itp_node_tp* it = root->children.data;
            itp_node_tp* end = it + root->children.count;
            for (; it != end; ++it) {
                #ifdef ITP_ENABLE_INTERNAL_CHECKS
                if (!(*it)) continue;
                #endif // ITP_ENABLE_INTERNAL_CHECKS
                array[length++] = (*it)->address;
                if ((*it)->subnodes.count > 0) {
                    uint8_t* ait = (*it)->subnodes.data;
                    uint8_t* aend = ait + (*it)->subnodes.count;
                    for (; ait != aend; ++ait) {
                        #ifdef ITP_ENABLE_INTERNAL_CHECKS
                        if (!(*ait)) continue;
                        #endif // ITP_ENABLE_INTERNAL_CHECKS
                        array[length++] = *ait;
                    }
                }
            }
            *out_array = array;
            *out_length = length;
        } else {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_get_children_list> Memory allocation failed");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_MALLOC_FAILED;
        }
    }
    return ITP_ERRC_NONE;
}

static itp_error_code_t itp_process_control(itp_root_tp root, itp_node_tp node, itp_frame_tp frame, itp_time_t time) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_control> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!node) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_control> Node is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!frame) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_process_control> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    switch (frame->command) {
        case ITP_CMD_SET_ADDRESS:
            {
                uint8_t address;
                uint16_t error = ITP_ERR_NONE;
                if (!itp_frame_read_value_1b(frame, &address)) {
                    root->root = address;
                } else error = ITP_ERR_BAD_DATA;
                if (!itp_frame_read_value_1b(frame, &address)) {
                    root->address = address;
                } else error = ITP_ERR_BAD_DATA;
                if (!error) {
                    node->address = frame->from;
                }
                { // Find handler
                    itp_frame_item_tp request = itp_find_frame_by_command(root, ITP_SYM_CTR, ITP_CMD_REQUEST_CONNECT);
                    if (request) {
                        itp_call_handler(&request->handler, root, error, NULL);
                        itp_erase_frame_item(&root->frames, request->frame);
                    }
                }
                itp_send_result(root, frame, ITP_SYM_ACK, error);
            }
            break;
        case ITP_CMD_CREATE_NODE:
            {
                uint16_t error = ITP_ERR_NONE;
                uint8_t address;
                if (!itp_frame_read_value_1b(frame, &address)) {
                    uint8_t type = 0;
                    itp_frame_read_value_1b(frame, &type);
                    if (type == ITP_CON_FORWARD ||
                        type == ITP_CON_CHANNEL) {
                        char* path = NULL;
                        if (!itp_frame_read_string(frame, &path)) {
                            if (root->on_create) {
                                itp_endpoint_tp endpoint = root->on_create(
                                    #ifdef ITP_EXTEND_CALLBACKS
                                    root->on_create_eparam,
                                    #endif // ITP_EXTEND_CALLBACKS
                                    path, &error
                                );
                                if (path) {
                                    itp_free(path);
                                    path = NULL;
                                }
                                if (endpoint) {
                                    if (type == ITP_CON_CHANNEL) {
                                        itp_error_code_t errc = itp_emplace_paired_buffer(&root->buffers, root, frame->from, endpoint);
                                        if (errc) {
                                            error = ITP_ERR_INTERNAL;
                                        } else error = ITP_ERR_NONE;
                                        itp_send_result(root, frame, ITP_SYM_ACK, error);
                                        return ITP_ERRC_NONE;
                                    } else {
                                        itp_handler_tp handler = itp_create_handler(ITP_HND_HOST);
                                        if (!handler) {
                                            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_process_control> Memory allocation failed");
                                            itp_print_stack_trace(stdout);
                                            error = ITP_ERR_INTERNAL;
                                        } else {
                                            handler->address = frame->from;
                                            handler->command = frame->command;
                                            handler->order = frame->order;
                                            if (!itp_connect_node(root, time, endpoint, address, handler)) {
                                                // Nothing need to do
                                            } else {
                                                itp_free(handler);
                                                error = ITP_ERR_INTERNAL;
                                            }
                                        }
                                    }
                                }
                            } else error = ITP_ERR_NOT_CONFIGURED;
                            if (path) itp_free(path);
                        } else error = ITP_ERR_BAD_DATA;
                    } else if (type == ITP_CON_REVERSE) {
                        uint8_t mark = 0;
                        if (!itp_frame_read_value_1b(frame, &mark)) {
                            itp_listener_item_tp listener = itp_find_listener(&root->listeners, mark);
                            if (listener) {
                                itp_endpoint_tp endpoint = listener->node->endpoint;
                                if (endpoint) {
                                    itp_handler_tp handler = itp_create_handler(ITP_HND_HOST);
                                    if (!handler) {
                                        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_process_control> Memory allocation failed");
                                        itp_print_stack_trace(stdout);
                                        error = ITP_ERR_INTERNAL;
                                    } else {
                                        handler->address = frame->from;
                                        handler->command = frame->command;
                                        handler->order = frame->order;
                                        if (!itp_connect_node(root, time, endpoint, address, handler)) {
                                            // Nothing need to do
                                        } else {
                                            itp_free(handler);
                                            error = ITP_ERR_INTERNAL;
                                        }
                                    }
                                }
                            } else error = ITP_ERR_NOT_CONFIGURED;
                        } else error = ITP_ERR_BAD_DATA;
                    } else {
                        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_process_control> Bad connection type 0x%02x", (int)type);
                        error = ITP_ERR_BAD_DATA;
                    }
                } else error = ITP_ERR_BAD_DATA;
                if (error) {
                    itp_send_result(root, frame, ITP_SYM_ACK, error);
                }
            }
            break;
        case ITP_CMD_GET_CHILDREN:
            {
                uint16_t error  = ITP_ERR_NONE;
                uint8_t* array = NULL;
                uint16_t length = 0;
                if (!itp_get_children_list(root, &array, &length)) {
                    itp_frame_tp response = itp_create_frame(ITP_CMD_GET_CHILDREN);
                    if (response) {
                        if (!itp_frame_write_array_1b(response, array, length)) {
                            if (itp_push_response(
                                    root, frame, response, NULL
                                    #ifdef ITP_EXTEND_CALLBACKS
                                    , 0
                                    #endif // ITP_EXTEND_CALLBACKS
                                )) {
                                itp_debug_print(ITP_ERRC_WARNING, "itp_process_control> Unable to send GET_CHILDREN DAT, push response failed");
                                itp_print_stack_trace(stdout);
                                itp_free_frame(response);
                                error = ITP_ERR_INTERNAL;
                            }
                        } else {
                            itp_debug_print(ITP_ERRC_WARNING, "itp_process_control> Unable to send GET_CHILDREN DAT, write array failed");
                            itp_print_stack_trace(stdout);
                            itp_free_frame(response);
                            error = ITP_ERR_INTERNAL;
                        }
                    } else error = ITP_ERR_INTERNAL;
                } else error = ITP_ERR_INTERNAL;
                if (array) itp_free(array);
                itp_send_result(root, frame, ITP_SYM_ACK, error);
            }
            break;
        case ITP_CMD_LISTEN_REMOTE:
            {
                uint16_t error = ITP_ERR_NONE;
                if (itp_frame_has_more(frame)) {
                    uint16_t* data = NULL;
                    uint16_t length = 0;
                    if (!itp_frame_read_array_2b(frame, (void**)&data, &length)) {
                        for (uint8_t i = 0; i < length; ++i) {
                            if (itp_push_remote_listener_item(&root->remote_listeners, frame->from, data[i])) {
                                error = ITP_ERR_INTERNAL;
                                break;
                            } else {
                                itp_debug_print(ITP_ERRC_TRACE, "itp_process_control> Node %d now listen 0x%04x event from %d", (int)frame->from, data[i], root->address);
                            }
                        }
                        itp_free(data);
                    } else error = ITP_ERR_BAD_DATA;
                } else {
                    if (itp_push_remote_listener_item(&root->remote_listeners, frame->from, 0)) {
                        error = ITP_ERR_INTERNAL;
                    } else {
                        itp_debug_print(ITP_ERRC_TRACE, "itp_process_control> Node %d now listen all events from %d", (int)frame->from, root->address);
                    }
                }
                itp_send_result(root, frame, ITP_SYM_ACK, error);
            }
            break;
        case ITP_CMD_TRACE_ROUTE:
            {
                itp_byte_t* array;
                uint16_t length = 0;
                uint16_t error = ITP_ERR_NONE;
                if (!itp_frame_read_array_1b(frame, (void**)&array, &length)) {
                    itp_byte_t* out = itp_malloc((length + 1) * sizeof(itp_byte_t), "itp_process_control:data:trace_route");
                    if (out) {
                        memcpy(out, array, length);
                        out[length] = root->address;
                        itp_frame_tp response = itp_create_frame(ITP_CMD_TRACE_ROUTE);
                        if (response) {
                            if (!itp_frame_write_array_1b(response, out, length + 1)) {
                                if (itp_push_response(
                                        root, frame, response, NULL
                                        #ifdef ITP_EXTEND_CALLBACKS
                                        , 0
                                        #endif // ITP_EXTEND_CALLBACKS
                                    )) {
                                    itp_debug_print(ITP_ERRC_WARNING, "itp_process_control> Unable to send TRACE_ROUTE DAT, push response failed");
                                    itp_print_stack_trace(stdout);
                                    itp_free_frame(response);
                                    error = ITP_ERR_INTERNAL;
                                }
                            } else {
                                itp_debug_print(ITP_ERRC_WARNING, "itp_process_control> Unable to send TRACE_ROUTE DAT, write array failed");
                                itp_print_stack_trace(stdout);
                                itp_free_frame(response);
                                error = ITP_ERR_INTERNAL;
                            }
                        } else error = ITP_ERR_INTERNAL;
                        itp_free(out);
                    } else error = ITP_ERR_INTERNAL;
                    itp_free(array);
                } else error = ITP_ERR_INTERNAL;
                itp_send_result(root, frame, ITP_SYM_ACK, error);
            }
            break;
        case ITP_CMD_RESET:
            itp_debug_print(ITP_ERRC_TRACE, "itp_process_control> Reset received");
            if (root->on_reset) {
                root->on_reset(
                    #ifdef ITP_EXTEND_CALLBACKS
                    root->on_reset_eparam,
                    #endif // ITP_EXTEND_CALLBACKS
                    root
                );
            }
            break;
        case ITP_CMD_UPDATE_FIRMWARE:
            {
                uint16_t major, minor;
                uint32_t build;
                itp_error_code_t errc = itp_read_update_version(frame, &major, &minor, &build);
                if (!errc) {
                    itp_debug_print(ITP_ERRC_TRACE, "itp_process_control> Update firmware received");
                    if (root->on_update_firmware) {
                        root->on_update_firmware(
                            #ifdef ITP_EXTEND_CALLBACKS
                            root->on_update_firmware_eparam,
                            #endif // ITP_EXTEND_CALLBACKS
                            root, major, minor, build
                        );
                    }
                } else {
                    itp_debug_print(errc, "itp_process_control> Update firmware failed cause of read error");
                }
            }
            break;
        case ITP_CMD_TRANSMIT_DATA:
            {
                uint16_t error = ITP_ERR_NONE;
                itp_paired_buffer_tp buffer = itp_find_buffer(&root->buffers, frame->from);
                if (buffer) {
                    if (buffer->endpoint) {
                        // Slave
                        if (buffer->endpoint->write(buffer->endpoint->object, frame->data, frame->length) != frame->length) {
                            itp_debug_print(ITP_ERRC_WARNING, "itp_process_control> Not all data was written (Slave)");
                        }
                    } else {
                        // Master
                        if (frame->length > 0) {
                            itp_size_t bytes = ITP_PAIRED_BUFFER_SIZE - buffer->length;
                            if (bytes < frame->length) {
                                itp_debug_print(ITP_ERRC_WARNING, "itp_process_control> Not all data was written (Master)");
                            } else bytes = frame->length;
                            memcpy(buffer->data, frame->data, bytes);
                            buffer->length += bytes;
                        }
                    }
                } else {
                    itp_debug_print(ITP_ERRC_WARNING, "itp_process_control> Was received binary data, but no buffer found");
                    error = ITP_ERR_NOT_CONFIGURED;
                }
                itp_send_result(root, frame, ITP_SYM_ACK, error);
            }
            break;
        case ITP_CMD_REQUEST_DATA:
            {
                uint16_t error = ITP_ERR_NONE;
                itp_paired_buffer_tp buffer = itp_find_buffer(&root->buffers, frame->from);
                if (buffer) {
                    if (buffer->endpoint) {
                        // Slave
                        itp_size_t length = buffer->endpoint->read(buffer->endpoint->object, buffer->data + buffer->length, ITP_PAIRED_BUFFER_SIZE - buffer->length);
                        buffer->length += length;
                        itp_frame_tp response = itp_create_frame(ITP_CMD_REQUEST_DATA);
                        if (response) {
                            if (buffer->length > 0) {
                                if (!itp_prepare_frame(response, buffer->length)) {
                                    memcpy(response->data, buffer->data, buffer->length);
                                } else {
                                    itp_debug_print(ITP_ERRC_WARNING, "itp_process_control> Unable to send REQUEST_DATA DAT, prepare frame failed");
                                    itp_free_frame(response);
                                    response = NULL;
                                    error = ITP_ERR_INTERNAL;
                                }
                            }
                            if (response) {
                                if (itp_push_response(
                                        root, frame, response, NULL
                                        #ifdef ITP_EXTEND_CALLBACKS
                                        , 0
                                        #endif // ITP_EXTEND_CALLBACKS
                                    )) {
                                    itp_debug_print(ITP_ERRC_WARNING, "itp_process_control> Unable to send REQUEST_DATA DAT, push response failed");
                                    itp_free_frame(response);
                                    error = ITP_ERR_INTERNAL;
                                } else {
                                    if (buffer->length > 0) {
                                        buffer->length = 0;
                                    }
                                }
                            }
                        } else error = ITP_ERR_INTERNAL;
                    } else {
                        // Master
                        itp_frame_tp response = itp_create_frame(ITP_CMD_REQUEST_DATA);
                        if (response) {
                            if (itp_push_response(
                                    root, frame, response, NULL
                                    #ifdef ITP_EXTEND_CALLBACKS
                                    , 0
                                    #endif // ITP_EXTEND_CALLBACKS
                                )) {
                                itp_debug_print(ITP_ERRC_WARNING, "itp_process_control> Unable to send REQUEST_DATA DAT, push response failed");
                                itp_free_frame(response);
                                error = ITP_ERR_INTERNAL;
                            }
                        } else error = ITP_ERR_INTERNAL;
                    }
                } else {
                    itp_debug_print(ITP_ERRC_WARNING, "itp_process_control> Was received binary data, but no buffer found");
                    error = ITP_ERR_NOT_CONFIGURED;
                }
                itp_send_result(root, frame, ITP_SYM_ACK, error);
            }
            break;
        default:
            break;
    }
    return ITP_ERRC_NONE;
}

void itp_poll_root(itp_root_tp root, itp_time_t time) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_poll_root> Root is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (root->time != time) {
        root->time = time;
    }

    itp_error_code_t error = ITP_ERRC_NONE;

    // Poll pending nodes
    itp_pending_node_tp last = root->pending.first;
    while (last) {
        itp_node_tp node = last->node;
        #ifdef ITP_ENABLE_INTERNAL_CHECKS
        if (!node) {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_poll_root> Null node found in pending list");
            itp_print_stack_trace(stdout);
            last = itp_erase_pending_node(&root->pending, node);
            continue;
        }
        #endif // ITP_ENABLE_INTERNAL_CHECKS
        last = last->next;
        error = itp_poll_node(
            #ifdef ITP_TRACE_BUFFERS
            root->address,
            #endif // ITP_TRACE_BUFFERS
            node, time
        );
        if (error && node->on_error) {
            node->on_error(
                #ifdef ITP_EXTEND_CALLBACKS
                node->on_error_eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, node->address, error
            );
        }
        error = itp_process_queue(root, node, 0, time);
        if (error && node->on_error) {
            node->on_error(
                #ifdef ITP_EXTEND_CALLBACKS
                node->on_error_eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, node->address, error
            );
        }
    }

    // Poll parent node
    if (root->parent) {
        error = itp_poll_node(
            #ifdef ITP_TRACE_BUFFERS
            root->address,
            #endif // ITP_TRACE_BUFFERS
            root->parent, time
        );
        if (error && root->parent->on_error) {
            root->parent->on_error(
                #ifdef ITP_EXTEND_CALLBACKS
                root->parent->on_error_eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, root->parent->address, error
            );
        }
        error = itp_process_queue(root, root->parent, 0, time);
        if (error && root->parent->on_error) {
            root->parent->on_error(
                #ifdef ITP_EXTEND_CALLBACKS
                root->parent->on_error_eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, root->parent->address, error
            );
        }
    }

    // Poll child nodes
    if (root->children.count > 0) {
        itp_node_tp* it = root->children.data;
        itp_node_tp* end = it + root->children.count;
        for (; it != end; ++it) {
            #ifdef ITP_ENABLE_INTERNAL_CHECKS
            if (!(*it)) {
                itp_debug_print(ITP_ERRC_INTERNAL, "itp_poll_root> Null pointer found in nodes array");
                itp_print_stack_trace(stdout);
                return;
            }
            #endif // ITP_ENABLE_INTERNAL_CHECKS
            error = itp_poll_node(
                #ifdef ITP_TRACE_BUFFERS
                root->address,
                #endif // ITP_TRACE_BUFFERS
                *it, time
            );
            if (error && (*it)->on_error) {
                (*it)->on_error(
                    #ifdef ITP_EXTEND_CALLBACKS
                    (*it)->on_error_eparam,
                    #endif // ITP_EXTEND_CALLBACKS
                    root, (*it)->address, error
                );
            }
            error = itp_process_queue(root, *it, 0, time);
            if (error && (*it)->on_error) {
                (*it)->on_error(
                    #ifdef ITP_EXTEND_CALLBACKS
                    (*it)->on_error_eparam,
                    #endif // ITP_EXTEND_CALLBACKS
                    root, (*it)->address, error
                );
            }
        }
    }

    { // Poll listeners
        itp_listener_item_tp last = root->listeners.first;
        while (last) {
            itp_node_tp node = last->node;
            itp_byte_t mark = last->mark;
            #ifdef ITP_ENABLE_INTERNAL_CHECKS
            if (!node) {
                itp_debug_print(ITP_ERRC_INTERNAL, "itp_poll_root> Null node found in listeners list");
                itp_print_stack_trace(stdout);
                last = itp_erase_listener_by_node(&root->listeners, node);
                continue;
            }
            #endif // ITP_ENABLE_INTERNAL_CHECKS
            last = last->next;
            error = itp_poll_node(
                #ifdef ITP_TRACE_BUFFERS
                root->address,
                #endif // ITP_TRACE_BUFFERS
                node, time
            );
            if (error && node->on_error) {
                node->on_error(
                    #ifdef ITP_EXTEND_CALLBACKS
                    node->on_error_eparam,
                    #endif // ITP_EXTEND_CALLBACKS
                    root, node->address, error
                );
            }
            error = itp_process_queue(root, node, mark, time);
            if (error && node->on_error) {
                node->on_error(
                    #ifdef ITP_EXTEND_CALLBACKS
                    node->on_error_eparam,
                    #endif // ITP_EXTEND_CALLBACKS
                    root, node->address, error
                );
            }
        }
    }

    // Poll frames
    itp_process_frames(root, time);
}

itp_root_tp itp_create_root(itp_on_create_fp on_create
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
) {
    itp_root_tp root = (itp_root_tp)itp_malloc(sizeof(itp_root_t), "itp_create_root:itp_root");
    if (!root) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_create_root> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    itp_init_root(
        root, on_create
        #ifdef ITP_EXTEND_CALLBACKS
        , eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
    return root;
}

void itp_dispose_root(itp_root_tp root) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_dispose_root> Root is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (root->parent)
        itp_free_node(root->parent);
    itp_cancel_all_requests(root);
    itp_clear_node_array(&root->children);
    itp_clear_listener_list(&root->listeners);
    itp_clear_remote_listener_list(&root->remote_listeners);
    itp_clear_pending_list(&root->pending);
    itp_clear_handler_list(&root->handlers);
    itp_clear_frame_list(&root->frames);
    itp_clear_address_array(&root->suspend);
    itp_clear_proxy_list(&root->proxy);
    itp_clear_buffer_list(&root->buffers);
    itp_free_any_handler(
        (void*)root->on_create
        #ifdef ITP_EXTEND_CALLBACKS
        , root->on_create_eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
    itp_free_any_handler(
        (void*)root->on_connect
        #ifdef ITP_EXTEND_CALLBACKS
        , root->on_connect_eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
    itp_free_any_handler(
        (void*)root->on_reset
        #ifdef ITP_EXTEND_CALLBACKS
        , root->on_reset_eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
    itp_free_any_handler(
        (void*)root->on_update_firmware
        #ifdef ITP_EXTEND_CALLBACKS
        , root->on_update_firmware_eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
}

void itp_free_root(itp_root_tp root) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_free_root> Root is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    itp_dispose_root(root);
    itp_free(root);
}
