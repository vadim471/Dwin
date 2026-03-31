#ifndef INITPLUS_HTTP_SERVER_HADNLER_HPP_INCLUDED
#define INITPLUS_HTTP_SERVER_HADNLER_HPP_INCLUDED

#include <initp/http/server/connection.hpp>
#include <initp/http/server/pool.hpp>
#include <initp/http/server/config.pb.h>
#include <initp/http/query.hpp>
#include <initp/http/cookies.hpp>
#include <initp/http/auth/digest.hpp>
#include <initp/system/macro.hpp>
#include <initp/system/cdebug>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/config.hpp>

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#ifndef DEVICE_HTTP_SESSION_KEY
#define DEVICE_HTTP_SESSION_KEY "session_id"
#endif // DEVICE_HTTP_SESSION_KEY

namespace initp {
namespace http {

template<class Send>
class http_handler: public std::enable_shared_from_this<http_handler<Send>> {
    typedef http_handler<Send> self_type;
    typedef std::function<boost::beast::http::response<boost::beast::http::empty_body>(void)> empty_response_t;
    typedef std::function<boost::beast::http::response<boost::beast::http::empty_body>(const std::string&)> empty_response_paramed_t;
    typedef std::function<boost::beast::http::response<boost::beast::http::string_body>(boost::beast::string_view)> string_response_t;
    typedef std::function<boost::beast::http::response<boost::beast::http::string_body>(boost::beast::string_view, boost::beast::string_view)> async_response_t;

public: // Construction
    http_handler(void) = delete;
    http_handler(const http_handler&) = delete;
    http_handler(http_handler&&) = delete;
    http_handler(
        http_connection::ptr session,
        boost::beast::http::request<boost::beast::http::string_body>&& request,
        Send& send,
        void* work
    )
        : config_(session->config())
        , session_(session)
        , request_(request)
        , timer_(session->strand().context(), (std::chrono::steady_clock::time_point::max)())
        , query_({ request_.target().data(), request_.target().size() })
        , send_(send)
        , work_(work)
        , accepted([this](boost::beast::string_view location, boost::beast::string_view body) {
            boost::beast::http::response<boost::beast::http::string_body> response { boost::beast::http::status::bad_request, this->request_.version() };
            response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(boost::beast::http::field::content_type, "text/html");
            response.set(boost::beast::http::field::content_location, location);
            this->append_cookies(response);
            response.keep_alive(this->request_.keep_alive());
            response.body() = body
                #if BOOST_VERSION <= 107700
                    .to_string()
                #endif // BOOST_VERSION
            ;
            response.prepare_payload();
            return response;
        })
        , no_content([this](void) {
            boost::beast::http::response<boost::beast::http::empty_body> response { boost::beast::http::status::no_content, this->request_.version() };
            response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(boost::beast::http::field::content_type, "text/html");
            this->append_cookies(response);
            response.keep_alive(this->request_.keep_alive());
            response.prepare_payload();
            return response;
        })
        , moved_permanently([this](const std::string& location) {
            boost::beast::http::response<boost::beast::http::empty_body> response { boost::beast::http::status::moved_permanently, this->request_.version() };
            response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(boost::beast::http::field::location, location);
            this->append_cookies(response);
            response.keep_alive(this->request_.keep_alive());
            response.prepare_payload();
            return response;
        })
        , bad_request([this](boost::beast::string_view why) {
            boost::beast::http::response<boost::beast::http::string_body> response { boost::beast::http::status::bad_request, this->request_.version() };
            response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(boost::beast::http::field::content_type, "text/html");
            this->append_cookies(response);
            response.keep_alive(this->request_.keep_alive());
            response.body() = why
                #if BOOST_VERSION <= 107700
                    .to_string()
                #endif // BOOST_VERSION
            ;
            response.prepare_payload();
            return response;
        })
        , unauthorized([this](const std::string& session_id) {
            boost::beast::http::response<boost::beast::http::empty_body> response { boost::beast::http::status::unauthorized, this->request_.version() };
            response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            this->append_cookies(response);
            if (!session_id.empty()) {
                if (this->config_.has_authorization()) {
                    response.set(
                        boost::beast::http::field::www_authenticate,
                        initp::http::auth::digest::generate(
                            this->config_.authorization().private_key(),
                            this->config_.authorization().realm()
                        )
                    );
                }
                response.set(boost::beast::http::field::set_cookie, initp::http::cookies::make(DEVICE_HTTP_SESSION_KEY, session_id, "/"));
            }
            response.set(boost::beast::http::field::content_type, "text/html");
            response.keep_alive(this->request_.keep_alive());
            response.prepare_payload();
            return response;
        })
        , forbidden([this](void) {
            boost::beast::http::response<boost::beast::http::empty_body> response { boost::beast::http::status::forbidden, this->request_.version() };
            response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(boost::beast::http::field::content_type, "text/html");
            this->append_cookies(response);
            response.keep_alive(this->request_.keep_alive());
            response.prepare_payload();
            return response;
        })
        , not_found([this](boost::beast::string_view target) {
            boost::beast::http::response<boost::beast::http::string_body> response { boost::beast::http::status::not_found, this->request_.version() };
            response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(boost::beast::http::field::content_type, "text/html");
            this->append_cookies(response);
            response.keep_alive(this->request_.keep_alive());
            response.body() = "The resource '" + std::string(target) + "' was not found.";
            response.prepare_payload();
            return response;
        })
        , not_acceptable([this](void) {
            boost::beast::http::response<boost::beast::http::empty_body> response { boost::beast::http::status::not_acceptable, this->request_.version() };
            response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(boost::beast::http::field::content_type, "text/html");
            this->append_cookies(response);
            response.keep_alive(this->request_.keep_alive());
            response.prepare_payload();
            return response;
        })
        , unsupported_media([this](void) {
            boost::beast::http::response<boost::beast::http::empty_body> response { boost::beast::http::status::unsupported_media_type, this->request_.version() };
            response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(boost::beast::http::field::content_type, "text/html");
            this->append_cookies(response);
            response.keep_alive(this->request_.keep_alive());
            response.prepare_payload();
            return response;
        })
        , server_error([this](boost::beast::string_view what) {
            boost::beast::http::response<boost::beast::http::string_body> response { boost::beast::http::status::internal_server_error, this->request_.version() };
            response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(boost::beast::http::field::content_type, "text/html");
            this->append_cookies(response);
            response.keep_alive(this->request_.keep_alive());
            response.body() = what
                #if BOOST_VERSION <= 107700
                    .to_string()
                #endif // BOOST_VERSION
            ;
            response.prepare_payload();
            return response;
        })
        , not_implemented([this](boost::beast::string_view what) {
            boost::beast::http::response<boost::beast::http::string_body> response { boost::beast::http::status::not_implemented, this->request_.version() };
            response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(boost::beast::http::field::content_type, "text/html");
            this->append_cookies(response);
            response.keep_alive(this->request_.keep_alive());
            response.body() = "Method '" +
                #if BOOST_VERSION <= 107700
                    what.to_string()
                #else // BOOST_VERSION
                    std::string(what)
                #endif // BOOST_VERSION
                + "' not implemented";
            response.prepare_payload();
            return response;
        })
    {}
    virtual ~http_handler(void) {}

public: // Public methods

