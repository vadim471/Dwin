#include <initp/log/frontend/log.hpp>
#include <initp/log/frontend/tools.hpp>
#include <initp/log/http/logger.hpp>
#include <initp/log/vector/logger.hpp>
#include <initp/platform/tools.hpp>
#include <initp/tools/ip.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <mutex>
#include <thread>
#include <functional>

namespace initp {
namespace log {

namespace {

    // std::cout / std::cerr

class std_out_backend : public log_base {
public:
    std_out_backend(const std_out_config& config)
        : log_base(to_native_level(config.level()))
        , stderr_level_(to_native_level(config.stderr_level()))
    {}
    ~std_out_backend(void) override {}
public:
    int type(void) const override {
        return LO_STREAM;
    }
    void write_line(level::value level, const std::string& what, const std::string& error, const std::string& message) override {
        if (level > this->level_) return;
        std::lock_guard<std::mutex> lock(this->mutex_);
        auto& out = (level >= this->stderr_level_ ? std::cerr : std::cout);
        out << what << "> ";
        if (error.size() > 0) {
            out << "<" << error << "> ";
        }
        out << message;
        out << std::endl;
    }
private:
    std::mutex mutex_;
    level::value stderr_level_;
};

    // boost::log -> file sink

class boost_log_backend : public log_base {
    typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> sync_text_sink;
    typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_file_backend> async_text_sink;
    enum byte_order_mark_t {
        BOM_NONE = 0,
        BOM_UTF8,
        BOM_UTF16_BE,
        BOM_UTF16_LE
    };
public:
    boost_log_backend(const char* app, const boost_log_config& config)
        : log_base(to_native_level(config.level()))
    {
        if (config.async()) {
            this->initialize_async(app, config);
        } else {
            this->initialize_sync(app, config);
        }
    }
    ~boost_log_backend(void) override {
        boost::log::core::get()->set_logging_enabled(false);
        boost::log::core::get()->flush();
        boost::log::core::get()->remove_all_sinks();
        if (this->sync_sink_) {
            this->sync_sink_.reset();
        }
        if (this->async_sink_) {
            this->async_sink_->flush();
            this->async_sink_->stop();
            this->async_sink_.reset();
        }
        if (this->thread_) {
            this->thread_->join();
            this->thread_.reset();
        }
    }
public:
    static void initialize(void) {
        boost::log::core::get()->set_logging_enabled(false);
    }
public:
    int type(void) const override {
        return LO_FILE;
    }
    void write_line(level::value level, const std::string& what, const std::string& error, const std::string& message) override {
        if (level > this->level_) return;
        if (error.size() > 0) {
            BOOST_LOG_SEV(this->instance_, to_boost_level(level)) << "<" << what << "> " << "<" << error << "> " << message;
        } else {
            BOOST_LOG_SEV(this->instance_, to_boost_level(level)) << "<" << what << "> " << message;
        }
    }
    boost::log::sources::severity_logger<boost::log::trivial::severity_level>& instance(void) {
        return this->instance_;
    }
private:
    static void on_open_handler(boost::log::sinks::text_file_backend::stream_type& stream, byte_order_mark_t bom) {
        switch (bom) {
            case BOM_UTF8:
                stream << char(0xEF) << char(0xBB) << char(0xBF);
                break;
            case BOM_UTF16_BE:
                stream << char(0xFE) << char(0xFF);
                break;
            case BOM_UTF16_LE:
                stream << char(0xFF) << char(0xFE);
                break;
            default:
                break;
        }
    }
    static boost::log::trivial::severity_level to_boost_level(level::value value) {
        switch (value) {
            case level::LV_FATAL:
                return boost::log::trivial::fatal;
            case level::LV_ERROR:
                return boost::log::trivial::error;
            case level::LV_WARNING:
                return boost::log::trivial::warning;
            case level::LV_INFO:
                return boost::log::trivial::info;
            case level::LV_DEBUG:
                return boost::log::trivial::debug;
            case level::LV_TRACE:
                return boost::log::trivial::trace;
            default:
                return boost::log::trivial::trace;
        }
    }
    void initialize_sync(const char* app, const boost_log_config& config) {
        namespace fs = boost::filesystem;
        fs::path target(config.path());
        std::string full_name;
        std::stringstream stream;
        stream << u8"%Y-%m-%d--%H-%M-%S--" << app << u8"-%N.log";
        target.append(stream.str());

        boost::shared_ptr<boost::log::sinks::text_file_backend> report_backend(
            boost::make_shared<boost::log::sinks::text_file_backend>(
                boost::log::keywords::file_name = target.generic_string(),
                boost::log::keywords::rotation_size = config.rotation_size() * 1024 * 1024,
                boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
                boost::log::keywords::open_mode = std::ios_base::app,
                boost::log::keywords::auto_flush = true
            )
        );
        report_backend->set_file_collector(
            boost::log::sinks::file::make_collector(
                boost::log::keywords::target = config.path(),
                boost::log::keywords::max_size = config.max_files_size() * 1024 * 1024,
                boost::log::keywords::min_free_space = config.min_free_space() * 1024 * 1024,
                boost::log::keywords::max_files = config.max_files_count()
            )
        );
        report_backend->set_open_handler(std::bind(&boost_log_backend::on_open_handler, std::placeholders::_1, BOM_UTF8));
        report_backend->scan_for_files();

        this->sync_sink_ = boost::make_shared<sync_text_sink>(report_backend);
        this->sync_sink_->set_formatter(
            boost::log::expressions::format("%1% %2% <%3%> %4%")
                % boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f")
                % boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID")
                % boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity")
                % boost::log::expressions::message
        );

        this->sync_sink_->set_filter(boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity") >= boost::log::trivial::trace);
        boost::log::core::get()->add_sink(this->sync_sink_);
        boost::log::add_common_attributes();
        boost::log::core::get()->set_logging_enabled(true);
    }
    void initialize_async(const char* app, const boost_log_config& config) {
        namespace fs = boost::filesystem;
        fs::path target(config.path());
        std::string full_name;
        std::stringstream stream;
        stream << u8"%Y-%m-%d--%H-%M-%S--" << app << u8"-%N.log";
        target.append(stream.str());

        boost::shared_ptr<boost::log::sinks::text_file_backend> report_backend(
            boost::make_shared<boost::log::sinks::text_file_backend>(
                boost::log::keywords::file_name = target.generic_string(),
                boost::log::keywords::rotation_size = config.rotation_size() * 1024 * 1024,
                boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
                boost::log::keywords::open_mode = std::ios_base::app,
                boost::log::keywords::auto_flush = true
            )
        );
        report_backend->set_file_collector(
            boost::log::sinks::file::make_collector(
                boost::log::keywords::target = config.path(),
                boost::log::keywords::max_size = config.max_files_size() * 1024 * 1024,
                boost::log::keywords::min_free_space = config.min_free_space() * 1024 * 1024,
                boost::log::keywords::max_files = config.max_files_count()
            )
        );
        report_backend->set_open_handler(std::bind(&on_open_handler, std::placeholders::_1, BOM_UTF8));
        report_backend->scan_for_files();

        this->async_sink_ = boost::make_shared<async_text_sink>(report_backend, false);
        this->async_sink_->set_formatter(
            boost::log::expressions::format("%1% %2% <%3%> %4%")
                % boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f")
                % boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID")
                % boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity")
                % boost::log::expressions::message
        );
        this->async_sink_->set_filter(boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity") >= boost::log::trivial::trace);
        boost::log::core::get()->add_sink(this->async_sink_);
        boost::log::add_common_attributes();
        this->thread_.reset(new (std::nothrow) std::thread([this] (void) -> void { this->async_sink_->run(); }));
        boost::log::core::get()->set_logging_enabled(true);
    }
private:
    boost::log::sources::severity_logger<boost::log::trivial::severity_level> instance_;
    boost::shared_ptr<sync_text_sink> sync_sink_;
    boost::shared_ptr<async_text_sink> async_sink_;
    std::unique_ptr<std::thread> thread_;
};

    // legacy http

class http_initp_backend : public log_base {
public:
    http_initp_backend(const char* app, const http_initp_config& config)
        : log_base(to_native_level(config.level()))
    {
        if (config.has_ssl()) {
            this->instance_.set_certificate(
                config.ssl().ca(),
                config.ssl().cl(),
                config.ssl().key()
            );
        }
        std::string host, port;
        std::tie(host, port) = tools::ip::split_address(config.host(), "80");
        this->instance_.initialize(
            host,
            port,
            config.query(),
            system::tools::get_host_name(),
            app,
            config.delay(),
            config.max_queue_size(),
            config.max_batch_size()
        );
        this->instance_.start();
    }
    ~http_initp_backend(void) override {
        if (this->instance_.started()) {
            this->instance_.disable();
            this->instance_.flush();
            this->instance_.stop();
        }
    }
public:
    int type(void) const override {
        return LO_NET;
    }
    void write_line(level::value level, const std::string& what, const std::string& error, const std::string& message) override {
        if (level > this->level_) return;
        if (error.size() > 0) {
            std::stringstream stream;
            stream << "<" << error << "> ";
            stream << message;
            this->instance_.queue_message(
                level,
                what,
                stream.str(),
                system::time::now()
            );
        } else {
            this->instance_.queue_message(
                level,
                what,
                message,
                system::time::now()
            );
        }
    }
    http::logger& instance(void) {
        return this->instance_;
    }
private:
    http::logger instance_;
};

    // vector.io

class vector_backend : public log_base {
public:
    vector_backend(const char* app, const vector_config& config)
        : log_base(to_native_level(config.level()))
    {
        if (config.has_ssl()) {
            this->instance_.set_certificate(
                config.ssl().ca(),
                config.ssl().cl(),
                config.ssl().key()
            );
        }
        std::string host, port;
        std::tie(host, port) = tools::ip::split_address(config.host(), "80");
        this->instance_.initialize(
            host,
            port,
            app,
            system::tools::get_cpu_id(),
            config.delay(),
            config.max_queue_size(),
            config.max_batch_size()
        );
        this->instance_.start();
    }
    ~vector_backend(void) override {
        if (this->instance_.started()) {
            this->instance_.disable();
            this->instance_.flush();
            this->instance_.stop();
        }
    }
public:
    int type(void) const override {
        return LO_NET;
    }
    void write_line(level::value level, const std::string& what, const std::string& error, const std::string& message) override {
        if (level > this->level_) return;
        this->instance_.log_message(level, error, what, message);
    }
    vector::logger& instance(void) {
        return this->instance_;
    }
private:
    vector::logger instance_;
};

}

