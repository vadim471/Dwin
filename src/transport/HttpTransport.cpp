//
// Created by vadim.tissen on 10.12.2025.
//
#include "bridge/transport/HttpTransport.hpp"
#include <iostream>
#include <sstream>
#include <thread>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPDigestCredentials.h>

#include "bridge/core/constant.hpp"

namespace bridge {

    HttpTransport::HttpTransport(const std::string& host, uint16_t port, const std::string& user, const std::string& pass)
        : m_host(host), m_port(port), m_user(user), m_pass(pass) {
        m_session.setHost(host);
        m_session.setPort(port);
        m_session.setKeepAlive(true);
        m_credentials.setUsername(m_user);
        m_credentials.setPassword(m_pass);
    }

    HttpTransport::~HttpTransport() { stop(); }

    void HttpTransport::start() {
        m_running = true;
        m_worker = std::thread(&HttpTransport::workerThread, this);
        std::cout << "[HTTP Transport] Worker Started" << std::endl;
    }

    void HttpTransport::stop() {
        m_running = false;
        m_cv.notify_all();
        if (m_worker.joinable()) {
            m_worker.join();
        }
    }

    void HttpTransport::setReceiveHandler(ReceiveHandler handler) {
        m_receive_handler = std::move(handler);
    }

    // По "|" разделены METHOD | URI | BODY
    void HttpTransport::send(const RawData& data) {
        if (!m_running) {
            return;
        }

        std::string command(data.data.begin(), data.data.end());

        std::stringstream ss(command);
        std::string segment;
        std::vector<std::string> parts;

        while(std::getline(ss, segment, '|')) {
            parts.push_back(segment);
        }

        if (parts.size() < 2) return;
        std::string method = parts[0];
        std::string uri = parts[1];
        std::string body = (parts.size() > 2) ? parts[2] : "";

        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            m_queue.push({method, uri, body});
        }
        m_cv.notify_one();
    }

    void HttpTransport::workerThread() {
        while (m_running) {
            HttpRequestTask task;
            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);
                m_cv.wait(lock, [this] { return !m_queue.empty() || !m_running; });

                if (!m_running && m_queue.empty()) break;

                task = m_queue.front();
                m_queue.pop();
            }

            performRequest(task.method, task.uri, task.body);
        }
    }

    void HttpTransport::performRequest(const std::string& method, const std::string& uri, const std::string& body) {
        std::lock_guard<std::mutex> lock(m_session_mutex);
        try {
            using namespace Poco::Net;

            HTTPRequest request(method, uri, HTTPMessage::HTTP_1_1);
            request.setKeepAlive(true);
            if (!body.empty()) {
                request.setContentLength(body.length());
                request.setContentType("application/json");
            }

            HttpResponseData response_data = authenticate(request, body);

            if (response_data.status_code >= 400) {
                std::cerr << "[HTTP ERROR] " << response_data.status_code << " (" << response_data.reason << ")\n"
                          << "Body: " << response_data.body << std::endl;

                if (response_data.status_code == 401 || response_data.status_code == 403) {
                    m_session_cookie.clear();
                }
            }

            if (m_receive_handler) {
                RawData raw_data;

                std::string string_status_code = std::to_string(response_data.status_code);
                raw_data.data.insert(raw_data.data.end(), string_status_code.begin(), string_status_code.end());
                raw_data.data.push_back('\0');

                raw_data.data.insert(raw_data.data.end(), response_data.url.begin(), response_data.url.end());
                raw_data.data.push_back('\0');
                raw_data.data.insert(raw_data.data.end(), response_data.body.begin(), response_data.body.end());

                m_receive_handler(raw_data, HTTP_LAYER);
            }

        } catch (Poco::Exception& ex) {
            std::cerr << "HTTP [POCO] Error: " << ex.displayText() << std::endl;
            m_session.reset();
            if (m_receive_handler) {
                RawData raw_data;

                std::string string_status_code = std::to_string(POCO_ERROR);
                raw_data.data.insert(raw_data.data.end(), string_status_code.begin(), string_status_code.end());
                raw_data.data.push_back('\0');

                raw_data.data.insert(raw_data.data.end(), uri.begin(), uri.end());
                raw_data.data.push_back('\0');

                std::string err_body = "{\"error\": \"" + ex.displayText() + "\"}";
                raw_data.data.insert(raw_data.data.end(), err_body.begin(), err_body.end());

                m_receive_handler(raw_data, HTTP_LAYER);
            }
        }
    }

    void HttpTransport::setCookie(const Poco::Net::HTTPResponse& response) {
        std::vector<Poco::Net::HTTPCookie> cookies;
        response.getCookies(cookies);

        for (const auto& cookie : cookies) {
            if (cookie.getName() == "session_id") {
                m_session_cookie = cookie.getName() + "=" + cookie.getValue();
            }
        }
    }

    HttpResponseData HttpTransport::authenticate(Poco::Net::HTTPRequest &request, const std::string &body) {
        using namespace Poco::Net;

        if (!m_session_cookie.empty()) {
            request.set("Cookie", m_session_cookie);
        }

        try {
            m_credentials.updateAuthInfo(request);
        } catch (std::exception& ex) {

        }

        HTTPResponse response;
        std::stringstream responseStream;

        try {
            std::ostream& os = m_session.sendRequest(request);
            if (!body.empty()) os << body;

            std::istream& is = m_session.receiveResponse(response);
            Poco::StreamCopier::copyStream(is, responseStream);
        } catch (std::exception& ex) {
            m_session.reset();
            std::ostream& os = m_session.sendRequest(request);
            if (!body.empty()) os << body;

            std::istream& is = m_session.receiveResponse(response);
            Poco::StreamCopier::copyStream(is, responseStream);
        }

        setCookie(response);

        if (response.getStatus() == HTTPResponse::HTTP_UNAUTHORIZED) {

            m_credentials.authenticate(request, response);

            if (!m_session_cookie.empty()) {
                request.set("Cookie", m_session_cookie);
            }

            std::ostream& os = m_session.sendRequest(request);
            if (!body.empty()) os << body;

            std::istream& is = m_session.receiveResponse(response);

            responseStream.str(""); responseStream.clear();
            Poco::StreamCopier::copyStream(is, responseStream);
        }
        return { request.getURI(), response.getStatus(), responseStream.str(), response.getReason() };
    }
}
