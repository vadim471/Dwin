#include <itp/debug.h>
#include <itp/command.h>
#include <itp/memory.h>
#include <itp/frame.h>
#include <itp/root.h>
#include <itp/stack_trace.h>

#include <string.h>

#ifdef ITP_ENABLE_REQUEST_COMPARISON
static uint8_t itp_compare_frames(itp_frame_list_tp list, itp_frame_tp frame) {
    itp_frame_item_tp last = list->first;
    itp_frame_tp current;
    while (last) {
        current = last->frame;
        last = last->next;
        if (current) {
            if (current->command != frame->command) continue;
            if (current->status != frame->status) continue;
            if (current->from != frame->from) continue;
            if (current->to != frame->to) continue;
            if (current->length != frame->length) continue;
            if (current->data && frame->data) {
                if (memcmp(current->data, frame->data, current->length) == 0) {
                    return 1;
                } else continue;
            }
            return 1;
        }
    }
    return 0;
}
#endif // ITP_ENABLE_REQUEST_COMPARISON

itp_error_code_t itp_push_notification(itp_root_tp root, itp_frame_tp frame, uint8_t address, itp_on_result_fp on_result
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_push_notification> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_push_notification> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_push_notification> Wrong address");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Fill frame
    frame->status = ITP_SYM_REQ;
    frame->from = root->address;
    frame->to = address;

    #ifdef ITP_ENABLE_REQUEST_COMPARISON
    if (itp_compare_frames(&root->frames, frame)) {
        itp_free_frame(frame);
        if (on_result) {
            on_result(
                #ifdef ITP_EXTEND_CALLBACKS
                eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, ITP_ERR_IGNORED
            );
        }
        return ITP_ERRC_NONE;
    }
    #endif // ITP_ENABLE_REQUEST_COMPARISON

    frame->order = itp_enumerate_frame(root);

    // Push to list
    itp_frame_item_tp item = itp_malloc(sizeof(itp_frame_item_t), "itp_push_notification:itp_frame_item");
    if (!item) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_notification> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    itp_init_frame_item(item, frame);
    itp_init_handler_wrapper(
        &item->handler, (void*)on_result,
        #ifdef ITP_EXTEND_CALLBACKS
        eparam,
        #endif // ITP_EXTEND_CALLBACKS
        0, 0
    );
    item->sender = root->address;
    item->wait_data = 0;
    itp_error_code_t error = itp_push_frame_item(&root->frames, item);
    if (error) {
        itp_debug_print(error, "itp_push_notification> Failed to push frame");
        itp_print_stack_trace(stdout);
        itp_free(item);
        return error;
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_push_event(itp_root_tp root, itp_frame_tp origin) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_push_event> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!origin) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_push_event> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Fill frame
    origin->status = ITP_SYM_REQ;
    origin->from = root->address;

    if (root->remote_listeners.first) {
        itp_remote_listener_item_tp listener = root->remote_listeners.first;
        while (listener) {
            if (listener->command == 0 ||
                listener->command == origin->command) {

                itp_frame_tp frame = itp_copy_frame(origin);
                if (!frame) return ITP_ERRC_MALLOC_FAILED;

                frame->to = listener->address;

                #ifdef ITP_ENABLE_REQUEST_COMPARISON
                if (itp_compare_frames(&root->frames, frame)) {
                    itp_free_frame(frame);
                    return ITP_ERRC_NONE;
                }
                #endif // ITP_ENABLE_REQUEST_COMPARISON

                frame->order = itp_enumerate_frame(root);

                // Push to list
                itp_frame_item_tp item = itp_malloc(sizeof(itp_frame_item_t), "itp_push_event:itp_frame_item");
                if (!item) {
                    itp_free_frame(frame);
                    itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_event> Memory allocation failed");
                    itp_print_stack_trace(stdout);
                    return ITP_ERRC_MALLOC_FAILED;
                }
                itp_init_frame_item(item, frame);
                itp_init_handler_wrapper(
                    &item->handler, NULL,
                    #ifdef ITP_EXTEND_CALLBACKS
                    0,
                    #endif // ITP_EXTEND_CALLBACKS
                    0, 0
                );
                item->sender = root->address;
                item->wait_data = 0;
                itp_error_code_t error = itp_push_frame_item(&root->frames, item);
                if (error) {
                    itp_debug_print(error, "itp_push_event> Failed to push frame");
                    itp_print_stack_trace(stdout);
                    itp_free_frame_item(item);
                    return error;
                } else {
                    itp_debug_print(ITP_ERRC_TRACE, "itp_push_event> Node %d send event 0x%04x to %d", root->address, frame->command, listener->address);
                }
            }
            listener = listener->next;
        }
    }
    itp_free_frame(origin);
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_push_request(itp_root_tp root, itp_frame_tp frame, uint8_t address, itp_on_response_fp on_response
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_push_request> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!frame) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_push_request> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_push_request> Wrong address");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Fill frame
    frame->status = ITP_SYM_REQ;
    frame->from = root->address;
    frame->to = address;

    #ifdef ITP_ENABLE_REQUEST_COMPARISON
    if (itp_compare_frames(&root->frames, frame)) {
        itp_free_frame(frame);
        if (on_result) {
            on_result(
                #ifdef ITP_EXTEND_CALLBACKS
                eparam,
                #endif // ITP_EXTEND_CALLBACKS
                root, ITP_ERR_IGNORED
            );
        }
        return ITP_ERRC_NONE;
    }
    #endif // ITP_ENABLE_REQUEST_COMPARISON

    frame->order = itp_enumerate_frame(root);

    // Push to list
    itp_frame_item_tp item = itp_malloc(sizeof(itp_frame_item_t), "itp_push_request:itp_frame_item");
    if (!item) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_request> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    itp_init_frame_item(item, frame);
    itp_init_handler_wrapper(
        &item->handler, (void*)on_response,
        #ifdef ITP_EXTEND_CALLBACKS
        eparam,
        #endif // ITP_EXTEND_CALLBACKS
        0, 1
    );
    item->sender = root->address;
    item->wait_data = 1;
    itp_error_code_t error = itp_push_frame_item(&root->frames, item);
    if (error) {
        itp_debug_print(error, "itp_push_request> Failed to push frame");
        itp_print_stack_trace(stdout);
        itp_free(item);
        return error;
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_push_response(itp_root_tp root, itp_frame_tp request, itp_frame_tp response, itp_on_result_fp on_result
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!request) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_push_response> Request is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    return itp_push_response_for(
        root, request->from, request->order, response, on_result
        #ifdef ITP_EXTEND_CALLBACKS
        , eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
}

itp_error_code_t itp_push_response_for(itp_root_tp root, uint8_t address, uint16_t order, itp_frame_tp response, itp_on_result_fp on_result
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_push_response_for> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_push_response_for> Address is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!response) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_push_response_for> Response is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Fill response
    response->status = ITP_SYM_DAT;
    response->order = order;
    response->from = root->address;
    response->to = address;

    // Push to list
    itp_frame_item_tp item = itp_malloc(sizeof(itp_frame_item_t), "itp_push_response_for:itp_frame_item");
    if (!item) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_push_response_for> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    itp_init_frame_item(item, response);
    itp_init_handler_wrapper(
        &item->handler, (void*)on_result,
        #ifdef ITP_EXTEND_CALLBACKS
        eparam,
        #endif // ITP_EXTEND_CALLBACKS
        0, 0
    );
    item->sender = address;
    itp_error_code_t error = itp_push_frame_item(&root->frames, item);
    if (error) {
        itp_debug_print(error, "itp_push_response_for> Failed to push frame");
        itp_print_stack_trace(stdout);
        itp_free(item);
        return error;
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_register_handler(itp_root_tp root, uint8_t address, uint16_t command, itp_on_request_fp on_request
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_register_handler> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (command < 0x00A0) {
        itp_debug_print(ITP_ERRC_WRONG_COMMAND, "itp_register_handler> Forbidden command");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_WRONG_COMMAND;
    }
    if (!on_request) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_register_handler> Handler is null. To unregister handler use itp_unregister_handler(...)");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }

    return itp_push_handler_item(
        &root->handlers, address, command, on_request
        #ifdef ITP_EXTEND_CALLBACKS
        , eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
}

void itp_unregister_handler(itp_root_tp root, uint8_t address, uint16_t command) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_unregister_handler> Root is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (command < 0x00A0) {
        itp_debug_print(ITP_ERRC_WRONG_COMMAND, "itp_unregister_handler> Forbidden command");
        itp_print_stack_trace(stdout);
        return;
    }

    itp_erase_handler_item(&root->handlers, address, command);
}

void itp_unregister_all_handlers(itp_root_tp root) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_unregister_all_handlers> Root is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    itp_clear_handler_list(&root->handlers);
}

itp_error_code_t itp_send_control_wait_result(itp_root_tp root, itp_frame_tp frame, uint8_t address, itp_on_result_fp on_result
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_send_control_wait_result> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Fill frame
    frame->status = ITP_SYM_CTR;
    frame->order = itp_enumerate_frame(root);
    frame->from = root->address;
    frame->to = address;

    // Push to list
    itp_frame_item_tp item = itp_malloc(sizeof(itp_frame_item_t), "itp_send_control_wait_result:itp_frame_item");
    if (!item) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_send_control_wait_result> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    itp_init_frame_item(item, frame);
    itp_init_handler_wrapper(
        &item->handler, (void*)on_result,
        #ifdef ITP_EXTEND_CALLBACKS
        eparam,
        #endif // ITP_EXTEND_CALLBACKS
        0, 0
    );
    item->sender = root->address;
    itp_error_code_t error = itp_push_frame_item(&root->frames, item);
    if (error) {
        itp_debug_print(error, "itp_send_control_wait_result> Failed to push frame");
        itp_print_stack_trace(stdout);
        itp_free(item);
        return error;
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_send_control_wait_response(itp_root_tp root, itp_frame_tp frame, uint8_t address, uint8_t wait_data, itp_on_response_fp on_response
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!frame) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_send_control_wait_response> Frame is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    // Fill frame
    frame->status = ITP_SYM_CTR;
    frame->order = itp_enumerate_frame(root);
    frame->from = root->address;
    frame->to = address;

    // Push to list
    itp_frame_item_tp item = itp_malloc(sizeof(itp_frame_item_t), "itp_send_control_wait_response:itp_frame_item");
    if (!item) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_send_control_wait_response> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    itp_init_frame_item(item, frame);
    itp_init_handler_wrapper(
        &item->handler, (void*)on_response,
        #ifdef ITP_EXTEND_CALLBACKS
        eparam,
        #endif // ITP_EXTEND_CALLBACKS
        0, 1
    );
    item->sender = root->address;
    item->wait_data = wait_data;
    itp_error_code_t error = itp_push_frame_item(&root->frames, item);
    if (error) {
        itp_debug_print(error, "itp_send_control_wait_response> Failed to push frame");
        itp_print_stack_trace(stdout);
        itp_free(item);
        return error;
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_listen_remote(itp_root_tp root, uint8_t address, const uint16_t* commands, uint8_t length, itp_on_result_fp on_result
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_listen_remote> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_listen_remote> Wrong address");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Create frame
    itp_frame_tp frame = itp_create_frame(ITP_CMD_LISTEN_REMOTE);
    if (!frame) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_listen_remote> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }

    // Write data
    if (commands && length > 0) {
        itp_frame_write_array_2b(frame, commands, length);
    }

    // Send request
    itp_error_code_t error = itp_send_control_wait_result(
        root, frame, address, on_result
        #ifdef ITP_EXTEND_CALLBACKS
        , eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
    if (error) itp_free_frame(frame);
    return error;
}

itp_error_code_t itp_trace_route(itp_root_tp root, uint8_t address, itp_on_response_fp on_response
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_trace_route> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_trace_route> Wrong address");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Create frame
    itp_frame_tp frame = itp_create_frame(ITP_CMD_TRACE_ROUTE);
    if (!frame) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_trace_route> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }
    itp_frame_write_array_1b(frame, NULL, 0);

    // Send request
    itp_error_code_t error = itp_send_control_wait_response(
        root, frame, address, 1, on_response
        #ifdef ITP_EXTEND_CALLBACKS
        , eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
    if (error) itp_free_frame(frame);
    return error;
}

itp_error_code_t itp_get_children_address(itp_root_tp root, uint8_t address, itp_on_response_fp on_response
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_get_children_address> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!address) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_get_children_address> Wrong address");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Create frame
    itp_frame_tp frame = itp_create_frame(ITP_CMD_GET_CHILDREN);
    if (!frame) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_get_children_address> Memory allocation failed");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_MALLOC_FAILED;
    }

    // Send request
    itp_error_code_t error = itp_send_control_wait_response(
        root, frame, address, 1, on_response
        #ifdef ITP_EXTEND_CALLBACKS
        , eparam
        #endif // ITP_EXTEND_CALLBACKS
    );
    if (error) itp_free_frame(frame);
    return error;
}

itp_error_code_t itp_cancel_requests(itp_root_tp root, uint16_t command) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_cancel_requests> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Проверяем первый элемент
    itp_frame_item_tp last = NULL;
REPEAT_FIRST:
    last = root->frames.first;
    if (!last) {
        return ITP_ERRC_NONE;
    } else if (last->frame->command == command) {
        root->frames.first = last->next;
        itp_call_handler(&last->handler, root, ITP_ERR_CANCELLED, NULL);
        itp_free_frame_item(last);
        last = NULL;
        goto REPEAT_FIRST;
    }

    // Проверяем остальные элементы
    for (;;) {
        itp_frame_item_tp next = last->next;
        if (next) {
            if (next->frame->command == command) {
                last->next = next->next;
                itp_call_handler(&next->handler, root, ITP_ERR_CANCELLED, NULL);
                itp_free_frame_item(next);
                last = last->next;
            } else last = next;
        } else break;
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_cancel_all_requests(itp_root_tp root) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!root) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_cancel_all_requests> Root is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (root->frames.first) {
        itp_frame_item_tp item = root->frames.first;
        while (item) {
            itp_frame_item_tp next = item->next;
            itp_call_handler(&item->handler, root, ITP_ERR_CANCELLED, NULL);
            itp_free_frame_item(item);
            item = next;
        }
        root->frames.first = NULL;
    }
    return ITP_ERRC_NONE;
}

static void itp_on_data_response(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    itp_root_tp root,
    uint16_t error,
    itp_frame_tp frame
) {
    if (error) {
        itp_debug_print(ITP_ERRC_GENERAL, "itp_on_data_response> Data request failed with error %d", error);
        return;
    }
    if (!root || !frame) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_on_data_response> Null pointer found");
        return;
    }
    itp_paired_buffer_tp buffer = itp_find_buffer(&root->buffers, frame->from);
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_on_data_response> No buffer for node %d", frame->from);
        return;
    }
    itp_size_t length = ITP_PAIRED_BUFFER_SIZE - buffer->length;
    if (frame->length < length) length = frame->length;
    memcpy(buffer->data + buffer->length, frame->data, length);
    buffer->length += length;
}

