//
// Created by vadim.tissen on 12.05.2026.
//


#include "bridge/httpAuth/DigestAuthenticator.hpp"

#include <Poco/StreamCopier.h>

namespace bridge {
    DigestAuthenticator::DigestAuthenticator(const Settings &settings) : m_settings(settings) {
        m_credentials.setUsername(m_settings.server_prime.username);
        m_credentials.setPassword(m_settings.server_prime.password);
    };

    DigestAuthenticator::~DigestAuthenticator(){};

    void DigestAuthenticator::prepareRequest(Poco::Net::HTTPRequest &request) {
        if (!m_session_cookie.empty()) {
            request.set("Cookie", m_session_cookie);
        }
        try {
            m_credentials.updateAuthInfo(request);
        } catch (std::exception& ex) {}
    }

    void DigestAuthenticator::setCookie(const Poco::Net::HTTPResponse& response) {
        std::vector<Poco::Net::HTTPCookie> cookies;
        response.getCookies(cookies);

        for (const auto& cookie : cookies) {
            if (cookie.getName() == "session_id") {
                m_session_cookie = cookie.getName() + "=" + cookie.getValue();
            }
        }
    }

    void DigestAuthenticator::processResponse(const Poco::Net::HTTPResponse &response) {

    }

    bool DigestAuthenticator::handleUnauthorized(Poco::Net::HTTPClientSession &session, Poco::Net::HTTPRequest &request, Poco::Net::HTTPResponse &response, const std::string &body, std::ostream &responseStream) {
        m_credentials.authenticate(request, response);

        if (!m_session_cookie.empty()) {
            request.set("Cookie", m_session_cookie);
        }

        std::ostream& os = session.sendRequest(request);
        if (!body.empty()) os << body;

        std::istream& is = session.receiveResponse(response);
        Poco::StreamCopier::copyStream(is, responseStream);

        setCookie(response);

        return response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED;
    }

    void DigestAuthenticator::reset() {
        m_session_cookie.clear();
    }
}
