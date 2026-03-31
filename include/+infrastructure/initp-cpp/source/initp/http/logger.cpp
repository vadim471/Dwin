#include <initp/system/error_code.hpp>
#include <initp/http/logger.hpp>
#include <initp/http/binary/ssl_connection.hpp>
#include <initp/http/DataContracts.pb.h>
#include <initp/utils/date.hpp>

#include <sstream>

namespace initp {
namespace http {

using namespace std::placeholders;

    // Construction

logger::logger(void):
    start_time_(0),
    max_queue_size_(100),
    max_batch_size_(20),
    remote_host_(),
    remote_port_(),
    remote_target_(),
    local_host_(),
    application_name_(),
    strand_(context_),
    ssl_context_(boost::asio::ssl::context::tlsv12_client),
    timer_(context_),
    started_(false),
    enabled_(false),
    flush_(false),
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
    http::binary::ssl_connection::initialize_ssl(this->ssl_context_, ca, cl, key);
}

void logger::initialize(
    const std::string& host,
    const std::string& port,
    const std::string& target,
    const std::string& local_name,
    const std::string& application,
    system::time_t delay,
    size_t max_queue_size,
    size_t max_batch_size
) {
    if (this->started_) return;
    if (delay < 1000)
        delay = 1000;
    this->delay_ = delay;
    this->start_time_ = system::time::milliseconds();
    this->remote_host_ = host;
    this->remote_port_ = port;
    this->remote_target_ = target;
    this->local_host_ = local_name;
    this->application_name_ = application;
    this->max_queue_size_ = max_queue_size;
    this->max_batch_size_ = max_batch_size;
}

void logger::reinitialize(
    system::time_t session_time,
    const std::string& local_name,
    const std::string& application
) {
    if (this->started_) {
        if (this->start_time_ != session_time ||
            this->local_host_.compare(local_name) ||
            this->application_name_.compare(application)) {
            this->flush();
        }
    }
    if (this->start_time_ != session_time)
        this->start_time_ = session_time;
    if (this->local_host_.compare(local_name))
        this->local_host_ = local_name;
    if (this->application_name_.compare(application))
        this->application_name_ = application;
}

void logger::start(bool enable) {

    if (this->started_) return;

    this->connection_ = std::make_shared<http::binary::ssl_connection>(this->context_, this->ssl_context_);

    this->timer_.expires_after(std::chrono::milliseconds(this->delay_));
    this->timer_.async_wait(boost::asio::bind_executor(this->strand_, RBIND(on_timer, _1)));

    this->started_ = true;
    this->enabled_ = enable;

    this->thread_.reset(new std::thread([this] (void) -> void {
        boost::system::error_code ec;
        sys_debug_print(SYSTEM_LEVEL_INFO, "http::logger::start", ">>> Logger thread started");
        this->context_.run(ec);
        sys_debug_print(SYSTEM_LEVEL_INFO, "http::logger::start", "<<< Logger thread stopped");
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

    for (size_t i = 0; i < 100 && this->flush_; ++i) {
        system::time::sleep_for(200);
    }

    if (this->flush_) {
        this->flush_ = false;
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::logger::flush", "Failed to flush logger");
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

void logger::queue_message(
    uint8_t level,
    const std::string& what,
    const std::string& message,
    system::time_t time
) {
    if (!this->started_ ||
        !this->enabled_) return;
    boost::asio::post(
        boost::asio::bind_executor(
            this->strand_,
            RBIND(push_message, level, what, message, time)
        )
    );
}

    // Private methods

void logger::push_message(
    uint8_t level,
    const std::string& what,
    const std::string& message,
    system::time_t time
) {
    if (!this->started_ ||
        !this->enabled_) return;
    this->queue_.emplace(level, what, message, time);
    while (this->queue_.size() > this->max_queue_size_) {
        this->queue_.pop();
    }
    if (!this->connection_) return;
    if (this->queue_.size() >= this->max_batch_size_ &&
        !this->connection_->connected() &&
        !this->connection_->busy()) {
        system::error_code ec = this->connection_->connect(this->remote_host_, this->remote_port_, RBIND(on_connect, _1));
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::logger::push_message", "Failed to connect to %s", this->remote_host_.c_str());
        }
    }
}

UniversalLogger::ProtobufDto::LogLevelDto convert_level(uint8_t level) {
    switch (level) {
        case SYSTEM_LEVEL_FATAL:    return UniversalLogger::ProtobufDto::LogLevelDto::Fatal;
        case SYSTEM_LEVEL_ERROR:    return UniversalLogger::ProtobufDto::LogLevelDto::Error;
        case SYSTEM_LEVEL_WARNING:  return UniversalLogger::ProtobufDto::LogLevelDto::Warn;
        case SYSTEM_LEVEL_INFO:     return UniversalLogger::ProtobufDto::LogLevelDto::Info;
        case SYSTEM_LEVEL_DEBUG:    return UniversalLogger::ProtobufDto::LogLevelDto::Debug;
        case SYSTEM_LEVEL_TRACE:
        default:                    return UniversalLogger::ProtobufDto::LogLevelDto::Trace;
    }
}

void logger::batch_messages(void) {
    if (!this->started_) return;
    if (this->queue_.empty()) return;
    UniversalLogger::ProtobufDto::LogRecordsBatchDto batch;
    batch.set_hostname(this->local_host_);
    batch.set_appname(this->application_name_);
    batch.set_startupglobaltimestamp(this->to_server_time_format(this->start_time_));
    batch.set_startuplocaltimestamp(this->to_local_time(this->start_time_));
    for (size_t i = 0; i < this->max_batch_size_ && !this->queue_.empty(); ++i) {
        UniversalLogger::ProtobufDto::LogRecordsBatchItemDto* item = batch.add_items();
        if (!item) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::logger::batch_messages", "Failed to add item to protobuf container");
            break;
        }
        log_message_t& message = this->queue_.front();
        item->set_sequenceid(++this->sequence_id_);
        item->set_globaltimestamp(this->to_server_time_format(std::get<3>(message)));
        item->set_localtimestamp(this->to_local_time(std::get<3>(message)));
        item->set_level(convert_level(std::get<0>(message)));
        item->set_internalname(std::get<1>(message));
        item->set_message(std::get<2>(message));
        this->queue_.pop();
    }
    if (batch.items_size() > 0) {
        size_t size = batch.ByteSizeLong();
        this->data_to_send_.resize(size);
        if (!batch.SerializeToArray(&this->data_to_send_[0], size)) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::logger::batch_messages", "Failed to serialize protobuf");
            return;
        }
    }
}

void logger::send_messages(void) {
    if (!this->started_) return;
    if (!this->connection_) return;
    if (this->connection_->busy()) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::logger::send_messages", "Requested message send, but connection are busy");
        return;
    }
    http::binary::connection::header_list_t headers;
    headers[boost::beast::http::field::host] = this->remote_host_ + ":" + this->remote_port_;
    headers[boost::beast::http::field::user_agent] = "Init-Plus HTTP logger client";
    headers[boost::beast::http::field::accept] = "application/x-protobuf";
    headers[boost::beast::http::field::content_type] = "application/x-protobuf";
    headers[boost::beast::http::field::expect] = "";
    system::error_code ec = this->connection_->post(this->remote_target_, headers, this->data_to_send_, RBIND(on_response, _1, _2, _4, _5));
    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::logger::send_messages", "Failed to post messages batch");
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
                sys_debug_print(SYSTEM_LEVEL_ERROR, "http::logger::do_flush", "Failed to connect to %s", this->remote_host_.c_str());
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

void logger::on_response(boost::system::error_code ec, boost::beast::http::status status, const uint8_t* body, size_t size) {
    if (!this->started_) return;
    if (!this->connection_) return;
    this->timer_.expires_after(std::chrono::milliseconds(this->delay_));
    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::http::logger::on_response", "Failed to send data or get response");
        this->connection_->close();
    } else if (status != boost::beast::http::status::ok) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::http::logger::on_response", "Server returned status %d", (int)status);
        this->connection_->close();
    } else {
        if (!body) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "http::logger::on_response", "No error, but data is null");
            this->connection_->close();
            return;
        }
        UniversalLogger::ProtobufDto::ApiResultDto result;
        result.set_apiresultstatus(UniversalLogger::ProtobufDto::ApiResultStatusDto::UnknownError);
        if (size) {
            if (!result.ParseFromArray(body, size)) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "http::logger::on_response", "Failed to parse response protobuf");
                this->connection_->close();
            } else {
                if (!result.apiresultstatus()) {
                    sys_debug_print(SYSTEM_LEVEL_ERROR, "http::logger::on_response", "Server respond with failed status");
                    this->connection_->close();
                } else {
                    //sys_debug_print(SYSTEM_LEVEL_TRACE, "http::logger::on_response", "Messages was sent");
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
                        if (this->flush_) this->flush_ = false;
                    }
                }
            }
        } else {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::logger::on_response", "Response body size is null");
            this->connection_->close();
        }
    }
}

