//
// Created by vadim.tissen on 10.12.2025.
//

#pragma once
#include <Poco/Net/HTTPClientSession.h>

#include "IMessageParser.hpp"
#include "bridge/core/Settings.hpp"


namespace bridge {
    struct ApiRoute {
        std::string method; //GET, POST, PUT
        std::string url; //"api/v3/device"
    };

    class HttpParser : public IMessageParser {
    public:
        explicit HttpParser(const Settings& settings);

        std::vector<Message> parse(const RawData& input, const std::string& message_source) override;

        RawData serialize(const Message& message) override;
    private:
        void addRoute(const std::string& messageType, const std::string& method, const std::string& url);

        std::map<std::string, ApiRoute> m_routes;
        std::vector<uint8_t> m_buffer;
        Settings m_settings;

    };
}
