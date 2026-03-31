#include <itp/debug.h>
#include <itp/handler.h>
#include <itp/callback.h>
#include <itp/endpoint.h>
#include <itp/package.h>
#include <itp/memory.h>
#include <itp/command.h>
#include <itp/array.h>
#include <itp/list.h>
#include <itp/root.h>
#include <itp/node.h>
#include <itp/symbol.h>
#include <itp/stack_trace.h>

#include <string.h>

void itp_init_handler_wrapper(
    itp_handler_wrapper_tp wrapper,
    void* handler,
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    uint8_t complex,
    uint8_t response
) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!wrapper) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_handler_wrapper> Wrapper is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    wrapper->handler = handler;
    #ifdef ITP_EXTEND_CALLBACKS
    wrapper->eparam = eparam;
    #endif // ITP_EXTEND_CALLBACKS
    wrapper->complex = complex;
    wrapper->response = response;
}

void itp_dispose_handler_wrapper(itp_handler_wrapper_tp wrapper) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!wrapper) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_clear_handler_wrapper> Wrapper is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (wrapper->complex) {
        if (wrapper->handler) {
            itp_free_handler((itp_handler_tp)wrapper->handler);
            wrapper->handler = NULL;
        }
    } else {
        itp_free_any_handler(
            wrapper->handler
            #ifdef ITP_EXTEND_CALLBACKS
            , wrapper->eparam
            #endif // ITP_EXTEND_CALLBACKS
        );
    }
}

itp_handler_tp itp_create_handler(uint8_t type) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    switch (type) {
        case ITP_HND_LOCAL:
        case ITP_HND_REMOTE:
        case ITP_HND_RECONNECT:
        case ITP_HND_HOST:
        case ITP_HND_PROXY:
        case ITP_HND_CHANNEL:
            break;
        default:
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_create_handler> Unknown type");
            itp_print_stack_trace(stdout);
            return NULL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_handler_tp handler = itp_malloc(sizeof(itp_handler_t), "itp_create_handler:itp_handler");
    if (!handler) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_create_handler> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return NULL;
    }
    handler->type = type;
    handler->address = 0;
    handler->node = NULL;
    handler->on_result = NULL;
    #ifdef ITP_EXTEND_CALLBACKS
    handler->on_result_eparam = 0;
    #endif // ITP_EXTEND_CALLBACKS
    handler->command = 0;
    handler->order = 0;
    return handler;
}

void itp_free_handler(itp_handler_tp handler) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!handler) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_free_handler> Handler is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_free_any_handler(
        (void*)handler->on_result
        #ifdef ITP_EXTEND_CALLBACKS
        , handler->on_result_eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
    itp_free(handler);
}

static void itp_handler_connect_local(itp_root_tp root, itp_node_tp node, uint16_t error, itp_on_connect_fp on_connect
#ifdef ITP_EXTEND_CALLBACKS
, size_t eparam
#endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!node) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_local> Node is null");
        itp_print_stack_trace(stdout);
        if (on_connect) on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, 0, ITP_ERR_INTERNAL
        );
        return;
    }
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_local> Root is null");
        itp_print_stack_trace(stdout);
        if (on_connect) on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, node->address, ITP_ERR_INTERNAL
        );
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    uint8_t node_address = node->address;
    if (!error) {
        if (!itp_pending_node_is_exist(&root->pending, node)) {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_local> Failed to remove pending node");
            itp_print_stack_trace(stdout);
            if (on_connect) on_connect(
                #ifdef ITP_EXTEND_CALLBACKS
                eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, node->address, ITP_ERR_INTERNAL
            );
            return;
        }
        itp_erase_listener_by_endpoint(&root->listeners, node->endpoint);
        itp_erase_pending_node(&root->pending, node);
        if (!itp_add_node_to_array(&root->children, node)) {
            if (on_connect) on_connect(
                #ifdef ITP_EXTEND_CALLBACKS
                eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, node->address, ITP_ERR_NONE
            );
        } else {
            itp_free_node(node);
            if (on_connect) on_connect(
                #ifdef ITP_EXTEND_CALLBACKS
                eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, node_address, ITP_ERR_INTERNAL
            );
        }
    } else {
        itp_erase_pending_node(&root->pending, node);
        itp_free_node(node);
        if (on_connect) on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, node_address, error
        );
    }
}

