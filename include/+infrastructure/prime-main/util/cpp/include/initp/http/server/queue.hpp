#ifndef INITPLUS_HTTP_SERVER_QUEUE_HPP_INCLUDED
#define INITPLUS_HTTP_SERVER_QUEUE_HPP_INCLUDED

#include <initp/http/server/connection.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>

#include <memory>
#include <vector>

namespace initp {
namespace http {

/**
 * \brief This queue is used for HTTP pipelining.
 */
class http_queue {

    enum {
        /**
         * \brief Maximum number of requests we will queue
         */
        limit = 8
    };

    class request_work {
        /**
         * \brief The type-erased, saved response work item.
         */
        struct response_work {
            typedef response_work self_type;
            typedef std::unique_ptr<self_type> uptr;
            virtual ~response_work(void) = default;
            virtual void operator()(void) = 0;
        };
        typedef request_work self_type;
    public:
        typedef std::unique_ptr<self_type> uptr;
        typedef std::vector<uptr> vector;
    public:
        request_work(http_connection& self):
            parent_(self) {}
        void operator()(void) {
            if (!this->response_) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "http::http_queue::request_work::operator()", "Called with no response to write");
            } else (*this->response_)();
        }
        template<bool isRequest, class Body, class Fields>
        void operator()(boost::beast::http::message<isRequest, Body, Fields>&& response) {

            // This holds a work item
            class work_impl: public response_work {

            public: // Construction

                work_impl(http_connection& self,
                    boost::beast::http::message<isRequest, Body, Fields>&& response):
                    parent_(self),
                    response_(std::move(response)) {}

            public: // Public methods

                void operator()(void) {
                    if (parent_.socket_.is_open()) {
                        if (parent_.ssl_) {
                            boost::beast::http::async_write(
                                *parent_.ssl_,
                                this->response_,
                                boost::asio::bind_executor(
                                    parent_.strand_,
                                    std::bind(
                                        &http_connection::on_write,
                                        parent_.shared_from_this(),
                                        std::placeholders::_1,
                                        this->response_.need_eof()
                                    )
                                )
                            );
                        } else {
                            boost::beast::http::async_write(
                                parent_.socket_,
                                this->response_,
                                boost::asio::bind_executor(
                                    parent_.strand_,
                                    std::bind(
                                        &http_connection::on_write,
                                        parent_.shared_from_this(),
                                        std::placeholders::_1,
                                        this->response_.need_eof()
                                    )
                                )
                            );
                        }
                    }
                }

                http_connection& parent_;
                boost::beast::http::message<isRequest, Body, Fields> response_;
            };

            // Check response
            if (this->response_) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "http::http_queue::request_work::operator(response)", "Response already exists");
                return;
            }

            // Allocate and store the work
            this->response_ = boost::make_unique<work_impl>(parent_, std::move(response));
        }
    private:
        http_connection& parent_;
        response_work::uptr response_;
    };

public: // Construction

    explicit http_queue(http_connection& self):
        parent_(self) {
        static_assert(limit > 0, "queue limit must be positive");
        this->items_.reserve(limit);
    }

public: // Public methods

    /**
     * \brief Returns `true` if we have reached the queue limit.
     */
    bool is_full(void) const {
        return this->items_.size() >= limit;
    }

    /**
     * \brief Called when a message finishes sending.
     *
     * Returns 'true' if the caller should initiate a read.
     */
    bool on_write(void) {
        BOOST_ASSERT(!this->items_.empty());
        auto const was_full = this->is_full();
        this->items_.erase(this->items_.begin());
        if (!this->items_.empty())
            (*this->items_.front())();
        return was_full;
    }

    /**
     * \brief Called when request received.
     */
    void operator()(boost::beast::http::request<boost::beast::http::string_body>&& request);

    /**
     * \brief Called by the HTTP handler to send a response.
     */
    template<bool isRequest, class Body, class Fields>
    void operator()(boost::beast::http::message<isRequest, Body, Fields>&& response, void* work) {

        request_work* request = static_cast<request_work*>(work);
        for (auto it = this->items_.begin(); it != this->items_.end(); ++it) {
            if (it->get() == request) {
                (**it)(std::move(response));
                if (it == this->items_.begin())
                    (**it)();
                return;
            }
        }

        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::http_queue::request_work::operator(response, work)", "Response received, but request already gone");
    }

private: // Private fields
    http_connection& parent_;
    request_work::vector items_;
};

}}

#endif // INITPLUS_HTTP_SERVER_QUEUE_HPP_INCLUDED