    // Construction

log_base::log_base(level::value level)
    : level_(level)
{}

log_base::~log_base(void) {}

    // Static methods

void log_base::initialize(void) {
    boost_log_backend::initialize();
}

    // Factory methods

std::unique_ptr<log_base> log_base::create(const std_out_config& config) {
    return std::make_unique<std_out_backend>(config);
}

std::unique_ptr<log_base> log_base::create(const char* app, const boost_log_config& config) {
    return std::make_unique<boost_log_backend>(app, config);
}

std::unique_ptr<log_base> log_base::create(const char* app, const http_initp_config& config) {
    return std::make_unique<http_initp_backend>(app, config);
}

std::unique_ptr<log_base> log_base::create(const char* app, const vector_config& config) {
    return std::make_unique<vector_backend>(app, config);
}

    // Find functions

template<typename T>
T* find(void);

template<>
boost::log::sources::severity_logger<boost::log::trivial::severity_level>* find(void) {
    for (const auto& logger : get_loggers()) {
        if (boost_log_backend* backend = dynamic_cast<boost_log_backend*>(logger.get())) {
            return &backend->instance();
        }
    }
    return nullptr;
}

template<>
http::logger* find(void) {
    for (const auto& logger : get_loggers()) {
        if (http_initp_backend* backend = dynamic_cast<http_initp_backend*>(logger.get())) {
            return &backend->instance();
        }
    }
    return nullptr;
}

template<>
vector::logger* find(void) {
    for (const auto& logger : get_loggers()) {
        if (vector_backend* backend = dynamic_cast<vector_backend*>(logger.get())) {
            return &backend->instance();
        }
    }
    return nullptr;
}

}}
