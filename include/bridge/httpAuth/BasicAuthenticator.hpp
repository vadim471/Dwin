//
// Created by vadim.tissen on 12.05.2026.
//

#pragma once

#include "IHttpAuthenticator.hpp"
#include "bridge/core/Settings.hpp"

namespace bridge {
    class BasicAuthenticator : public IHttpAuthenticator {
    public:
        BasicAuthenticator(const Settings &settings);
        ~BasicAuthenticator();

        void prepareRequest(Poco::Net::HTTPRequest& request) override ;

        void processResponse(const Poco::Net::HTTPResponse& response) override {};

        bool handleUnauthorized(Poco::Net::HTTPClientSession& session,
                                Poco::Net::HTTPRequest& request,
                                Poco::Net::HTTPResponse& response,
                                const std::string& body,
                                std::ostream& responseStream) override {
            return false;
        }

        void reset() override {};
    private:
        Settings m_settings;
    };
}
