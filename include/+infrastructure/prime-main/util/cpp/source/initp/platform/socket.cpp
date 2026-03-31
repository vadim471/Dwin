#include <initp/system/debug.hpp>
#include <initp/platform/socket.hpp>
#include <initp/system/macro.hpp>

#include <boost/make_unique.hpp>
#include <boost/asio.hpp>

#include <sstream>
#include <thread>

namespace initp {
namespace system {

using namespace std::placeholders;

    // Class buffer

socket::buffer::buffer(void):
    begin_(0),
    end_(0)
{}

size_t socket::buffer::max(void) const {
    return 0x100;
}

size_t socket::buffer::size(void) const {
    if (this->begin_ >= this->end_)
        return 0;
    return this->end_ - this->begin_;
}

bool socket::buffer::empty(void) const {
    return this->begin_ >= this->end_;
}

void socket::buffer::clear(void) {
    this->begin_ = 0;
    this->end_ = 0;
}

void socket::buffer::read(size_t count) {
    this->begin_ += count;
    if (this->begin_ >= this->end_) {
        this->clear();
    }
}

void socket::buffer::write(size_t count) {
    this->end_ += count;
    if (this->end_ > 0x100) {
        this->end_ = 0x100;
    }
}

uint8_t* socket::buffer::data(void) {
    return this->data_ + this->begin_;
}

