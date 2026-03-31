#include <itp/root.hpp>

extern "C" {

#include <itp/root.h>
#include <itp/callback.h>
#include <itp/command.h>
#include <itp/memory.h>

}

#include <initp/system/macro.hpp>
#include <initp/generic/function_generic.hpp>

namespace itp {

using namespace std::placeholders;

    // Construction

root::root(void):
    c_object_(NULL) {
    this->c_object_ = (itp_root_tp)itp_malloc(sizeof(itp_root_t), "itp::root:itp_root");
    if (this->c_object_) {
        itp_init_root(this->c_object_, NULL, 0);
    }
}

root::~root(void) {
    if (this->c_object_) {
        itp_free_root(this->c_object_);
    }
}

    // Static methods

itp_error_code_t root::generate_update_firmware(itp_byte_t* buffer, itp_size_t* length, uint16_t major, uint16_t minor, uint32_t build, bool use_checksum) {
    return itp_generate_update_firmware(buffer, length, major, minor, build, use_checksum ? 1 : 0);
}

    // Public methods

void root::poll(itp_time_t time) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    itp_poll_root(this->c_object_, time);
}

void root::dispose(void) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    itp_dispose_root(this->c_object_);
    this->children_.clear();
    this->listeners_.clear();
    this->parent_.reset();
}

void root::reinitialize(void) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    itp_dispose_root(this->c_object_);
    itp_init_root(this->c_object_, NULL, 0);
    this->children_.clear();
    this->listeners_.clear();
    this->parent_.reset();
}

void root::reinitialize(on_error_f f) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    endpoint::uptr ep = std::move(this->parent_);
    itp_dispose_root(this->c_object_);
    itp_init_root(this->c_object_, NULL, 0);
    this->children_.clear();
    this->listeners_.clear();
    if (ep) {
        this->set_parent(std::move(ep), f);
    }
}

itp_error_code_t root::request_connect(itp_time_t time, on_result_f on_result) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (on_result) {
        auto f = initp::tools::function<itp_on_result_f>::bind(RBIND(call_on_result, _2, on_result));
        itp_error_code_t errc = itp_request_connect(this->c_object_, time, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function<itp_on_result_fp>::free(std::get<1>(f));
        return errc;
    } else return itp_request_connect(this->c_object_, time, NULL, 0);
}

itp_error_code_t root::connect_local_node(itp_time_t time, endpoint::uptr&& endpoint, uint8_t address, on_connect_f on_connect) {
    if (!endpoint) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp::root::connect_local_node> Endpoint is null");
        return ITP_ERRC_INTERNAL;
    }
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    itp_endpoint_tp ep = endpoint->c_object();
    this->children_.emplace_back(std::move(endpoint));
    if (on_connect) {
        auto f = initp::tools::function<itp_on_connect_f>::bind(RBIND(call_on_connect, _2, _3, ep, on_connect));
        itp_error_code_t errc = itp_connect_local_node(this->c_object_, time, ep, address, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function<itp_on_connect_fp>::free(std::get<1>(f));
        return errc;
    } else return itp_connect_local_node(this->c_object_, time, ep, address, NULL, 0);
}

itp_error_code_t root::connect_remote_node(itp_time_t time, uint8_t host, uint8_t address, const std::string& path, on_connect_f on_connect) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (on_connect) {
        auto f = initp::tools::function<itp_on_connect_f>::bind(RBIND(call_on_connect, _2, _3, (itp_endpoint_tp)0, on_connect));
        itp_error_code_t errc = itp_connect_remote_node(this->c_object_, time, host, address, path.c_str(), std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function_pointer<itp_on_connect_fp>::free(std::get<1>(f));
        return errc;
    } else return itp_connect_remote_node(this->c_object_, time, host, address, path.c_str(), NULL, 0);
}

itp_error_code_t root::connect_remote_endpoint(itp_time_t time, uint8_t address, const std::string& path, on_connect_endpoint_f on_connect) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (on_connect) {
        auto f = initp::tools::function<itp_on_connect_endpoint_f>::bind(RBIND(call_on_connect_endpoint, _2, _3, _4, on_connect));
        itp_error_code_t errc = itp_connect_remote_endpoint(this->c_object_, time, address, path.c_str(), std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function_pointer<itp_on_connect_endpoint_fp>::free(std::get<1>(f));
        return errc;
    } else return itp_connect_remote_endpoint(this->c_object_, time, address, path.c_str(), NULL, 0);
}