static void itp_handler_connect_remote(itp_root_tp root, itp_node_tp node, uint8_t address, uint16_t error, itp_on_connect_fp on_connect
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_remote> Root is null");
        itp_print_stack_trace(stdout);
        if (on_connect) on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, address, ITP_ERR_INTERNAL
        );
        return;
    }
    if (!node) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_remote> Node is null");
        itp_print_stack_trace(stdout);
        if (on_connect) on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, address, ITP_ERR_INTERNAL
        );
        return;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_remote> Wrong address");
        itp_print_stack_trace(stdout);
        if (on_connect) on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, address, ITP_ERR_INTERNAL
        );
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_remove_address_from_array(&root->suspend, address);
    if (!error) {
        if (itp_add_address_to_array(&node->subnodes, address)) {
            if (on_connect) on_connect(
                #ifdef ITP_EXTEND_CALLBACKS
                eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, address, ITP_ERR_INTERNAL
            );
        } else {
            if (on_connect) on_connect(
                #ifdef ITP_EXTEND_CALLBACKS
                eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, address, ITP_ERR_NONE
            );
        }
    } else {
        if (on_connect) on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, address, error
        );
    }
}

static void itp_handler_reconnect_node(itp_root_tp root, uint8_t address, uint16_t error, itp_on_connect_fp on_connect
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_reconnect_node> Root is null");
        itp_print_stack_trace(stdout);
        if (on_connect) on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, address, ITP_ERR_INTERNAL
        );
        return;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_reconnect_node> Wrong address");
        itp_print_stack_trace(stdout);
        if (on_connect) on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, address, ITP_ERR_INTERNAL
        );
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (on_connect) on_connect(
        #ifdef ITP_EXTEND_CALLBACKS
        eparam,
        #endif // ITP_EXTEND_CALLBACKS
        root, address, error
    );
}

static void itp_handler_connect_channel(itp_root_tp root, itp_node_tp node, uint8_t address, uint16_t error, itp_on_connect_endpoint_fp on_connect
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_channel> Root is null");
        itp_print_stack_trace(stdout);
        if (on_connect) on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, address, ITP_ERR_INTERNAL, NULL
        );
        return;
    }
    if (!node) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_channel> Node is null");
        itp_print_stack_trace(stdout);
        if (on_connect) on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, address, ITP_ERR_INTERNAL, NULL
        );
        return;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_channel> Wrong address");
        itp_print_stack_trace(stdout);
        if (on_connect) on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, address, ITP_ERR_INTERNAL, NULL
        );
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (!error) {
        if (on_connect) {
            itp_endpoint_tp endpoint = (itp_endpoint_tp)itp_malloc(sizeof(itp_endpoint_t), "itp_handler_connect_channel:itp_endpoint_t");
            if (!endpoint) {
                itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_handler_connect_channel> Memory allocation failed");
                error = ITP_ERR_MALLOC_FAILED;
            }
            itp_error_code_t errc = itp_emplace_paired_buffer(&root->buffers, root, address, NULL);
            if (errc) {
                itp_free(endpoint);
                endpoint = NULL;
                itp_debug_print(errc, "itp_handler_connect_channel> Failed to create buffer");
                error = ITP_ERR_INTERNAL;
            } else {
                errc = itp_init_endpoint(endpoint, &itp_remote_endpoint_read, &itp_remote_endpoint_write, itp_find_buffer(&root->buffers, address));
                if (errc) {
                    itp_erase_paired_buffer(&root->buffers, address);
                    itp_free(endpoint);
                    endpoint = NULL;
                    itp_debug_print(errc, "itp_handler_connect_channel> Failed to initialize endpoint");
                    error = ITP_ERR_INTERNAL;
                }
            }
            on_connect(
                #ifdef ITP_EXTEND_CALLBACKS
                eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, address, error, endpoint
            );
        }
    } else {
        if (on_connect) on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, address, error, NULL
        );
    }
}