itp_size_t itp_remote_endpoint_read(void* object, itp_byte_t* data, itp_size_t size) {

    itp_paired_buffer_tp buffer = (itp_paired_buffer_tp)object;
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_remote_endpoint_read> Buffer is null");
        return 0;
    }
    if (!buffer->root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_remote_endpoint_read> Root is null");
        return 0;
    }

    // Проверяем возможность отправки запроса
    if (buffer->lock_read) {
        if (buffer->lock_read + ITP_WAIT_CONTROL < buffer->root->time) {
            itp_cancel_requests(buffer->root, ITP_CMD_REQUEST_DATA);
            buffer->lock_read = 0;
        }
    }

    // Отправляем запрос на данные
    if (!buffer->lock_read) {
        itp_frame_tp request = itp_create_frame(ITP_CMD_REQUEST_DATA);
        if (request) {
            if (itp_send_control_wait_response(
                buffer->root, request, buffer->address, 1, &itp_on_data_response
                #ifdef ITP_EXTEND_CALLBACKS
                , 0
                #endif // ITP_EXTEND_CALLBACKS
            )) {
                itp_debug_print(ITP_ERRC_WARNING, "itp_remote_endpoint_read> Failed to push request");
                itp_free_frame(request);
            } else {
                buffer->lock_read = buffer->root->time;
            }
        } else {
            itp_debug_print(ITP_ERRC_WARNING, "itp_remote_endpoint_read> Failed to create request");
        }
    }

    // Читаем данные из буфера
    if (buffer->length > 0) {
        if (size < buffer->length) {
            memcpy(data, buffer->data, size);
            itp_size_t remain = buffer->length - size;
            for (itp_size_t i = 0; i < remain; ++i)
                data[i] = data[i + size];
            buffer->length -= size;
            return size;
        } else {
            memcpy(data, buffer->data, buffer->length);
            itp_size_t result = buffer->length;
            buffer->length = 0;
            return result;
        }
    }
    return 0;
}

