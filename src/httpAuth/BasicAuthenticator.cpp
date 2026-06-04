//
// Created by vadim.tissen on 12.05.2026.
//

#include "bridge/httpAuth/BasicAuthenticator.hpp"

#include "bridge/core/Settings.hpp"

namespace bridge {
    BasicAuthenticator::BasicAuthenticator(const Settings &settings) : m_settings(settings) {
    }
    BasicAuthenticator::~BasicAuthenticator() {};

    void BasicAuthenticator::prepareRequest(Poco::Net::HTTPRequest& request) {
        Poco::Net::HTTPBasicCredentials creds(m_settings.server_bos.username, m_settings.server_bos.password);
        creds.authenticate(request);
    }
}