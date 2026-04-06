#include "bridge/ArkaimParser.hpp"
#include "bridge/constant.hpp"

#include <iostream>

namespace bridge {

std::vector<Message> ArkaimParser::parse(const RawData& raw_data, const std::string& message_source) {
    // ITP protocol framing is handled by itp::entity inside ArkaimLogic.
    // This parser only handles Messages routed through sendToLogicLayer().
    // Raw pipe data never reaches here — it goes through the entity's internal transport.
    std::vector<Message> messages;

    if (!raw_data.data.empty()) {
        Message msg;
        msg.source = message_source;
        msg.payload = raw_data.data;
        messages.push_back(msg);
    }

    return messages;
}

RawData ArkaimParser::serialize(const Message& message) {
    // ArkaimLogic sends commands via itp::root::push_request(), not via transport.send().
    // This serialize is only called if someone does core.sendTo(PIPE_LAYER, msg),
    // which is not the normal path. Pass payload through for completeness.
    RawData raw;
    raw.data = message.payload;
    return raw;
}

} // namespace bridge