    /**
     * \brief Вызов REST объекта в контексте устройства.
     * \param[in] object Указатель на вызываемый объект.
     */
    /*void call_wrapper(rest::node_t* object) {

        // Check REST object just to be sure
        if (!object) {
            log::fatal("http::http_handler::call_wrapper", system::err::null_pointer) << "Attempt to call null pointer";
            this->result_wrapper(std::make_tuple(http::status::internal_server_error, rest::make_error(rest::error::internal, "Attempt to call null pointer")));
            print_stack_trace();
            return;
        }

        // Call REST with result callback
        using namespace std::placeholders;
        return (*object)(
            this->query_.params(),
            this->request_.body(),
            BIND(result_wrapper, _1)
        );
    }*/

    /**
     * \brief Возврат результата из контекста устройства.
     * \param[in] response Ответ от REST объекта.
     */
    /*void result_wrapper(const rest::result_t& response) {

        // Check session object
        http_connection::ptr session = this->session_.lock();
        if (!session) {
            log::debug("http::http_handler::result_wrapper") << "Result called, but session is gone";
            return;
        }

        // Send response to REST context
        boost::asio::post(
            boost::asio::bind_executor(
                session->strand(),
                BIND(on_rest_result, session, response)
            )
        );
    }*/

    /**
     * \brief Отправка результата выполнения операции в контексте REST.
     * \param[in] result Результат выполнения операции.
     */
    /*void on_rest_result(http_connection::ptr session, const rest::result_t& result) {

        // Check session object
        //http_connection::ptr session = this->session_.lock();
        if (!session) {
            log::debug("http::http_handler::on_rest_result") << "Result called, but session is gone";
            return;
        }

        // Process REST
        this->timer_.cancel();
        http::status status = std::get<0>(result);
        std::string body = std::get<1>(result);

        // Check status
        switch (status) {
            case http::status::ok:                      break;
            case http::status::accepted:                return this->send_(accepted("", body), this->work_);
            case http::status::no_content:              return this->send_(no_content(), this->work_);
            case http::status::bad_request:             return this->send_(bad_request(body), this->work_);
            case http::status::unauthorized:            return this->send_(unauthorized(""), this->work_);
            case http::status::forbidden:               return this->send_(forbidden(), this->work_);
            case http::status::not_found:               return this->send_(not_found(this->request_.target()), this->work_);
            case http::status::not_acceptable:          return this->send_(not_acceptable(), this->work_);
            case http::status::unsupported_media_type:  return this->send_(unsupported_media(), this->work_);
            case http::status::internal_server_error:   return this->send_(server_error(body), this->work_);
            case http::status::not_implemented:         return this->send_(not_implemented(body), this->work_);
            default: return this->send_(server_error(rest::make_error(rest::error::internal, "Returned unsupported status code")), this->work_);
        }

        // Send response
        http::response<http::string_body> response {
            std::piecewise_construct,
            std::make_tuple(body),
            std::make_tuple(http::status::ok, this->request_.version())
        };
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "application/json");
        this->append_cookies(response);
        response.content_length(body.length());
        response.keep_alive(this->request_.keep_alive());
        return this->send_(std::move(response), this->work_);
    }*/

