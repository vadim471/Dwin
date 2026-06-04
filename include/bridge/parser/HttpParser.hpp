//
// Created by vadim.tissen on 12.05.2026.
//

#pragma once
#include <unordered_map>

#include "IMessageParser.hpp"

namespace bridge {
    class HttpParser : public IMessageParser {
    public:
        virtual ~HttpParser() = default;

        std::vector<Message> parse(const RawData& input, const std::string& source) override;
        RawData serialize(const Message& message) override;

    protected:
        void addRoute(const std::string &type, const std::string &method, const std::string &url);
    private:
        std::unordered_map<std::string, ApiRoute> m_routes;
    };
}