itp_error_code_t root::reconnect_local_node(itp_time_t time, uint8_t address, on_connect_f on_connect) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (on_connect) {
        auto f = initp::tools::function<itp_on_connect_f>::bind(RBIND(call_on_reconnect, _2, _3, on_connect));
        itp_error_code_t errc = itp_reconnect_local_node(this->c_object_, time, address, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function<itp_on_connect_fp>::free(std::get<1>(f));
        return errc;
    } else return itp_reconnect_local_node(this->c_object_, time, address, NULL, 0);
}

std::tuple<uint8_t, itp_error_code_t> root::get_free_address(void) {
    itp_error_code_t error = ITP_ERRC_NONE;
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    uint8_t address = itp_get_free_address(this->c_object_, &error);
    return std::make_tuple(address, error);
}

bool root::is_local_child(uint8_t address) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    return itp_is_local_child(this->c_object_, address) > 0;
}

void root::on_create(on_create_f on_create) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (this->c_object_->on_create ||
        this->c_object_->on_create_eparam) {
        itp_free_any_handler(
            (void*)this->c_object_->on_create,
            this->c_object_->on_create_eparam
        );
    }
    if (on_create) {
        auto f = initp::tools::function<itp_on_create_f>::bind(RBIND(call_on_create, _1, _2, on_create));
        this->c_object_->on_create = std::get<0>(f);
        this->c_object_->on_create_eparam = std::get<1>(f);
    } else {
        this->c_object_->on_create = NULL;
        this->c_object_->on_create_eparam = 0;
    }
}

void root::on_connect(on_connect_f on_connect) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (this->c_object_->on_connect ||
        this->c_object_->on_connect_eparam) {
        itp_free_any_handler(
            (void*)this->c_object_->on_connect,
            this->c_object_->on_connect_eparam
        );
    }
    if (on_connect) {
        auto f = initp::tools::function<itp_on_connect_f>::bind(RBIND(call_on_connect, _2, _3, nullptr, on_connect));
        this->c_object_->on_connect = std::get<0>(f);
        this->c_object_->on_connect_eparam = std::get<1>(f);
    } else {
        this->c_object_->on_connect = NULL;
        this->c_object_->on_connect_eparam = 0;
    }
}

void root::on_reset(on_signal_f on_reset) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (this->c_object_->on_reset ||
        this->c_object_->on_reset_eparam) {
        itp_free_any_handler(
            (void*)this->c_object_->on_reset,
            this->c_object_->on_reset_eparam
        );
    }
    if (on_reset) {
        auto f = initp::tools::function<itp_on_signal_f>::bind(RBIND(call_on_signal, on_reset));
        this->c_object_->on_reset = std::get<0>(f);
        this->c_object_->on_reset_eparam = std::get<1>(f);
    } else {
        this->c_object_->on_reset = NULL;
        this->c_object_->on_reset_eparam = 0;
    }
}

void root::on_update_firmware(on_update_f on_update_firmware) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (this->c_object_->on_update_firmware ||
        this->c_object_->on_update_firmware_eparam) {
        itp_free_any_handler(
            (void*)this->c_object_->on_update_firmware,
            this->c_object_->on_update_firmware_eparam
        );
    }
    if (on_update_firmware) {
        auto f = initp::tools::function<itp_on_update_f>::bind(RBIND(call_on_update, on_update_firmware, _2, _3, _4));
        this->c_object_->on_update_firmware = std::get<0>(f);
        this->c_object_->on_update_firmware_eparam = std::get<1>(f);
    } else {
        this->c_object_->on_update_firmware = NULL;
        this->c_object_->on_update_firmware_eparam = 0;
    }
}