    void on_timer_result(boost::system::error_code ec) {

        // Timer failed or cancelled
        if (ec) {
            if (ec != boost::asio::error::operation_aborted) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "http::http_handler::on_timer_result", "Timer error: %s", ec.message().c_str());
                this->send_(server_error("Failed to run timer"), this->work_);
                return;
            } else return;
        }

        // Send timeout
        this->send_(server_error("Associated device not respond in time"), this->work_);
    }

    void operator()(void) {

        // Check session object
        http_connection::ptr session = this->session_.lock();
        if (!session) {
            sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::http_handler::operator()", "Result called, but session is gone");
            return;
        }

        // Make sure we can handle the method
        if (this->request_.method() != boost::beast::http::verb::get &&
            this->request_.method() != boost::beast::http::verb::post) {
            return this->send_(bad_request("Unknown HTTP-method"), this->work_);
        }

        // Remove query string
        boost::beast::string_view target = this->request_.target();
        auto const pos = target.find("?");
        if (pos != boost::beast::string_view::npos) {
            target = target.substr(0, pos);
        }

        // Check session ID
        std::string session_id;
        if (this->request_.find(boost::beast::http::field::cookie) == this->request_.end()) {
            session_id = session->pool().create(system::time::now());
            if (this->config_.has_authorization()) {
                return this->send_(unauthorized(session_id), this->work_);
            }
            this->cookies_.emplace_back(initp::http::cookies::make(DEVICE_HTTP_SESSION_KEY, session_id, "/"));
        } else {
            initp::http::cookies cookies(this->request_[boost::beast::http::field::cookie]
                #if BOOST_VERSION <= 107700
                    .to_string()
                #endif // BOOST_VERSION
            );
            auto cookie = cookies->find(DEVICE_HTTP_SESSION_KEY);
            if (cookie == cookies->end()) {
                session_id = session->pool().create(system::time::now());
                if (this->config_.has_authorization()) {
                    return this->send_(unauthorized(session_id), this->work_);
                }
                this->cookies_.emplace_back(initp::http::cookies::make(DEVICE_HTTP_SESSION_KEY, session_id, "/"));
            } else if (!session->pool().check_and_extend(cookie->second)) {
                session_id = session->pool().create(system::time::now());
                if (this->config_.has_authorization()) {
                    return this->send_(unauthorized(session_id), this->work_);
                }
                this->cookies_.emplace_back(initp::http::cookies::make(DEVICE_HTTP_SESSION_KEY, session_id, "/"));
            } else {
                session_id = cookie->second;
            }
        }

        // Get session data
        session_data* session_data = session->pool().find(session_id);
        if (!session_data) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "http::http_handler::operator()", "Session not found after renew");
            return this->send_(server_error("Internal server error"), this->work_);
        }
        if (!session_data->authorized()) {
            if (!this->config_.has_authorization()) {
                session_data->authorized(true);
            } else if (this->config_.authorization().users().empty()) {
                sys_debug_print(SYSTEM_LEVEL_WARNING, "http::http_handler::operator()", "HTTP password not set in config");
                return this->send_(unauthorized(session_data->id()), this->work_);
            } else if (this->request_.find(boost::beast::http::field::authorization) != this->request_.end()) {
                std::string authorization = this->request_[boost::beast::http::field::authorization]
                    #if BOOST_VERSION <= 107700
                        .to_string()
                    #endif // BOOST_VERSION
                ;
                bool result = false;
                for (const auto& user : this->config_.authorization().users()) {
                    result = initp::http::auth::digest::check(
                        user.name(),
                        user.password(),
                        (this->request_.method() == boost::beast::http::verb::get) ? "GET" : "POST",
                        authorization
                    );
                    if (result) {
                        sys_debug_print(SYSTEM_LEVEL_TRACE, "http::http_handler::operator()", "Authorized");
                        session_data->authorized(true);
                        break;
                    }
                }
                if (!result) {
                    sys_debug_print(SYSTEM_LEVEL_WARNING, "http::http_handler::operator()", "Authorization failed with data: %s", authorization.c_str());
                    return this->send_(unauthorized(session_data->id()), this->work_);
                }
            } else {
                return this->send_(unauthorized(session_data->id()), this->work_);
            }
        } else session_data->renew(system::time::now());

        // GET files
        if (this->config_.has_files_root()) {
            if (target.empty() || boost::beast::iequals(target, "/")) {
                // Redirect to index.html
                return this->send_(moved_permanently("/index.html"), this->work_);
            }
            static const boost::beast::string_view mime_application_text("application/text");
            if (this->request_.method() == boost::beast::http::verb::get &&
                this->mime_type(target) != mime_application_text) {
                return this->get_file(target);
            }
        }

        return this->send_(not_found(this->request_.target()), this->work_);
        /*#ifdef PRIME_TRACE_HTTP_REQUESTS
        {
            LOCK_RMUTEX(initp::prime::debug_output_mutex_);
            if (this->request_.method() == http::verb::get) {
                std::cout << "GET: ";
            } else if (this->request_.method() == http::verb::post) {
                std::cout << "POST: ";
            }
            std::cout << this->request_.target() << std::endl;
            for (const auto& field : this->request_)
                std::cout << field.name() << ": " << field.value() << std::endl;
            if (this->request_.method() == http::verb::post)
                std::cout << "Body: " << this->request_.body() << std::endl;
        }
        #endif // PRIME_TRACE_HTTP_REQUESTS

        #ifndef PRIME_DISABLE_HTTP_RULES
        if (this->request_.method() == http::verb::get) {
            if (this->request_.find(http::field::accept) == this->request_.end() ||
                this->request_[http::field::accept] != JSON_API_MEDIA_TYPE) {
                return this->send_(not_acceptable(), this->work_);
            }
        } else if (this->request_.method() == http::verb::post) {
            if (this->request_.find(http::field::content_type) == this->request_.end() ||
                this->request_[http::field::content_type] != JSON_API_MEDIA_TYPE) {
                return this->send_(unsupported_media(), this->work_);
            }
        }
        #endif // PRIME_DISABLE_HTTP_RULES

        // Check manager
        manager::ptr manager = manager_weak.lock();
        if (!manager) {
            log::fatal("http::http_handler::operator()", system::err::null_pointer) << "Manager is null";
            return;
        }

        // Check flags
        if (!manager->started()) {
            return this->send_(server_error(rest::make_error(rest::error::internal, "Device manager not started")), this->work_);
        }
        if (!manager->initialized()) {
            return this->send_(server_error(rest::make_error(rest::error::not_initialized, "Device manager not initialized yet")), this->work_);
        }
        if (!manager->internal_errors().empty()) {
            return this->send_(server_error(rest::make_error_stack(manager->internal_errors())), this->work_);
        }

        // Find associated REST object
        if (rest::node_t* object = manager->find_rest_node(this->request_.method(), this->query_.path())) {

            using namespace std::placeholders;
            http_connection::ptr session = this->session_.lock();

            const rest::meta_t& meta = object->rest_meta();
            if (rest::event::emitter* emitter = std::get<1>(meta)) {

                // Wait for events
                rest::event::collector::ptr collector = session_data.rest_events().get_or_create_collector(std::static_pointer_cast<context>(session), emitter, this->query_.path());
                if (collector) {
                    collector->wait(*this, emitter, BIND(on_rest_result, session, _1));
                } else {
                    log::fatal("http::http_handler::operator()", system::err::internal_error) << "Failed to get events collector";
                    print_stack_trace();
                }

            } else {

                // Place wrapped call to device context
                std::string body = this->request_.body();
                boost::asio::post(
                    boost::asio::bind_executor(
                        std::get<0>(meta)->strand(),
                        BIND(call_wrapper, object)
                    )
                );

                // Run operation timer in REST context
                this->timer_.expires_after(std::chrono::milliseconds(REST_REQUEST_TIMEOUT));
                this->timer_.async_wait(
                    boost::asio::bind_executor(
                        session->strand(),
                        BIND(on_timer_result, _1)
                    )
                );
            }
        } else return this->send_(not_found(this->request_.target()), this->work_);*/

    }