static void itp_handler_connect_host(itp_root_tp root, itp_node_tp node, uint8_t address, uint16_t command, uint16_t order, uint16_t error) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_host> Root is null");
        itp_print_stack_trace(stdout);
        return;
    }
    if (!node) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_host> Node is null");
        itp_print_stack_trace(stdout);
        return;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_host> Wrong address");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (!error) {
        if (itp_pending_node_is_exist(&root->pending, node)) {
            itp_erase_listener_by_endpoint(&root->listeners, node->endpoint);
            itp_erase_pending_node(&root->pending, node);
            if (itp_add_node_to_array(&root->children, node)) {
                itp_free_node(node);
                error = ITP_ERR_INTERNAL;
            }
        } else {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_host> Failed to remove pending node");
            itp_print_stack_trace(stdout);
            error = ITP_ERR_INTERNAL;
        }
    }

    itp_error_code_t result;
    itp_package_tp package = itp_serialize_result(root->address, address, command, order, ITP_SYM_ACK, error, &result);
    if (package) {
        itp_node_tp node = root->parent;
        if (node) {
            result = itp_push_package(&node->queue_out, package);
            if (result) {
                itp_debug_print(ITP_ERRC_GENERAL, "itp_handler_connect_host> Failed to push package");
                itp_print_stack_trace(stdout);
                itp_free_package(package);
            }
        } else {
            itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_host> Host handler called, but no parent node");
            itp_print_stack_trace(stdout);
        }
    } else {
        itp_debug_print(result, "itp_handler_connect_host> Serialization failed");
        itp_print_stack_trace(stdout);
    }
}

static void itp_handler_connect_proxy(itp_root_tp root, itp_node_tp node, uint8_t address, uint16_t error) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_proxy> Root is null");
        itp_print_stack_trace(stdout);
        return;
    }
    if (!node) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_proxy> Node is null");
        itp_print_stack_trace(stdout);
        return;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_proxy> Wrong address");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_remove_address_from_array(&root->suspend, address);
    if (!error) {
        // 17.08.2020. Existing proxy node connect.
        if (!itp_is_array_contains(&node->subnodes, address)) {
            if (itp_add_address_to_array(&node->subnodes, address)) {
                itp_debug_print(ITP_ERRC_INTERNAL, "itp_handler_connect_proxy> Failed to add subnode to array");
                itp_print_stack_trace(stdout);
            }
        }
    }
}

void itp_call_handler(itp_handler_wrapper_tp wrapper, itp_root_tp root, uint16_t error, itp_frame_tp frame) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!wrapper) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_call_handler> Wrapper is null");
        itp_print_stack_trace(stdout);
        return;
    }
    if (!root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_call_handler> Root is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    if (!wrapper->handler) return;

    if (wrapper->complex) {
        itp_handler_tp handler = (itp_handler_tp)wrapper->handler;
        switch (handler->type) {
            case ITP_HND_LOCAL:
                itp_handler_connect_local(root, handler->node, error, (itp_on_connect_fp)handler->on_result
                    #ifdef ITP_EXTEND_CALLBACKS
                    , handler->on_result_eparam
                    #endif // ITP_EXTEND_CALLBACKS
                );
                break;
            case ITP_HND_REMOTE:
                itp_handler_connect_remote(root, handler->node, handler->address, error, (itp_on_connect_fp)handler->on_result
                    #ifdef ITP_EXTEND_CALLBACKS
                    , handler->on_result_eparam
                    #endif // ITP_EXTEND_CALLBACKS
                );
                break;
            case ITP_HND_RECONNECT:
                itp_handler_reconnect_node(root, handler->address, error, (itp_on_connect_fp)handler->on_result
                    #ifdef ITP_EXTEND_CALLBACKS
                    , handler->on_result_eparam
                    #endif // ITP_EXTEND_CALLBACKS
                );
                break;
            case ITP_HND_CHANNEL:
                itp_handler_connect_channel(root, handler->node, handler->address, error, (itp_on_connect_endpoint_fp)handler->on_result
                    #ifdef ITP_EXTEND_CALLBACKS
                    , handler->on_result_eparam
                    #endif // ITP_EXTEND_CALLBACKS
                );
                break;
            case ITP_HND_HOST:
                itp_handler_connect_host(root, handler->node, handler->address, handler->command, handler->order, error);
                break;
            case ITP_HND_PROXY:
                itp_handler_connect_proxy(root, handler->node, handler->address, error);
                break;
            default:
                itp_debug_print(ITP_ERRC_INTERNAL, "itp_call_handler> Unknown handler type");
                itp_print_stack_trace(stdout);
                break;
        }
    } else {
        //itp_debug_print(ITP_ERRC_INTERNAL, "itp_call_handler> Wrapped call of simple handler is prohibited");
        if (wrapper->response) {
            ((itp_on_response_fp)wrapper->handler)(
                #ifdef ITP_EXTEND_CALLBACKS
                wrapper->eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, error, frame
            );
        } else {
            ((itp_on_result_fp)wrapper->handler)(
                #ifdef ITP_EXTEND_CALLBACKS
                wrapper->eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, error
            );
        }
    }
}