    // Class service

socket::service::service(void):
    context_(),
    pending_accept_(false)
{}

socket::service::~service(void) {
    sys_debug_print(SYSTEM_LEVEL_DEBUG, "system::socket::service::~service", "Called");
    this->context_.stop();
    this->lock_.reset();
    if (this->thread_) {
        this->thread_->join();
        this->thread_.reset();
    }
    sys_debug_print(SYSTEM_LEVEL_DEBUG, "system::socket::service::~service", "Stopped");
}

void socket::service::run(void) {
    if (this->lock_) return;
    this->lock_.reset(new work_guard_type(this->context_.get_executor()));
    std::thread* thread = new (std::nothrow) std::thread([this] (void) -> void {
        boost::system::error_code ec;
        sys_debug_print(SYSTEM_LEVEL_INFO, "system::socket::service::run", ">>> Socket thread started");
        this->context_.run(ec);
        sys_debug_print(SYSTEM_LEVEL_INFO, "system::socket::service::run", "<<< Socket thread complete");
    });
    this->thread_.reset(thread);
}

void socket::service::cancel(void) {
    if (this->acceptor_ &&
        this->pending_accept_) {
        this->acceptor_->cancel();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void socket::service::listen(uint16_t port, socket* child) {
    if (!this->lock_) this->run();
    if (this->pending_accept_) return;
    if (!this->acceptor_) {
        this->acceptor_.reset(new boost::asio::ip::tcp::acceptor(this->context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)));
    }
    this->pending_accept_ = true;
    this->acceptor_->async_accept(
        RBIND(on_single_accept, _1, _2, child)
    );
}

boost::asio::io_context& socket::service::context(void) {
    return this->context_;
}

void socket::service::on_single_accept(boost::system::error_code ec, boost::asio::ip::tcp::socket object, socket* child) {

    if (ec) {
        this->pending_accept_ = false;
        sys_debug_print(SYSTEM_LEVEL_ERROR, "system::socket::service::on_single_accept", "Accept failed. Error code %d", ec.value());
        //sys_debug_print(SYSTEM_LEVEL_ERROR, "system::socket::service::on_single_accept", "Accept failed. %s", ec.message().c_str());
        return;
    }
    std::stringstream stream;
    stream << object.remote_endpoint().address();
    sys_debug_print(SYSTEM_LEVEL_INFO, "system::socket::service::on_single_accept", "Accepted connection from %s", stream.str().c_str());

    {
        LOCK_RMUTEX(this->mutex_);

        // Check children
        for (auto it = this->children_.begin(); it != this->children_.end(); ++it) {
            if (child == *it) {
                goto STORE;
            }
        }
        this->children_.push_back(child);
    STORE:
        child->remote_ip_ = object.remote_endpoint().address();
        child->socket_.reset(new boost::asio::ip::tcp::socket(std::move(object)));
        child->connected_ = true;
        if (child->on_connect_)
            child->on_connect_();
        if (child->was_connected_) {
            if (child->on_reconnect_) {
                child->on_reconnect_();
            }
        } else {
            child->was_connected_ = true;
        }
    }

    this->pending_accept_ = false;
}

    // Construction

socket::socket(void):
    service_(std::make_shared<service>()),
    resolver_(),
    socket_(),
    port_(0),
    pending_connect_(false),
    pending_read_(false),
    pending_write_(false),
    was_connected_(false),
    connected_(false),
    client_(false),
    last_connect_(0),
    reconnect_delay_(0)
{}

socket::socket(service::ptr service):
    service_(service),
    resolver_(),
    socket_(),
    port_(0),
    pending_connect_(false),
    pending_read_(false),
    pending_write_(false),
    was_connected_(false),
    connected_(false),
    client_(false),
    last_connect_(0),
    reconnect_delay_(0)
{}

socket::~socket(void) {
    sys_debug_print(SYSTEM_LEVEL_DEBUG, "system::socket::~socket", "Called");
    if (this->socket_) {
        if (this->pending_connect_ ||
            this->pending_read_ ||
            this->pending_write_) {
            this->socket_->cancel();
            const uint32_t retries = 30;
            for (uint32_t i = 0; i <= retries; ++i) {
                if (this->pending_connect_ ||
                    this->pending_read_ ||
                    this->pending_write_) {
                    if (i == retries) {
                        sys_debug_print(
                            SYSTEM_LEVEL_WARNING,
                            "system::socket::~socket",
                            "Failed to finish asynchronous tasks, %s still active",
                            this->pending_connect_ ? "connect" : this->pending_read_ ? "read": "write"
                        );
                    } else {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                } else {
                    sys_debug_print(SYSTEM_LEVEL_DEBUG, "system::socket::~socket", "Asynchronous tasks stopped on try %d", (int)i);
                    break;
                }
            }
        } else {
            sys_debug_print(SYSTEM_LEVEL_DEBUG, "system::socket::~socket", "No asynchronous tasks found");
        }
    }
    if (this->socket_ &&
        this->socket_->is_open()) {
        this->socket_->close();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (this->service_) {
        {
            LOCK_RMUTEX(this->service_->mutex_);
            for (auto it = this->service_->children_.begin(); it != this->service_->children_.end();) {
                if (*it == this) {
                    it = this->service_->children_.erase(it);
                    break;
                } else ++it;
            }
        }
        this->service_.reset();
    }
    if (this->socket_) {
        sys_debug_print(SYSTEM_LEVEL_DEBUG, "system::socket::~socket", "Reset socket");
        this->socket_.reset();
    }
    sys_debug_print(SYSTEM_LEVEL_DEBUG, "system::socket::~socket", "Complete");
}

    // Public methods

void socket::listen(uint16_t port) {
    this->service_->listen(port, this);
    this->service_->run();
}

void socket::connect(const std::string& host, uint16_t port) {
    this->client_ = true;
    this->host_ = host;
    this->port_ = port;
    this->do_resolve();
    this->service_->run();
}

bool socket::connected(void) const {
    return this->connected_;
}

void socket::reconnect_delay(initp::system::time_t value) {
    this->reconnect_delay_ = value;
}

const boost::asio::ip::address& socket::remote_ip(void) const {
    return this->remote_ip_;
}

void socket::set_on_connect(const std::function<void(void)>& f) {
    this->on_connect_ = f;
}

void socket::set_on_reconnect(const std::function<void(void)>& f) {
    this->on_reconnect_ = f;
}

void socket::set_on_disconnect(const std::function<void(void)>& f) {
    this->on_disconnect_ = f;
}

    // Read/write

size_t socket::read(char* data, size_t size) {
    if (!this->read_buffer_.empty()) {
        size_t need = this->read_buffer_.size();
        size_t read = (need > size) ? size : need;
        memcpy(data, this->read_buffer_.data(), read);
        this->read_buffer_.read(read);
        if (!this->pending_read_ &&
            this->read_buffer_.empty()) {
            this->do_read();
        }
        return read;
    } else if (!this->pending_read_) {
        this->do_read();
    }
    return 0;
}

size_t socket::write(const char* data, size_t length) {
    if (!this->connected_ &&
        !this->pending_write_) {
        this->do_write();
        return 0;
    }
    if (this->write_buffer_.empty()) {
        this->write_buffer_.clear();
        if (length > this->write_buffer_.max())
            length = this->write_buffer_.max();
        memcpy(this->write_buffer_.data(), data, length);
        this->write_buffer_.write(length);
        if (!this->pending_write_) {
            this->do_write();
        }
        return length;
    } else if (!this->pending_write_) {
        this->do_write();
    }
    return 0;
}

    // Private methods

void socket::do_resolve(void) {
    if (this->connected_) return;
    if (this->pending_connect_) return;
    if (!this->service_) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "system::socket::do_resolve", "Service is null");
        return;
    }
    system::time_t now = system::time::now();
    if (this->last_connect_ + 5000 < now) {
        this->last_connect_ = now;
    } else return;
    if (!this->resolver_) {
        this->resolver_.reset(new boost::asio::ip::tcp::resolver(this->service_->context()));
    }
    if (!this->pending_connect_) {
        sys_debug_print(SYSTEM_LEVEL_INFO, "system::socket::do_resolve", "Asynchronous connection");
        this->pending_connect_ = true;
    }
    std::stringstream port;
    port << this->port_;
    this->resolver_->async_resolve(
        this->host_,
        port.str(),
        RBIND(on_resolve, _1, _2)
    );
}

void socket::do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints) {
    if (this->connected_) {
        this->pending_connect_ = false;
        return;
    }
    if (!this->service_) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "system::socket::do_connect", "Service is null");
        this->pending_connect_ = false;
        return;
    }
    if (!this->socket_) {
        this->socket_.reset(new boost::asio::ip::tcp::socket(this->service_->context()));
    }
    if (!this->pending_connect_)
        this->pending_connect_ = true;
    boost::asio::async_connect(
        *this->socket_,
        endpoints,
        RBIND(on_connect, _1)
    );
}