private: // Private methods

    boost::beast::string_view mime_type(boost::beast::string_view path) {
        using boost::beast::iequals;
        auto const ext = [&path] {
            auto const pos = path.rfind(".");
            if (pos == boost::beast::string_view::npos)
                return boost::beast::string_view {};
            return path.substr(pos);
        } ();
        if (iequals(ext, ".htm"))  return "text/html";
        if (iequals(ext, ".html")) return "text/html";
        if (iequals(ext, ".php"))  return "text/html";
        if (iequals(ext, ".css"))  return "text/css";
        if (iequals(ext, ".txt"))  return "text/plain";
        if (iequals(ext, ".js"))   return "application/javascript";
        if (iequals(ext, ".json")) return "application/json";
        if (iequals(ext, ".xml"))  return "application/xml";
        if (iequals(ext, ".swf"))  return "application/x-shockwave-flash";
        if (iequals(ext, ".flv"))  return "video/x-flv";
        if (iequals(ext, ".png"))  return "image/png";
        if (iequals(ext, ".jpe"))  return "image/jpeg";
        if (iequals(ext, ".jpeg")) return "image/jpeg";
        if (iequals(ext, ".jpg"))  return "image/jpeg";
        if (iequals(ext, ".gif"))  return "image/gif";
        if (iequals(ext, ".bmp"))  return "image/bmp";
        if (iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
        if (iequals(ext, ".tiff")) return "image/tiff";
        if (iequals(ext, ".tif"))  return "image/tiff";
        if (iequals(ext, ".svg"))  return "image/svg+xml";
        if (iequals(ext, ".svgz")) return "image/svg+xml";
        if (iequals(ext, ".ttf"))  return "font/ttf";
        if (iequals(ext, ".otf"))  return "font/otf";
        return "application/text";
    }

    void get_file(boost::beast::string_view target) {

        std::string path = this->path_cat(this->config_.files_root(), target);
        #ifdef SYSTEM_TRACE_HTTP_REQUESTS
        sys_debug_print(SYSTEM_LEVEL_TRACE, "http::http_handler::get_file", "GET file: %s", path.c_str());
        #endif // SYSTEM_TRACE_HTTP_REQUESTS

        // Attempt to open the file
        boost::beast::error_code err;
        boost::beast::http::file_body::value_type body;
        body.open(path.c_str(), boost::beast::file_mode::scan, err);

        // Handle the case where the file doesn't exist
        if (err == boost::system::errc::no_such_file_or_directory)
            return this->send_(not_found(this->request_.target()), this->work_);

        // Handle an unknown error
        if (err) return this->send_(server_error(err.message()), this->work_);

        // Cache the size since we need it after the move
        const auto size = body.size();

        // Respond to GET request
        boost::beast::http::response<boost::beast::http::file_body> response {
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(boost::beast::http::status::ok, this->request_.version())
        };
        response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(boost::beast::http::field::content_type, this->mime_type(path));
        this->append_cookies(response);
        response.content_length(size);
        response.keep_alive(this->request_.keep_alive());
        return this->send_(std::move(response), this->work_);
    }

    // Append an HTTP rel-path to a local filesystem path.
    // The returned path is normalized for the platform.
    std::string path_cat(boost::beast::string_view base, boost::beast::string_view path) {
        std::string path_str = path
            #if BOOST_VERSION <= 107700
                .to_string()
            #endif // BOOST_VERSION
        ;
        if (base.empty()) return path_str;
        std::string result = base
            #if BOOST_VERSION <= 107700
                .to_string()
            #endif // BOOST_VERSION
        ;
    #if BOOST_MSVC
        char constexpr path_separator = '\\';
        if (result.back() == path_separator)
            result.resize(result.size() - 1);
        result.append(path.data(), path.size());
        for (auto& c : result)
            if (c == '/') c = path_separator;
    #else // BOOST_MSVC
        char constexpr path_separator = '/';
        if (result.back() == path_separator)
            result.resize(result.size() - 1);
        result.append(path.data(), path.size());
    #endif // BOOST_MSVC
        return result;
    }

    template<typename T>
    void append_cookies(boost::beast::http::response<T>& response) {
        if (this->cookies_.empty()) return;
        for (const std::string& cookie : this->cookies_) {
            response.set(boost::beast::http::field::set_cookie, cookie);
        }
    }

public: // Public properties
    virtual boost::asio::steady_timer& timer(void) {
        return this->timer_;
    }
    virtual initp::http::query& query(void) {
        return this->query_;
    }

private: // Private fields
    const server_config& config_;
    http_connection::wptr session_;
    boost::beast::http::request<boost::beast::http::string_body> request_;
    boost::asio::steady_timer timer_;
    initp::http::query query_;
    std::vector<std::string> cookies_;
    Send& send_;
    void* work_;

private: // Responses
    /**
     * \brief HTTP status 202 Accepted
     */
    const async_response_t accepted;
    /**
     * \brief HTTP status 204 No Content
     */
    const empty_response_t no_content;
    /**
     * \brief HTTP status 301 Moved Permanently
     */
    const empty_response_paramed_t moved_permanently;
    /**
     * \brief HTTP status 400 Bad Request
     */
    const string_response_t bad_request;
    /**
     * \brief HTTP status 401 Unauthorized
     */
    const empty_response_paramed_t unauthorized;
    /**
     * \brief HTTP status 403 Forbidden
     */
    const empty_response_t forbidden;
    /**
     * \brief HTTP status 404 Not Found
     */
    const string_response_t not_found;
    /**
     * \brief HTTP status 406 Not Acceptable
     */
    const empty_response_t not_acceptable;
    /**
     * \brief HTTP status 415 Unsupported Media Type
     */
    const empty_response_t unsupported_media;
    /**
     * \brief HTTP status 500 Internal Server Error
     */
    const string_response_t server_error;
    /**
     * \brief HTTP status 501 Not Implemented
     */
    const string_response_t not_implemented;
};

}}

#endif // INITPLUS_HTTP_SERVER_HADNLER_HPP_INCLUDED