itp_size_t itp_remote_endpoint_write(void* object, const itp_byte_t* data, itp_size_t length) {

    itp_paired_buffer_tp buffer = (itp_paired_buffer_tp)object;
    if (!buffer) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_remote_endpoint_write> Buffer is null");
        return 0;
    }
    if (!buffer->root) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_remote_endpoint_write> Root is null");
        return 0;
    }

    // Проверяем возможность отправки запроса
    if (buffer->lock_write) {
        if (buffer->lock_write + ITP_WAIT_CONTROL < buffer->root->time) {
            itp_cancel_requests(buffer->root, ITP_CMD_TRANSMIT_DATA);
            buffer->lock_write = 0;
        }
    }

    // Отправляем пакет с данными
    if (!buffer->lock_write) {
        itp_frame_tp request = itp_create_frame(ITP_CMD_TRANSMIT_DATA);
        if (request) {
            if (!itp_prepare_frame(request, length)) {
                memcpy(request->data, data, length);
                if (itp_send_control_wait_response(
                    buffer->root, request, buffer->address, 0, NULL
                    #ifdef ITP_EXTEND_CALLBACKS
                    , 0
                    #endif // ITP_EXTEND_CALLBACKS
                )) {
                    itp_debug_print(ITP_ERRC_WARNING, "itp_remote_endpoint_write> Failed to push request");
                    itp_free_frame(request);
                } else {
                    buffer->lock_write = buffer->root->time;
                    return length;
                }
            } else {
                itp_debug_print(ITP_ERRC_WARNING, "itp_remote_endpoint_write> Failed to write array");
                itp_free_frame(request);
            }
        } else {
            itp_debug_print(ITP_ERRC_WARNING, "itp_remote_endpoint_write> Failed to create request");
        }
    }
    return 0;
}