itp_error_code_t root::set_parent(endpoint::uptr&& endpoint, on_error_f on_error) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (endpoint) {
        this->parent_ = std::move(endpoint);
        if (on_error) {
            auto f = initp::tools::function<itp_on_error_f>::bind(RBIND(call_on_error, _2, _3, on_error));
            itp_error_code_t errc = itp_set_parent(this->c_object_, this->parent_->c_object(), std::get<0>(f), std::get<1>(f));
            if (errc) initp::tools::function_pointer<itp_on_error_fp>::free(std::get<1>(f));
            return errc;
        } else return itp_set_parent(this->c_object_, this->parent_->c_object(), NULL, 0);
    } else {
        if (this->parent_) {
            this->parent_.reset();
            return itp_set_parent(this->c_object_, NULL, NULL, 0);
        }
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t root::set_connection_listener(endpoint::uptr&& endpoint) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (endpoint) {
        for (endpoint::uptr& it : this->listeners_) {
            if (it->c_object() == endpoint->c_object()) {
                return ITP_ERRC_NONE;
            }
        }
        itp_error_code_t errc = itp_set_connection_listener(this->c_object_, endpoint->c_object());
        if (!errc) this->listeners_.push_back(std::move(endpoint));
        return errc;
    }
    return ITP_ERRC_NONE;
}

    // Command layer

itp_error_code_t root::push_event(frame::uptr frame) {
    itp_error_code_t error;
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    error = itp_push_event(this->c_object_, frame->c_object());
    if (error) itp_free_frame(frame->c_object());
    frame->c_object(NULL);
    return error;
}

itp_error_code_t root::push_notification(frame::uptr frame, uint8_t address, on_result_f on_result) {
    itp_error_code_t error;
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (on_result) {
        auto f = initp::tools::function<itp_on_result_f>::bind(RBIND(call_on_result, _2, on_result));
        error = itp_push_notification(this->c_object_, frame->c_object(), address, std::get<0>(f), std::get<1>(f));
        if (error) initp::tools::function_pointer<itp_on_error_fp>::free(std::get<1>(f));
    } else error = itp_push_notification(this->c_object_, frame->c_object(), address, NULL, 0);
    if (error) itp_free_frame(frame->c_object());
    frame->c_object(NULL);
    return error;
}

itp_error_code_t root::push_request(frame::uptr frame, uint8_t address, on_response_f on_response) {
    itp_error_code_t error;
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (on_response) {
        auto f = initp::tools::function<itp_on_response_f>::bind(RBIND(call_on_response, _2, _3, on_response));
        error = itp_push_request(this->c_object_, frame->c_object(), address, std::get<0>(f), std::get<1>(f));
        if (error) initp::tools::function_pointer<itp_on_response_fp>::free(std::get<1>(f));
    } else error = itp_push_request(this->c_object_, frame->c_object(), address, NULL, 0);
    if (error) itp_free_frame(frame->c_object());
    frame->c_object(NULL);
    return error;
}

itp_error_code_t root::push_response(frame& request, frame::uptr&& response, on_result_f on_result) {
    itp_error_code_t error;
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (on_result) {
        auto f = initp::tools::function<itp_on_result_f>::bind(RBIND(call_on_result, _2, on_result));
        error = itp_push_response(this->c_object_, request.c_object(), response->c_object(), std::get<0>(f), std::get<1>(f));
        if (error) initp::tools::function_pointer<itp_on_result_fp>::free(std::get<1>(f));
    } else error = itp_push_response(this->c_object_, request.c_object(), response->c_object(), NULL, 0);
    if (error) itp_free_frame(response->c_object());
    response->c_object(NULL);
    return error;
}

itp_error_code_t root::push_response_for(uint8_t address, uint16_t order, frame::uptr&& response, on_result_f on_result) {
    itp_error_code_t error;
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (on_result) {
        auto f = initp::tools::function<itp_on_result_f>::bind(RBIND(call_on_result, _2, on_result));
        error = itp_push_response_for(this->c_object_, address, order, response->c_object(), std::get<0>(f), std::get<1>(f));
        if (error) initp::tools::function_pointer<itp_on_result_fp>::free(std::get<1>(f));
    } else error = itp_push_response_for(this->c_object_, address, order, response->c_object(), NULL, 0);
    if (error) itp_free_frame(response->c_object());
    response->c_object(NULL);
    return error;
}

itp_error_code_t root::register_handler(uint8_t address, uint16_t command, on_request_f on_request) {
    if (!on_request) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp::root::register_handler> Handler is null");
        return ITP_ERRC_NULL_POINTER;
    }
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    auto f = initp::tools::function<itp_on_request_f>::bind(RBIND(call_on_request, _2, on_request));
    itp_error_code_t errc = itp_register_handler(this->c_object_, address, command, std::get<0>(f), std::get<1>(f));
    if (errc) initp::tools::function_pointer<itp_on_request_fp>::free(std::get<1>(f));
    return errc;
}

