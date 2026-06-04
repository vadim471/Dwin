//
// Created by vadim.tissen on 08.05.2026.
//

#pragma once
#include <Poco/Net/HTTPDigestCredentials.h>
#include "IHttpAuthenticator.hpp"
#include "bridge/core/Settings.hpp"

namespace bridge {
    class DigestAuthenticator : public IHttpAuthenticator {
    public:
        DigestAuthenticator(const Settings &settings);
        ~DigestAuthenticator();

        void prepareRequest(Poco::Net::HTTPRequest& request) override;
        void processResponse(const Poco::Net::HTTPResponse& response) override;
        bool handleUnauthorized(Poco::Net::HTTPClientSession& session,
                                Poco::Net::HTTPRequest& request,
                                Poco::Net::HTTPResponse& response,
                                const std::string& body,
                                std::ostream& responseStream) override;
        void setCookie(const Poco::Net::HTTPResponse& response);
        void reset();

    private:
        Poco::Net::HTTPDigestCredentials m_credentials;
        std::string m_session_cookie;
        Settings m_settings;
    };
}
