//
// Created by vadim.tissen on 02.12.2025.
//

#pragma once

#include "types.hpp"
#include <queue>
#include <memory>

//IMessageParser.hpp
namespace bridge {
    class IMessageParser {
    public:
        virtual ~IMessageParser() = default;

        // Сырые данные в сообщение.
        virtual std::vector<Message> parse(const RawData& raw_data, const std::string& message_source) = 0;

        // Превращает сообщение в сырые данные для отправки.
        virtual RawData serialize(const Message& message) = 0;
    };

    using IMessageParserPtr = std::shared_ptr<IMessageParser>;
}