itp_error_code_t root::register_handler(uint16_t command, on_request_f on_request) {
    return this->register_handler(0, command, on_request);
}

root::handler_proxy_t* root::create_handler_proxy(uint8_t address, uint16_t command) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    for (multi_handler_t& item : this->handlers_) {
        if (std::get<0>(item) == address &&
            std::get<1>(item) == command) {
            return &std::get<2>(item);
        }
    }
    on_request_f f = [this, address, command] (root&, frame& event) -> uint16_t {
        for (multi_handler_t& item : this->handlers_) {
            if (std::get<0>(item) == address &&
                std::get<1>(item) == command) {
                event.prepare_to_read();
                std::get<2>(item).rise(event);
            }
        }
        return ITP_ERR_NONE;
    };
    itp_error_code_t errc = this->register_handler(address, command, f);
    if (!errc) {
        this->handlers_.emplace_back(address, command, handler_proxy_t());
        return &std::get<2>(this->handlers_.back());
    }
    return nullptr;
}

void root::unregister_handler(uint8_t address, uint16_t command) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    return itp_unregister_handler(this->c_object_, address, command);
}

void root::unregister_handler(uint16_t command) {
    return this->unregister_handler(0, command);
}

void root::unregister_all_handlers(void) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    itp_unregister_all_handlers(this->c_object_);
}

itp_error_code_t root::listen_remote(uint8_t address, on_result_f on_result) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (on_result) {
        auto f = initp::tools::function<itp_on_result_f>::bind(RBIND(call_on_result, _2, on_result));
        itp_error_code_t errc = itp_listen_remote(this->c_object_, address, NULL, 0, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function<itp_on_result_fp>::free(std::get<1>(f));
        return errc;
    } else return itp_listen_remote(this->c_object_, address, NULL, 0, NULL, 0);
}

itp_error_code_t root::listen_remote(uint8_t address, const std::vector<uint16_t>& commands, on_result_f on_result) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    const uint16_t* data = (commands.size() > 0) ? &commands[0] : NULL;
    uint8_t length = (commands.size() > 122) ? 122 : (uint8_t)commands.size();
    if (on_result) {
        auto f = initp::tools::function<itp_on_result_f>::bind(RBIND(call_on_result, _2, on_result));
        itp_error_code_t errc = itp_listen_remote(this->c_object_, address, data, length, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function<itp_on_result_fp>::free(std::get<1>(f));
        return errc;
    } else return itp_listen_remote(this->c_object_, address, data, length, NULL, 0);
}

itp_error_code_t root::trace_route(uint8_t address, on_response_f on_response) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (on_response) {
        auto f = initp::tools::function<itp_on_response_f>::bind(RBIND(call_on_response, _2, _3, on_response));
        itp_error_code_t errc = itp_trace_route(this->c_object_, address, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function<itp_on_response_fp>::free(std::get<1>(f));
        return errc;
    } else return itp_trace_route(this->c_object_, address, NULL, 0);
}

itp_error_code_t root::get_children_address(uint8_t address, on_response_f on_response) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    if (on_response) {
        auto f = initp::tools::function<itp_on_response_f>::bind(RBIND(call_on_response, _2, _3, on_response));
        itp_error_code_t errc = itp_get_children_address(this->c_object_, address, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function<itp_on_response_fp>::free(std::get<1>(f));
        return errc;
    } else return itp_get_children_address(this->c_object_, address, NULL, 0);
}

itp_error_code_t root::signal_reset(uint8_t address) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    return itp_signal_reset(this->c_object_, address);
}

itp_error_code_t root::signal_update_firmware(uint8_t address, uint16_t major, uint16_t minor, uint32_t build) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    return itp_signal_update_firmware(this->c_object_, address, major, minor, build);
}

    // Handler calls

void root::call_on_error(uint8_t address, itp_error_code_t error, on_error_f on_error) {
    if (!on_error) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::root::call_on_error> No on_error callback");
        return;
    }
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    return on_error(*this, address, error);
}

void root::call_on_signal(on_signal_f on_signal) {
    if (!on_signal) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::root::call_on_signal> No on_signal callback");
        return;
    }
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    return on_signal(*this);
}

