#include <initp/system/error_code.hpp>
#include <initp/log/vector/logger.hpp>
#include <initp/socket/raw/basic_connection.hpp>
#include <initp/socket/raw/ssl_connection.hpp>
#include <initp/utils/date.hpp>

#include <boost/algorithm/string.hpp>
#include <sstream>

namespace initp {
namespace log {
namespace vector {

using namespace std::placeholders;

    // Construction

logger::logger(void):
    device_name_(),
    device_id_(),
    started_(false),
    enabled_(false),
    flush_(false),
    max_queue_size_(100),
    max_batch_size_(20),
    remote_host_(),
    remote_port_(),
    strand_(context_),
    ssl_context_(boost::asio::ssl::context::tlsv12_client),
    timer_(context_),
    ssl_context_used_(false),
    delay_(0),
    sequence_id_(0)
{}

    // Public methods

void logger::set_certificate(
    const std::string& ca,
    const std::string& cl,
    const std::string& key
) {
    if (this->started_) return;
    socket::raw::ssl_connection::initialize_ssl(this->ssl_context_, ca, cl, key);
    this->ssl_context_used_ = true;
}

void logger::initialize(
    const std::string& host,
    const std::string& port,
    const std::string& device_name,
    const std::string& device_id,
    system::time_t delay,
    size_t max_queue_size,
    size_t max_batch_size
) {
    if (this->started_) return;
    if (delay < 1000)
        delay = 1000;
    this->delay_ = delay;
    this->remote_host_ = host;
    this->remote_port_ = port;
    this->device_name_ = device_name;
    this->device_id_ = device_id;
    this->max_queue_size_ = max_queue_size;
    this->max_batch_size_ = max_batch_size;
}

void logger::reinitialize(
    const std::string& device_name,
    const std::string& device_id
) {
    if (this->started_) {
        if (this->device_name_.compare(device_name) ||
            this->device_id_.compare(device_id)) {
            this->flush();
        }
    }
    if (this->device_name_.compare(device_name))
        this->device_name_ = device_name;
    if (this->device_id_.compare(device_id))
        this->device_id_ = device_id;
}

void logger::start(bool enable) {

    if (this->started_) return;

    if (this->ssl_context_used_) {
        this->connection_ = std::make_shared<socket::raw::ssl_connection>(this->context_, this->ssl_context_);
    } else {
        this->connection_ = std::make_shared<socket::raw::basic_connection>(this->context_);
    }

    this->timer_.expires_after(std::chrono::milliseconds(this->delay_));
    this->timer_.async_wait(boost::asio::bind_executor(this->strand_, RBIND(on_timer, _1)));

    this->started_ = true;
    this->enabled_ = enable;

    this->thread_.reset(new std::thread([this] (void) -> void {
        boost::system::error_code ec;
        sys_debug_print(SYSTEM_LEVEL_INFO, "log::vector::logger::start", ">>> Logger thread started");
        this->context_.run(ec);
        sys_debug_print(SYSTEM_LEVEL_INFO, "log::vector::logger::start", "<<< Logger thread stopped");
    }));
}

void logger::stop(void) {
    if (!this->started_) return;
    this->started_ = false;
    if (this->connection_) {
        if (this->connection_->connected()) {
            this->connection_->close();
            system::time::sleep_for(200);
        }
        this->connection_.reset();
    }
    this->context_.stop();
    if (this->thread_) {
        this->thread_->join();
        this->thread_.reset();
    }
    system::time::sleep_for(200);
    this->context_.restart();
}

void logger::flush(void) {
    if (!this->started_) return;
    if (this->flush_) return;
    this->flush_ = true;

    boost::asio::post(boost::asio::bind_executor(this->strand_, RBIND(do_flush)));

    for (size_t i = 0; i < 25 && this->flush_; ++i) {
        system::time::sleep_for(200);
    }

    if (this->flush_) {
        this->flush_ = false;
        sys_debug_print(SYSTEM_LEVEL_ERROR, "log::vector::logger::flush", "Failed to flush logger");
    }
}

void logger::enable(void) {
    if (!this->started_) return;
    boost::asio::post(
        boost::asio::bind_executor(
            this->strand_,
            RBIND(set_enabled, true)
        )
    );
}

void logger::disable(void) {
    if (!this->started_) return;
    boost::asio::post(
        boost::asio::bind_executor(
            this->strand_,
            RBIND(set_enabled, false)
        )
    );
}

void logger::queue_message(message_t& message) {
    if (!this->started_ ||
        !this->enabled_) return;
    boost::asio::post(
        boost::asio::bind_executor(
            this->strand_,
            RBIND(push_message, message)
        )
    );
}

