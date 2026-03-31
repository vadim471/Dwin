#ifndef INITPLUS_SOCKET_DETAIL_HPP_INCLUDED
#define INITPLUS_SOCKET_DETAIL_HPP_INCLUDED

#include <initp/system/debug.hpp>
#include <initp/platform/socket.hpp>
#include <initp/system/macro.hpp>

namespace initp {
namespace system {

template<typename socket_type>
void socket::service::listen(uint16_t port, const std::function<void(std::unique_ptr<socket_type>&&)>& listener) {
    if (!this->lock_) this->run();
    if (this->pending_accept_) return;
    if (!this->acceptor_) {
        this->acceptor_.reset(new boost::asio::ip::tcp::acceptor(this->context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)));
    }
    this->pending_accept_ = true;
    using namespace std::placeholders;
    this->acceptor_->async_accept(
        RBIND(on_accept<socket_type>, _1, _2, listener)
    );
}

template<typename socket_type>
void socket::service::on_accept(boost::system::error_code ec, boost::asio::ip::tcp::socket object, const std::function<void(std::unique_ptr<socket_type>&&)>& listener) {
    if (ec) {
        this->pending_accept_ = false;
        sys_debug_print(SYSTEM_LEVEL_ERROR, "system::socket::service::on_accept", "Accept failed. %s", ec.message().c_str());
        return;
    }
    std::stringstream stream;
    stream << object.remote_endpoint().address();
    sys_debug_print(SYSTEM_LEVEL_INFO, "system::socket::service::on_accept", "Accepted connection from %s", stream.str().c_str());

    {
        LOCK_RMUTEX(this->mutex_);

        // Check children
        for (auto it = this->children_.begin(); it != this->children_.end(); ++it) {
            socket* child = *it;
            if (child->remote_ip_ == object.remote_endpoint().address()) {
                if (child->connected_) {
                    sys_debug_print(SYSTEM_LEVEL_WARNING, "system::socket::service::on_accept", "Same endpoint, but child connected");
                    continue;
                }
                sys_debug_print(SYSTEM_LEVEL_DEBUG, "system::socket::service::on_accept", "Child socket replaced");
                child->connected_ = true;
                child->socket_.reset(new boost::asio::ip::tcp::socket(std::move(object)));
                if (child->on_connect_)
                    child->on_connect_();
                if (child->was_connected_) {
                    if (child->on_reconnect_) {
                        child->on_reconnect_();
                    }
                } else {
                    child->was_connected_ = true;
                }
                goto DO_ACCEPT;
            }
        }

        { // Push new child
            sys_debug_print(SYSTEM_LEVEL_DEBUG, "system::socket::service::on_accept", "Creating new child");
            std::unique_ptr<socket_type> child(new socket_type(this->shared_from_this()));
            child->remote_ip_ = object.remote_endpoint().address();
            child->socket_.reset(new boost::asio::ip::tcp::socket(std::move(object)));
            child->connected_ = true;
            this->children_.push_back(child.get());
            if (listener) listener(std::move(child));
        }
    }

    // Accept next
DO_ACCEPT:
    using namespace std::placeholders;
    this->acceptor_->async_accept(
        RBIND(on_accept<socket_type>, _1, _2, listener)
    );
}

}}

#endif // INITPLUS_SOCKET_DETAIL_HPP_INCLUDED
