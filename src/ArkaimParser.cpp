#include "bridge/ArkaimParser.hpp"
#include <iostream>

namespace bridge {

ArkaimParser::ArkaimParser()
    : m_terminal(nullptr)
{
}

std::vector<Message> ArkaimParser::parse(const RawData& data, const std::string& source) {
    // ITP protocol uses callbacks, not polling
    // Responses are handled in callbacks registered with push_request()
    // So this method returns empty vector
    return {};
}

RawData ArkaimParser::serialize(const Message& message) {
    // ITP protocol sending happens through terminal->node().push_request()
    // in ArkaimLogic, not through this serialize method
    // So this method returns empty RawData
    return RawData{};
}

} // namespace bridge
