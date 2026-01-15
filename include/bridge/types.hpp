//
// Created by vadim.tissen on 18.11.2025.
//

#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <chrono>

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
        Bytes payload; //deserialized data
        std::string resource_id; // http url id
        std::chrono::system_clock::time_point timestamp;
    };

    struct HttpResponseData {
        int status_code;
        std::string body;
        std::string reason;
    };

    struct Order {
        float amount = 0.0f;
        float volume = 0.0f;
        float price = 0.0f;
    };
}
