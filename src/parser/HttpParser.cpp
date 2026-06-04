//
// Created by vadim.tissen on 12.05.2026.
//

#include "bridge/parser/HttpParser.hpp"
#include "bridge/core/types.hpp"
#include "bridge/core/utility.hpp"

namespace bridge {
    void HttpParser::addRoute(const std::string &type, const std::string &method, const std::string &url) {
        m_routes[type] = { method, url};
    }   

    std::vector<Message> HttpParser::parse(const RawData& input, const std::string& source) {
        std::vector<Message> messages;
        if (input.data.empty()) return messages;

        try {
            Message msg;
            msg.source = source;
            msg.type = HTTP_RESPONSE;

            // Разделитель между http.code/http.url/http.body
            auto it1 = std::find(input.data.begin(), input.data.end(), '\0');

            if (it1 != input.data.end()) {
                std::string status_str(input.data.begin(), it1);
                msg.status_code = std::stoi(status_str);

                auto it2 = std::find(it1 + 1, input.data.end(), '\0');

                if (it2 != input.data.end()) {
                    msg.url.assign(it1 + 1, it2);
                    msg.payload.assign(it2 + 1, input.data.end());
                } else {

                    msg.url.assign(it1 + 1, input.data.end());
                }
            }

            messages.push_back(msg);

        } catch (const std::exception& e) {
            std::cerr << "[Parser] Error: " << e.what() << std::endl;
        }
        return messages;
    }

    // Передает в MessageLayer
    RawData HttpParser::serialize(const Message& message) {
        RawData raw_data;

        auto it = m_routes.find(message.type);
        if (it == m_routes.end()) {
            std::cerr << "[HttpParser] Route NOT FOUND for type: " << message.type << std::endl;
            return raw_data;
        }

        const ApiRoute& route = it->second;
        std::string url = route.url;
        std::string body = "";

        if (!message.resource_id.empty()) {
            url = utility::replaceIdInUrl(url, "ID", message.resource_id);
        }

        // Добавление тела в POST методы.
        if (!message.payload.empty() && route.method != "GET") {
            body.assign(message.payload.begin(), message.payload.end());
        }

        // Собираем строку: METHOD|URL|BODY
        std::string command = it->second.method + "|" + url + "|" + body;

        raw_data.data.assign(command.begin(), command.end());
        return raw_data;
    }
}