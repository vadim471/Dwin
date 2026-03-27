#include <itp/socket.hpp>
#include <itp/entity.hpp>
#include <itp/tools.hpp>
#include <itp/tunnel.hpp>
#include <itp/named_pipe.hpp>

#include <initp/system/debug.hpp>
#include <initp/utils/locale.hpp>

#include <boost/make_unique.hpp>

namespace itp {

    // Static methods

const char* entity::get_device_status_description(uint8_t status) {
    switch (status) {
        case itp::CL2_DST_OFFLINE:  return u8"OFFLINE";
        case itp::CL2_DST_ERROR:    return u8"ERROR";
        default:
            if ((status & itp::CL2_DST_CARD_INSIDE) &&
                (status & itp::CL2_DST_RFID_ACTIVE)) {
                return u8"INSIDE+RFID";
            } else if (status & itp::CL2_DST_CARD_INSIDE) {
                return u8"INSIDE";
            } else if (status & itp::CL2_DST_RFID_ACTIVE) {
                return u8"RFID";
            } else if (status & itp::CL2_DST_CARD_GATE) {
                return u8"GATE";
            } else return u8"ONLINE";
    }
}

    // Construction

entity::entity(uint32_t api, uint8_t type):
    api_(api | itp::CL2_API_BASIC), type_(type), node_()
{
    using namespace std::placeholders;
    this->node_.register_handler(itp::CL2_CMD_ALL_GET_TYPE, std::bind(&self_type::on_get_type, this, _1, _2));
    this->node_.register_handler(itp::CL2_CMD_ALL_GET_STATUS, std::bind(&self_type::on_get_status, this, _1, _2));
}

    // Public methods

itp::endpoint::uptr entity::make_tunnel(void) {
    std::tuple<itp::endpoint::uptr, itp::endpoint::uptr> tunnel = itp::tunnel::join();
    this->node_.set_parent(std::move(std::get<1>(tunnel)), nullptr);
    return std::move(std::get<0>(tunnel));
}

void entity::open_pipe(const std::string& name) {
    itp::named_pipe::uptr pipe = boost::make_unique<itp::named_pipe>(name);
    if (pipe) {
        pipe->open();
        this->node_.set_parent(std::move(pipe), nullptr);
    }
}

void entity::listen_port(uint16_t port) {
    itp::socket::uptr socket = boost::make_unique<itp::socket>();
    if (socket) {
        socket->listen(port);
        this->node_.set_parent(std::move(socket), nullptr);
    }
}

void entity::poll(initp::system::time_t time) {
    this->node_.poll((itp_time_t)time);
}

    // Request handlers

uint16_t entity::on_get_type(itp::root& root, itp::frame& request) {

    itp::frame::uptr response(new (std::nothrow) itp::frame(request.command()));
    if (!response) return ITP_ERR_MALLOC_FAILED;

    itp_error_code_t error = response->write_value(this->api());
    if (error) return ITP_ERR_INTERNAL;

    error = response->write_value(this->type());
    if (error) return ITP_ERR_INTERNAL;

    // Send response
    error = this->node_.push_response(request, std::move(response), nullptr);
    if (error) return ITP_ERR_INTERNAL;
    return ITP_ERR_NONE;
}

uint16_t entity::on_get_status(itp::root& root, itp::frame& request) {

    itp::frame::uptr response(new (std::nothrow) itp::frame(request.command()));
    if (!response) return ITP_ERR_MALLOC_FAILED;

    itp_error_code_t error = response->write_value(this->status());
    if (error) return ITP_ERR_INTERNAL;

    // Send response
    error = this->node_.push_response(request, std::move(response), nullptr);
    if (error) return ITP_ERR_INTERNAL;
    return ITP_ERR_NONE;
}

void entity::status_changed(void) {

    uint32_t status = this->status();
    sys_debug_print(SYSTEM_LEVEL_INFO, "itp::entity::status_changed", "Node %d (type %d) status now %s", (int)this->node_.address(), (int)this->type(), get_device_status_description(status));

    itp::frame::uptr frame(new (std::nothrow) itp::frame(itp::CL2_CMD_ALL_STATUS_CHANGED));
    if (!frame) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "itp::entity::status_changed>", "Memory allocation failed");
        return;
    }

    if (frame->write_value(status)) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "itp::entity::status_changed>", "Failed to write status");
        return;
    }

    if (this->node_.push_event(std::move(frame))) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "itp::entity::status_changed>", "Failed to push notification");
        return;
    }
}

    // Public properties

itp::root& entity::node(void) {
    return this->node_;
}

uint32_t entity::api(void) const {
    return this->api_;
}

uint8_t entity::type(void) const {
    return this->type_;
}

}