itp_endpoint_tp root::call_on_create(const char* path, uint16_t* error, on_create_f on_create) {
    if (!on_create) {
        itp_debug_print(ITP_ERRC_EP_NOT_CONFIGURED, "itp::root::call_on_create> No on_create callback");
        if (error) *error = ITP_ERR_NOT_CONFIGURED;
        return NULL;
    }
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    uint16_t result;
    endpoint::uptr endpoint;
    std::tie(endpoint, result) = on_create(std::string(path));
    if (!result) {
        if (!endpoint) {
            if (error) *error = ITP_ERR_NULL_POINTER;
            return NULL;
        } else {
            if (error) *error = result;
            itp_endpoint_tp ep = endpoint->c_object();
            this->children_.emplace_back(std::move(endpoint));
            return ep;
        }
    } else {
        if (error) *error = result;
        return NULL;
    }
}

void root::call_on_connect(uint8_t address, uint16_t error, itp_endpoint_tp endpoint, on_connect_f on_connect) {
    endpoint::uptr ep;
    {
        #ifdef ITP_ENABLE_THREAD_SAFETY
        std::lock_guard<std::recursive_mutex> lock(this->mutex_);
        #endif // ITP_ENABLE_THREAD_SAFETY
        if (error && endpoint) {
            for (auto it = this->children_.begin(); it != this->children_.end(); ++it) {
                if ((*it)->c_object() == endpoint) {
                    ep.swap(*it);
                    this->children_.erase(it);
                    break;
                }
            }
        }
    }
    if (ep) ep.reset();
    {
        if (!on_connect) {
            itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::root::call_on_connect> No on_connect callback");
            return;
        }
        #ifdef ITP_ENABLE_THREAD_SAFETY
        std::lock_guard<std::recursive_mutex> lock(this->mutex_);
        #endif // ITP_ENABLE_THREAD_SAFETY
        return on_connect(*this, address, error);
    }
}

void root::call_on_reconnect(uint8_t address, uint16_t error, on_connect_f on_connect) {
    if (!on_connect) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::root::call_on_connect> No on_connect callback");
        return;
    }
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    return on_connect(*this, address, error);
}

void root::call_on_result(uint16_t error, on_result_f on_result) {
    if (!on_result) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::root::call_on_result> No on_result callback");
        return;
    }
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    return on_result(*this, error);
}

uint16_t root::call_on_request(itp_frame_tp frame, on_request_f on_request) {
    if (!on_request) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::root::call_on_request> No on_request callback");
        return ITP_ERR_NONE;
    }
    itp::frame f(frame);
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    uint16_t result = on_request(*this, f);
    f.c_object(NULL);
    return result;
}

void root::call_on_response(uint16_t error, itp_frame_tp frame, on_response_f on_response) {
    if (!on_response) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::root::call_on_response> No on_response callback");
        return;
    }
    itp::frame f(frame);
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    on_response(*this, error, f);
    f.c_object(NULL);
}

void root::call_on_update(on_update_f on_update, uint16_t major, uint16_t minor, uint32_t build) {
    if (!on_update) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::root::call_on_update> No on_update callback");
        return;
    }
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    return on_update(*this, major, minor, build);
}

void root::call_on_connect_endpoint(uint8_t address, uint16_t error, itp_endpoint_tp endpoint, on_connect_endpoint_f on_connect) {
    if (!on_connect) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp::root::call_on_connect_endpoint> No on_connect callback");
        return;
    }
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    return on_connect(*this, address, error, endpoint ? remote_endpoint::uptr(new remote_endpoint(endpoint)) : remote_endpoint::uptr());
}

    // Public properties

itp_root_tp root::c_object(void) {
    return this->c_object_;
}

endpoint::uptr& root::parent(void) {
    return this->parent_;
}

uint8_t root::address(void) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    return this->c_object_->address;
}

void root::address(uint8_t value) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    this->c_object_->address = value;
}

uint8_t root::root_address(void) {
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // ITP_ENABLE_THREAD_SAFETY
    return this->c_object_->root;
}

#ifdef ITP_ENABLE_THREAD_SAFETY
std::recursive_mutex& root::mutex(void) {
    return this->mutex_;
}
#endif // ITP_ENABLE_THREAD_SAFETY

}