void logger::on_timer(boost::system::error_code ec) {

    if (ec == boost::system::errc::operation_canceled) {
        // Nothing need to do
    } else if (ec) {
        sys_debug_print(SYSTEM_LEVEL_INFO, "http::logger::on_timer", "%s (%d)", ec.message().c_str(), ec.value());
        return;
    }

    if (this->timer_.expiry() <= boost::asio::steady_timer::clock_type::now()) {
        if (!this->connection_->connected() &&
            (!this->data_to_send_.empty() || !this->queue_.empty())) {
            system::error_code ec = this->connection_->connect(this->remote_host_, this->remote_port_, RBIND(on_connect, _1));
            if (ec) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "http::logger::on_timer", "Failed to connect to %s", this->remote_host_.c_str());
            }
        }
    }

    // Put the actor back to sleep.
    if (this->delay_ > 0) {
        this->timer_.expires_after(std::chrono::milliseconds(this->delay_));
        this->timer_.async_wait(boost::asio::bind_executor(this->strand_, RBIND(on_timer, _1)));
    }
}

    // Time functions

int64_t logger::to_server_time_format(system::time_t time) {
    int64_t value(time);
    value += 62135596800000L;
    value *= 10000L;
    return value;
}

int64_t logger::to_local_time(system::time_t time) {
    int64_t value(time);
    int64_t timezone = utils::date::timezone_milliseconds();
    if (timezone < 0) {
        if (value > -timezone) {
            value += timezone;
        } else value = 0;
    } else {
        value += timezone;
    }
    value += 62135596800000L;
    value *= 10000L;
    return value;
}

    // Public properties

bool logger::started(void) const {
    return this->started_;
}

bool logger::enabled(void) const {
    return this->enabled_;
}

system::time_t logger::start_time(void) const {
    return this->start_time_;
}

const std::string& logger::local_host(void) const {
    return this->local_host_;
}

const std::string& logger::application_name(void) const {
    return this->application_name_;
}

}}
