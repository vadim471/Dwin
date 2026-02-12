#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include "bridge/ITransport.hpp"
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPDigestCredentials.h>


namespace bridge {
    class HttpTransport : public ITransport {
    public:
        HttpTransport(const std::string& host, uint16_t port, const std::string& user, const std::string& pass);
        ~HttpTransport();


        using ReceiveHandler = std::function<void(const RawData&, const std::string&)>;

        void start() override;
        void stop() override;
        void send(const RawData& data) override;
        void setReceiveHandler(ReceiveHandler handler) override;

    private:
        void performRequest(const std::string& method, const std::string& uri, const std::string& body);
        HttpResponseData authenticate(Poco::Net::HTTPRequest& request, const std::string& body);
        void setCookie(const Poco::Net::HTTPResponse& response);
        void workerThread();

        std::string m_host;
        uint16_t m_port;
        std::string m_user;
        std::string m_pass;

        std::atomic<bool> m_running{false};
        ReceiveHandler m_receive_handler;

        std::thread m_worker;
        std::condition_variable m_cv;
        std::queue<HttpRequestTask> m_queue;
        std::mutex m_queue_mutex;

        // Mutex для каждого запроса
        std::mutex m_session_mutex;
        std::string m_session_cookie;

        // Сессия, чтобы не плодить на прайме
        Poco::Net::HTTPClientSession m_session;
        Poco::Net::HTTPDigestCredentials m_credentials;
    };
}