void itp_on_connect_local_listener(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    itp_root_tp root,
    uint8_t address,
    uint16_t error
) {
    if (error) {
        itp_debug_print(ITP_ERRC_GENERAL, "itp_on_connect_local_listener> Failed to connect local node with %d", error);
    } else {
        itp_debug_print(ITP_ERRC_TRACE, "itp_on_connect_local_listener> Connected local node %d", (int)address);
    }
    if (root->on_connect) {
        root->on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            root->on_connect_eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, address, error
        );
    }
}

void itp_on_connect_remote_listener(
    #ifdef ITP_EXTEND_CALLBACKS
    size_t eparam,
    #endif // ITP_EXTEND_CALLBACKS
    itp_root_tp root,
    uint8_t address,
    uint16_t error
) {
    if (error) {
        itp_debug_print(ITP_ERRC_GENERAL, "itp_on_connect_remote_listener> Failed to connect remote node with %d", error);
    } else {
        itp_debug_print(ITP_ERRC_TRACE, "itp_on_connect_remote_listener> Connected remote node %d", (int)address);
    }
    if (root->on_connect) {
        root->on_connect(
            #ifdef ITP_EXTEND_CALLBACKS
            root->on_connect_eparam,
            #endif // ITP_EXTEND_CALLBACKS
            root, address, error
        );
    }
}

void itp_free_any_handler(
    void* ptr
    #ifdef ITP_EXTEND_CALLBACKS
    , size_t eparam
    #endif // ITP_EXTEND_CALLBACKS
) {
    if (ptr != &itp_on_data_response &&
        ptr != &itp_on_connect_local_listener &&
        ptr != &itp_on_connect_remote_listener) {
        itp_free_user_handler(
            ptr
            #ifdef ITP_EXTEND_CALLBACKS
            , eparam
            #endif // ITP_EXTEND_CALLBACKS
        );
    }
}
