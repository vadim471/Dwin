#include <initp/http/server/server.hpp>
#include <initp/http/server/connection.hpp>
#include <initp/http/server/pool.hpp>
#include <initp/http/server/config.pb.h>
#include <initp/system/cdebug>
#include <initp/system/time.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/steady_timer.hpp>

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace {

constexpr const char* default_host = "0.0.0.0";
constexpr int default_port = 80;

constexpr uint32_t default_auth_timeout = 10;
constexpr uint32_t default_session_timeout = 600;
constexpr uint32_t default_max_sessions = 10;

constexpr const char* default_files_root = "doc";

constexpr const char* default_private_key = "server-dS89kPt";
constexpr const char* default_realm = "tools@init-plus.com";
constexpr const char* default_username = "user";
constexpr const char* default_password = "test";

class http_server : public initp::http::server {
    typedef http_server self_type;
    using tcp = boost::asio::ip::tcp;
public: // Construction
    http_server(const initp::http::server_config& config)
        : config_(config)
        , pool_(
            config.auth_timeout() * 1000,
            config.session_timeout() * 1000,
            config.max_sessions()
        )
        , started_(false)
        , port_(config.port())
    {
        if (config.has_ssl()) {
            this->create_ssl_context(config.ssl());
        }
        this->create_endpoint(default_host, this->port_);
    }
    http_server(const self_type&) = delete;
    http_server(self_type&&) = delete;
    ~http_server(void) override {
        this->stop();
    }
public: // Public methods
    bool serve(int threads) override {
        if (this->started_) {
            sys_debug_print(SYSTEM_LEVEL_WARNING, "http::http_server::serve", "Server already started");
            return false;
        }

        boost::system::error_code ec;
        this->threads_.reserve(threads);
        this->context_.reset(new boost::asio::io_context(threads));
        this->timer_.reset(new boost::asio::steady_timer(*this->context_, (std::chrono::steady_clock::time_point::max)()));
        this->acceptor_.reset(new tcp::acceptor(*this->context_));
        this->socket_.reset(new tcp::socket(*this->context_));

        // Open the acceptor
        this->acceptor_->open(this->endpoint_->protocol(), ec);
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "http::http_server::serve", "Failed to open acceptor");
            sys_debug_print(SYSTEM_LEVEL_FATAL, "http::http_server::serve", "%s", ec.message().c_str());
            return false;
        }

        // Allow address reuse
        this->acceptor_->set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "http::http_server::serve", "Failed to set address reuse");
            sys_debug_print(SYSTEM_LEVEL_FATAL, "http::http_server::serve", "%s", ec.message().c_str());
            return false;
        }

        // Bind to the server address
        this->acceptor_->bind(*this->endpoint_, ec);
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "http::http_server::serve", "Failed to bind address");
            sys_debug_print(SYSTEM_LEVEL_FATAL, "http::http_server::serve", "%s", ec.message().c_str());
            return false;
        }

        // Start listening for connections
        this->acceptor_->listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "http::http_server::serve", "Failed to listen address");
            sys_debug_print(SYSTEM_LEVEL_FATAL, "http::http_server::serve", "%s", ec.message().c_str());
            return false;
        }

        // Accept connections
        if (!this->acceptor_->is_open()) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "http::http_server::serve", "Acceptor not opened");
            return false;
        }
        this->do_accept();

        // Start threads
        for (int i = threads; i > 0; --i) {
            this->threads_.emplace_back([this] {
                this->context_->run();
            });
        }

        // Set the timer
        on_timer({});
        this->started_ = true;
        return true;
    }
    void stop(void) override {
        if (!this->started_) return;

        if (this->context_) {
            this->context_->stop();
        }
        for (auto& thread : this->threads_) {
            thread.join();
        }

        this->threads_.clear();
        this->acceptor_.reset();
        this->socket_.reset();
        this->context_.reset();
        this->started_ = false;
    }
private: // Private methods
    void create_ssl_context(const initp::ssl::certificate_config& config) {
        boost::system::error_code ec;
        this->ssl_context_.reset(new boost::asio::ssl::context(boost::asio::ssl::context::sslv23));
        this->ssl_context_->load_verify_file(config.ca(), ec);
        if (!ec) this->ssl_context_->use_certificate_chain_file(config.cl(), ec);
        if (!ec) this->ssl_context_->use_private_key_file(config.key(), boost::asio::ssl::context::pem, ec);
        if (ec) {
            this->ssl_context_.reset();
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::http_server::create_ssl_context", "Failed to load SSL certificate");
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::http_server::create_ssl_context", "%s", ec.message().c_str());
            return;
        }
        sys_debug_print(SYSTEM_LEVEL_INFO, "http::http_server::create_ssl_context", "Loaded SSL certificate");
    }
    void create_endpoint(const char* host, int port) {
        this->endpoint_.reset(new tcp::endpoint(boost::asio::ip::make_address(host), port));
    }
    void do_accept(void) {
        this->acceptor_->async_accept(
            *this->socket_,
            std::bind(
                &http_server::on_accept,
                this,
                std::placeholders::_1
            )
        );
    }
    void on_accept(boost::system::error_code ec) {
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::http_server::on_accept", "Operation failed. %s", ec.message().c_str());
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return;
        }

        // Create the session and run it
        if (this->ssl_context_) {
            std::make_shared<initp::http::http_connection>(
                *this->context_,
                *this->ssl_context_,
                this->config_,
                this->pool_,
                std::move(*this->socket_)
            )->run();
        } else {
            std::make_shared<initp::http::http_connection>(
                *this->context_,
                this->config_,
                this->pool_,
                std::move(*this->socket_)
            )->run();
        }

        // Accept another connection
        this->do_accept();
    }
    void on_timer(boost::system::error_code ec) {

        if (ec) {
            if (ec != boost::asio::error::operation_aborted) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "http::http_server::on_timer", "Operation failed. %s", ec.message().c_str());
            }
            return;
        }

        if (this->timer_->expiry() <= std::chrono::steady_clock::now()) {
            this->pool_.poll(initp::system::time::now());
        }

        this->timer_->expires_after(std::chrono::seconds(1));
        this->timer_->async_wait(
            std::bind(
                &http_server::on_timer,
                this,
                std::placeholders::_1
            )
        );
    }
private: // Private fields
    std::unique_ptr<boost::asio::io_context> context_;
    std::unique_ptr<boost::asio::steady_timer> timer_;
    std::unique_ptr<boost::asio::ssl::context> ssl_context_;
    std::unique_ptr<tcp::endpoint> endpoint_;
    std::unique_ptr<tcp::acceptor> acceptor_;
    std::unique_ptr<tcp::socket> socket_;
    std::vector<std::thread> threads_;
    initp::http::server_config config_;
    initp::http::session_pool pool_;
    std::atomic<bool> started_;
    int port_;
};

}

namespace initp {
namespace http {

    // Construction

server::~server(void) {}

    // Factory methods

std::unique_ptr<server> server::create(void) {
    server_config config;
    config.set_port(default_port);
    config.set_auth_timeout(default_auth_timeout);
    config.set_session_timeout(default_session_timeout);
    config.set_max_sessions(default_max_sessions);
    config.set_files_root(default_files_root);
    config.mutable_authorization()->set_private_key(default_private_key);
    config.mutable_authorization()->set_realm(default_realm);
    auto* user = config.mutable_authorization()->add_users();
    user->set_name(default_username);
    user->set_password(default_password);
    return create(config);
}

std::unique_ptr<server> server::create(const server_config& config) {
    return std::make_unique<http_server>(config);
}

}}
