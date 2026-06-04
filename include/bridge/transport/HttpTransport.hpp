#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include "bridge/transport/ITransport.hpp"
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPDigestCredentials.h>

#include "bridge/core/Settings.hpp"
#include "bridge/core/types.hpp"
#include "bridge/httpAuth/IHttpAuthenticator.hpp"

namespace bridge {
    class HttpTransport : public ITransport {
    public:
        HttpTransport(const std::string& channel_name, const std::string& host,
                      uint16_t port, bool user_ssl, std::shared_ptr<IHttpAuthenticator> authenticator);
        ~HttpTransport();


        using ReceiveHandler = std::function<void(const RawData&)>;

        void start() override;
        void stop() override;
        void send(const RawData& data) override;
        void setReceiveHandler(ReceiveHandler handler) override;

    private:
        void performRequest(const std::string& method, const std::string& uri, const std::string& body);
        HttpResponseData authenticate(Poco::Net::HTTPRequest& request, const std::string& body);
        void workerThread();

        std::string m_host;
        uint16_t m_port;

        std::string m_channel_name;
        std::shared_ptr<IHttpAuthenticator> m_authenticator;

        std::atomic<bool> m_running{false};
        ReceiveHandler m_receive_handler;

        std::thread m_worker;
        std::condition_variable m_cv;
        std::queue<HttpRequestTask> m_queue;
        std::mutex m_queue_mutex;

        // Mutex для каждого запроса.
        std::mutex m_session_mutex;

        // Сессия, чтобы не плодить на прайме.
        std::unique_ptr<Poco::Net::HTTPClientSession> m_session;
    };
}