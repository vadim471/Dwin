//
// Created by vadim.tissen on 15.01.2026.
//

#pragma once
#include "bridge/core/MessageLayer.hpp"
#include "bridge/core/constant.hpp"
#include <vector>


#include "bridge/json.hpp"

namespace bridge {
    class PrimeCommands {
        public:
        static void handleCommand(MessageLayer& core, const std::string& command, const std::string trk_id) {
            nlohmann::json json_object;
            json_object["command"] = command;
            std::string json_string = json_object.dump();

            Message msg;
            msg.source = UART_LAYER;
            msg.type = SET_COMMAND;
            msg.resource_id = trk_id;
            msg.payload.assign(json_string.begin(), json_string.end());

            core.sendTo(HTTP_LAYER, msg);
        }
    };
}
