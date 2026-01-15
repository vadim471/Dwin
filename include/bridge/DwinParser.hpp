//
// Created by vadim.tissen on 05.12.2025.
//

#pragma once
#include "IMessageParser.hpp"
#include "MessageLayer.hpp"

namespace bridge {
    class DwinParser : public IMessageParser {
    public:
        std::vector<Message> parse(const RawData& input, const std::string& message_source) override;

        RawData serialize(const Message& message) override;
    private:
        std::vector<uint8_t> m_buffer;
    };
}