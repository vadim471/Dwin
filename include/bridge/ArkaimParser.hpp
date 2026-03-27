//
// Created by vadim.tissen on 26.03.2026.
//

#pragma once

#include "IMessageParser.hpp"
#include <infrastructure/itp-cpp/include/itp/frame.hpp>

namespace bridge {
    class ArkaimParser : public IMessageParser {
    public:
        ArkaimParser() = default;
        ~ArkaimParser() override = default;

        std::vector<Message> parse(const RawData& raw_data, const std::string& message_source) override;
        RawData serialize(const Message& message) override;

    private:
        std::vector<uint8_t> m_buffer;
    };
}