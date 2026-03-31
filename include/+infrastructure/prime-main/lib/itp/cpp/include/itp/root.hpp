#ifndef ITP_ROOT_HPP_INCLUDED
#define ITP_ROOT_HPP_INCLUDED

#include "config.hpp"
#include "callback.hpp"
#include "endpoint.hpp"
#include "frame.hpp"

extern "C" {

#include <itp/types.h>

}

#include <initp/generic/event_proxy.hpp>

#include <tuple>
#include <memory>
#include <functional>
#include <vector>
#include <list>

#ifdef ITP_ENABLE_THREAD_SAFETY
#include <mutex>
#endif // ITP_ENABLE_THREAD_SAFETY

namespace itp {

/**
 * \brief Класс, управляющий корневой структурой узла сети \c itp_root.
 */
class root:
    public std::enable_shared_from_this<root> {

private: // Private types
    typedef root self_type;

public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::unique_ptr<self_type> uptr;
    typedef std::weak_ptr<self_type> wptr;
    typedef initp::tools::event_proxy<void(frame&)> handler_proxy_t;
    typedef std::tuple<uint8_t, uint16_t, handler_proxy_t> multi_handler_t;

public: // Construction
    root(void);
    root(const self_type&) = delete;
    virtual ~root(void);

public: // Static methods
    static itp_error_code_t generate_update_firmware(itp_byte_t* buffer, itp_size_t* length, uint16_t major, uint16_t minor, uint32_t build, bool use_checksum);

public: // Public methods
    void poll(itp_time_t time);
    void dispose(void);
    void reinitialize(void);
    void reinitialize(on_error_f);
    itp_error_code_t request_connect(itp_time_t time, on_result_f on_result);
    itp_error_code_t connect_local_node(itp_time_t time, endpoint::uptr&& endpoint, uint8_t address, on_connect_f on_connect);
    itp_error_code_t connect_remote_node(itp_time_t time, uint8_t host, uint8_t address, const std::string& path, on_connect_f on_connect);
    itp_error_code_t connect_remote_endpoint(itp_time_t time, uint8_t address, const std::string& path, on_connect_endpoint_f on_connect);
    itp_error_code_t reconnect_local_node(itp_time_t time, uint8_t address, on_connect_f on_connect);
    std::tuple<uint8_t, itp_error_code_t> get_free_address(void);
    bool is_local_child(uint8_t address);
    void on_create(on_create_f on_create);
    void on_connect(on_connect_f on_connect);
    void on_reset(on_signal_f on_reset);
    void on_update_firmware(on_update_f on_update_firmware);
    itp_error_code_t set_parent(endpoint::uptr&& endpoint, on_error_f on_error);
    itp_error_code_t set_connection_listener(endpoint::uptr&& endpoint);

public: // Command layer
    itp_error_code_t push_event(frame::uptr frame);
    itp_error_code_t push_notification(frame::uptr frame, uint8_t address, on_result_f on_result);
    itp_error_code_t push_request(frame::uptr frame, uint8_t address, on_response_f on_response);
    itp_error_code_t push_response(frame& request, frame::uptr&& response, on_result_f on_result);
    itp_error_code_t push_response_for(uint8_t address, uint16_t order, frame::uptr&& response, on_result_f on_result);
    itp_error_code_t register_handler(uint8_t address, uint16_t command, on_request_f on_request);
    itp_error_code_t register_handler(uint16_t command, on_request_f on_request);
    handler_proxy_t* create_handler_proxy(uint8_t address, uint16_t command);
    void unregister_handler(uint8_t address, uint16_t command);
    void unregister_handler(uint16_t command);
    void unregister_all_handlers(void);
    itp_error_code_t listen_remote(uint8_t address, on_result_f on_result);
    itp_error_code_t listen_remote(uint8_t address, const std::vector<uint16_t>& commands, on_result_f on_result);
    itp_error_code_t trace_route(uint8_t address, on_response_f on_response);
    itp_error_code_t get_children_address(uint8_t address, on_response_f on_response);
    itp_error_code_t signal_reset(uint8_t address);
    itp_error_code_t signal_update_firmware(uint8_t address, uint16_t major, uint16_t minor, uint32_t build);

private: // Handler calls
    void call_on_error(uint8_t address, itp_error_code_t error, on_error_f on_error);
    void call_on_signal(on_signal_f on_signal);
    itp_endpoint_tp call_on_create(const char* path, uint16_t* error, on_create_f on_create);
    void call_on_connect(uint8_t address, uint16_t error, itp_endpoint_tp endpoint, on_connect_f on_connect);
    void call_on_reconnect(uint8_t address, uint16_t error, on_connect_f on_connect);
    void call_on_result(uint16_t error, on_result_f on_result);
    uint16_t call_on_request(itp_frame_tp frame, on_request_f on_request);
    void call_on_response(uint16_t error, itp_frame_tp frame, on_response_f on_response);
    void call_on_update(on_update_f on_update, uint16_t, uint16_t, uint32_t);
    void call_on_connect_endpoint(uint8_t address, uint16_t error, itp_endpoint_tp endpoint, on_connect_endpoint_f on_connect);

public: // Public properties
    itp_root_tp c_object(void);
    endpoint::uptr& parent(void);
    uint8_t address(void);
    void address(uint8_t value);
    uint8_t root_address(void);
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::recursive_mutex& mutex(void);
    #endif // ITP_ENABLE_THREAD_SAFETY

private: // Private fields
    itp_root_tp c_object_;
    endpoint::uptr parent_;
    std::vector<endpoint::uptr> children_;
    std::vector<endpoint::uptr> listeners_;
    std::vector<multi_handler_t> handlers_;
    #ifdef ITP_ENABLE_THREAD_SAFETY
    std::recursive_mutex mutex_;
    #endif // ITP_ENABLE_THREAD_SAFETY
};

}

#endif // ITP_ROOT_HPP_INCLUDED