void socket::do_reconnect(void) {
    if (this->connected_) return;
    if (this->client_) {
        if (!this->pending_connect_) {
            system::time_t now = system::time::now();
            if (this->last_connect_ + this->reconnect_delay_ < now) {
                if (this->socket_)
                    this->socket_.reset();
                this->do_resolve();
            }
        }
    } else {
        if (this->socket_)
            this->socket_.reset();
        this->service_->listen(this->port_, this);
    }
}

void socket::do_read(void) {
    if (!this->connected_) {
        return this->do_reconnect();
    }
    if (!this->socket_) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "system::socket::do_read", "Socket is null");
        this->connected_ = false;
        if (this->on_disconnect_)
            this->on_disconnect_();
        return;
    }
    if (this->pending_read_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "system::socket::do_read", "Read operation already started");
        return;
    } else this->pending_read_ = true;
    this->read_buffer_.clear();
    this->socket_->async_receive(
        boost::asio::buffer(this->read_buffer_.data(), this->read_buffer_.max()),
        RBIND(on_read, _1, _2)
    );
}

void socket::do_write(void) {
    if (!this->connected_) {
        return this->do_reconnect();
    }
    if (!this->socket_) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "system::socket::do_write", "Socket is null");
        this->connected_ = false;
        if (this->on_disconnect_)
            this->on_disconnect_();
        return;
    }
    if (this->pending_write_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "system::socket::do_write", "Write operation already started");
        return;
    }
    size_t size = this->write_buffer_.size();
    if (size > 0) {
        //sys_debug_print(SYSTEM_LEVEL_TRACE, "system::socket::do_write", "Async send %d bytes", (int)size);
        this->pending_write_ = true;
        this->socket_->async_send(
            boost::asio::buffer(this->write_buffer_.data(), size),
            RBIND(on_write, _1, _2)
        );
    } else {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "system::socket::do_write", "Trying to send with empty buffer");
    }
}

void socket::on_resolve(boost::system::error_code ec, const boost::asio::ip::tcp::resolver::results_type& endpoints) {

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "system::socket::on_resolve", "Resolve failed for %s:%d. Error code %d", this->host_.c_str(), this->port_, ec.value());
        //sys_debug_print(SYSTEM_LEVEL_ERROR, "system::socket::on_resolve", "Resolve failed for %s:%d. %s", this->host_.c_str(), this->port_, ec.message().c_str());
        this->pending_connect_ = false;
        return;
    }

    return this->do_connect(endpoints);
}

void socket::on_connect(boost::system::error_code ec) {

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "system::socket::on_connect", "Connection %s failed. %s", this->host_.c_str(), ec.message().c_str());
        this->pending_connect_ = false;
        this->socket_.reset();
        return;
    }

    this->remote_ip_ = this->socket_->remote_endpoint().address();
    std::stringstream stream;
    stream << this->remote_ip_;
    sys_debug_print(SYSTEM_LEVEL_INFO, "system::socket::on_connect", "Connection established to %s", stream.str().c_str());

    this->connected_ = true;
    this->pending_connect_ = false;
    if (this->on_connect_)
        this->on_connect_();
    if (this->was_connected_) {
        if (this->on_reconnect_) {
            this->on_reconnect_();
        }
    } else {
        this->was_connected_ = true;
    }
}

void socket::on_read(boost::system::error_code ec, size_t length) {

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "system::socket::on_read", "Read failed. Error code %d", ec.value());
        this->pending_read_ = false;
        this->connected_ = false;
        this->socket_->close();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        this->socket_.reset();
        this->last_connect_ = system::time::now();
        if (this->on_disconnect_)
            this->on_disconnect_();
        return;
    }

    this->read_buffer_.write(length);
    this->pending_read_ = false;
}

void socket::on_write(boost::system::error_code ec, size_t length) {

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "system::socket::on_write", "Write failed. Error code %d", ec.value());
        //sys_debug_print(SYSTEM_LEVEL_ERROR, "system::socket::on_write", "Write failed. %s", ec.message().c_str());
        this->pending_write_ = false;
        this->connected_ = false;
        this->socket_->close();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        this->socket_.reset();
        this->last_connect_ = system::time::now();
        if (this->on_disconnect_)
            this->on_disconnect_();
        return;
    }

    this->write_buffer_.read(length);
    size_t size = this->write_buffer_.size();
    if (size > 0) {
        this->socket_->async_send(
            boost::asio::buffer(this->write_buffer_.data(), size),
            RBIND(on_write, _1, _2)
        );
    } else {
        this->pending_write_ = false;
    }
}

}}
