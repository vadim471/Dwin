//
// Created by vadim.tissen on 18.11.2025.
//

#pragma once

#include <atomic>
#include <condition_variable>
#include <map>

#include "types.hpp"
#include <memory>

#include "ILogicHandler.hpp"
#include "IMessageParser.hpp"
#include "ITransport.hpp"


//MessageLayer.hpp
namespace bridge {

    class MessageLayer {
    public:
        MessageLayer();
        ~MessageLayer();

        MessageLayer(const MessageLayer&) = delete;

        void registerChannel(const std::string& channel_name,
                            ITransportPtr transport,
                            IMessageParserPtr parser);

        void registerLogic(const std::string& logic_name,
                            ILogicHandlerPtr handler);

        void run();

        void stop();

        void sendTo(const std::string& targetName, const Message& msg);

        void sendToLogicLayer(const std::string &target_logic_name, const Message &message);

    private:
        // Принимает Message, формирует исходящий поток
        void processMessage(const Message& message);

        struct Channel {
            ITransportPtr transport;
            IMessageParserPtr parser;
        };

        // Лоический обработчик для Message
        std::map<std::string, ILogicHandlerPtr> m_logic_handlers;

        // Физические каналы: DWIN -> { Serial, Parser }
        std::map<std::string, Channel> m_channels;

        std::queue<Message> m_queue;
        std::mutex m_mutex;
        std::condition_variable m_cond;
        std::atomic<bool> m_running;
    };
}