    // Private methods

void logger::push_message(message_t& message) {
    if (!this->started_ ||
        !this->enabled_) return;
    this->queue_.emplace(std::move(message));
    while (this->queue_.size() > this->max_queue_size_) {
        this->queue_.pop();
    }
    if (!this->connection_) return;
    if (this->queue_.size() >= this->max_batch_size_ &&
        !this->connection_->connected() &&
        !this->connection_->busy()) {
        system::error_code ec = this->connection_->connect(this->remote_host_, this->remote_port_, RBIND(on_connect, _1));
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "log::vector::logger::push_message", "Failed to connect to %s", this->remote_host_.c_str());
        }
    }
}

void logger::batch_messages(void) {
    if (!this->started_) return;
    if (this->queue_.empty()) return;
    event::EventArray batch;
    event::LogArray* logs = batch.mutable_logs();
    for (size_t i = 0; i < this->max_batch_size_ && !this->queue_.empty(); ++i) {
        event::Log* log = logs->add_logs();
        event::Value* value = log->mutable_value();
        event::ValueMap* content = value->mutable_map();
        *content->mutable_fields() = std::move(this->queue_.front());
        this->queue_.pop();
    }
    if (logs->logs_size() > 0) {
        size_t size = batch.ByteSizeLong();
        this->data_to_send_.resize(size + 4);
        if (!batch.SerializeToArray(this->data_to_send_.data() + 4, size)) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "log::vector::logger::batch_messages", "Failed to serialize protobuf");
            return;
        }
        *((int32_t*)this->data_to_send_.data()) = size;
        std::reverse(this->data_to_send_.data(), this->data_to_send_.data() + 4);
    }
}

void logger::send_messages(void) {
    if (!this->started_) return;
    if (!this->connection_) return;
    if (this->connection_->busy()) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "log::vector::logger::send_messages", "Requested message send, but connection are busy");
        return;
    }
    system::error_code ec = this->connection_->write(this->data_to_send_, RBIND(on_write, _1));
    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "log::vector::logger::send_messages", "Failed to post messages batch");
        this->timer_.expires_after(std::chrono::milliseconds(this->delay_));
        this->connection_->close();
    }
}

void logger::do_flush(void) {
    if (!this->started_) return;
    if (!this->connection_) return;
    if (!this->data_to_send_.empty() || !this->queue_.empty()) {
        if (!this->connection_->connected()) {
            system::error_code ec = this->connection_->connect(this->remote_host_, this->remote_port_, RBIND(on_connect, _1));
            if (ec) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "log::vector::logger::do_flush", "Failed to connect to %s", this->remote_host_.c_str());
            }
        }
    } else {
        this->flush_ = false;
    }
}

void logger::set_enabled(bool value) {
    this->enabled_ = value;
}

    // Callbacks

void logger::on_connect(boost::system::error_code ec) {
    if (ec) return;
    if (this->data_to_send_.empty()) {
        this->batch_messages();
        if (this->data_to_send_.empty()) {
            this->connection_->close();
            if (this->flush_) this->flush_ = false;
        } else {
            this->send_messages();
        }
    } else {
        this->send_messages();
    }
}

void logger::on_write(boost::system::error_code ec) {
    if (!this->started_) return;
    if (!this->connection_) return;
    this->timer_.expires_after(std::chrono::milliseconds(this->delay_));
    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::log::vector::logger::on_response", "Failed to send data or get response");
        this->connection_->close();
        return;
    }

    //sys_debug_print(SYSTEM_LEVEL_TRACE, "log::vector::logger::on_response", "Messages was sent");
    this->data_to_send_.clear();
    if (!this->queue_.empty()) {
        this->batch_messages();
        if (this->data_to_send_.empty()) {
            this->connection_->close();
        } else {
            this->send_messages();
        }
    } else {
        this->connection_->close();
        if (this->flush_) {
            this->flush_ = false;
        }
    }
}

void logger::on_timer(boost::system::error_code ec) {

    if (ec == boost::system::errc::operation_canceled) {
        // Nothing need to do
    } else if (ec) {
        sys_debug_print(SYSTEM_LEVEL_INFO, "log::vector::logger::on_timer", "%s (%d)", ec.message().c_str(), ec.value());
        return;
    }

    if (this->timer_.expiry() <= boost::asio::steady_timer::clock_type::now()) {
        if (!this->connection_->connected() &&
            (!this->data_to_send_.empty() || !this->queue_.empty())) {
            system::error_code ec = this->connection_->connect(this->remote_host_, this->remote_port_, RBIND(on_connect, _1));
            if (ec) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "log::vector::logger::on_timer", "Failed to connect to %s", this->remote_host_.c_str());
            }
        }
    }

    // Put the actor back to sleep.
    if (this->delay_ > 0) {
        this->timer_.expires_after(std::chrono::milliseconds(this->delay_));
        this->timer_.async_wait(boost::asio::bind_executor(this->strand_, RBIND(on_timer, _1)));
    }
}

    // Public properties

bool logger::started(void) const {
    return this->started_;
}

bool logger::enabled(void) const {
    return this->enabled_;
}

const std::string& logger::device_name(void) const {
    return this->device_name_;
}

const std::string& logger::device_id(void) const {
    return this->device_id_;
}

}}}
