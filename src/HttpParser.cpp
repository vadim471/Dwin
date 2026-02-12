//
// Created by vadim.tissen on 10.12.2025.
//

#include "bridge/HttpParser.hpp"
#include "bridge/json.hpp"
#include "bridge/types.hpp"
#include <algorithm>
#include <random>
#include <vector>
#include <openssl/rand.h>
#include <regex>
#include "bridge/constant.hpp"
#include "bridge/utility.hpp"


using json = nlohmann::json;

namespace bridge {

    // Найти подстроку в векторе. Возвращает индекс начала.
    long findSubVector(const std::vector<uint8_t>& haystack, const std::string& needle) {
        auto it = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end());
        if (it == haystack.end()) return -1;
        return std::distance(haystack.begin(), it);
    }

    HttpParser::HttpParser(const Settings &settings) : m_settings(settings) {
        //GET
        addRoute(GET_EVENTS, "GET", "/api/v3/events?last_id=ID"); //GET /events?last_id=N&max=M  on_dsp_status_changed
        addRoute(GET_DEVICES, "GET", "/api/v3/devices"); // get all devices and get
        addRoute(GET_IO_PARAMETERS, "GET", "/api/v3/devices/ID/parameters"); //replace ID
        addRoute(GET_DISPENSER_STATUS, "GET", "/api/v3/devices/ID/status");
        addRoute(GET_PRODUCTS, "GET", "/api/v3/products");
        addRoute(GET_TASKS, "GET", "/api/v3/tasks/ID");

        //POST
        addRoute(CREATE_ORDER, "POST", "/api/v3/devices/ID/order");
        addRoute(SET_COMMAND, "POST", "/api/v3/devices/ID/command");

        //NOT YET IMPLEMENTED
        addRoute("set_io_parameters", "POST", "");
    }

    // type === Message.type
    void HttpParser::addRoute(const std::string &type, const std::string &method, const std::string &url) {
        m_routes[type] = { method, url};
    }

    // Собирает Message из входящего потока байтов HTTP. Ответ на запросы.
    std::vector<Message> HttpParser::parse(const RawData& input, const std::string& source) {
        std::vector<Message> messages;
        if (input.data.empty()) return messages;

        try {
            auto it = std::find(input.data.begin(), input.data.end(), '\0');
            std::string url;
            Bytes json_body;

            if (it != input.data.end()) {
                url.assign(input.data.begin(), it);

                json_body.assign(it + 1, input.data.end());
            } else {
                // Все в JSON body.
                json_body.assign(input.data.begin(), input.data.end());
            }

            Message msg;
            msg.source = source;
            msg.url = url;
            msg.type = HTTP_RESPONSE;
            msg.payload = json_body;
            messages.push_back(msg);

        } catch (std::exception& e) {
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
