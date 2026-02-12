//
// Created by vadim.tissen on 18.11.2025.
//

#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <chrono>
#include "constant.hpp"

//types.hpp
namespace bridge {
    using Bytes = std::vector<uint8_t>;

    struct RawData {
        Bytes data;
    };

    struct Message {
        uint64_t id = 0;
        std::string type; //button_click, update
        std::string source; //ws, http, uart
        std::string url; //http url response
        Bytes payload; //deserialized data
        std::string resource_id; // id replace in URL
        std::chrono::system_clock::time_point timestamp;
    };

    struct HttpResponseData {
        std::string url;
        int status_code;
        std::string body;
        std::string reason;
    };

    struct Order {
        std::string amount;
        std::string volume;
        std::string price;
        std::string order_id;
        std::string product_id;
    };

    struct HttpRequestTask {
        std::string method;
        std::string uri;
        std::string body;
    };

    struct Product {
        std::string id;
        std::string rating;
        std::string title;
        std::string price;

        Product(const std::string& id) : id(id) {}
    };

    struct Dispenser {
        std::string id;
        std::string status;
        Order order;
        std::string product_id;

        Dispenser(const std::string &_id) : id(_id), status(DISPENSER_IDLE) {}
    };
}
