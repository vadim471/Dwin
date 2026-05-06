#pragma once

#include "IMessageParser.hpp"

namespace bridge {
    class ArkaimParser : public IMessageParser {
    public:
        ArkaimParser() = default;
        ~ArkaimParser() override = default;

        std::vector<Message> parse(const RawData& raw_data, const std::string& message_source) override;
        RawData serialize(const Message& message) override;
};

using ArkaimParserPtr = std::shared_ptr<ArkaimParser>;

} // namespace bridge
