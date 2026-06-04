//
// Created by vadim.tissen on 18.11.2025.
//

#include "bridge/core/MessageLayer.hpp"

#include <iostream>

#include "bridge/logic/PrimeLogic.hpp"

//MessageLayer.cpp
namespace bridge {
    MessageLayer::MessageLayer() : m_running(false) {
    }

    MessageLayer::~MessageLayer() { stop(); }

    void MessageLayer::stop() {
        m_running = false;
        m_cond.notify_all();

        for (auto &pair: m_channels) {
            pair.second.transport->stop();
        }
    }

    void MessageLayer::registerChannel(const std::string &channel_name, ITransportPtr transport,
                                       IMessageParserPtr parser) {
        m_channels[channel_name] = {transport, parser};

        transport->setReceiveHandler(
            [this, channel_name, parser](const RawData &raw_data) {
                auto message = parser->parse(raw_data, channel_name);

                if (!message.empty()) {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    for (const auto &m: message) {
                        m_queue.push(m);
                    }
                    m_cond.notify_one();
                }
            });
    }

    void MessageLayer::run() {
        m_running = true;

        for (auto &pair: m_channels) {
            pair.second.transport->start();
        }

        std::cout << "Running Core" << std::endl;

        while (m_running) {


            Message message;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cond.wait(lock, [this] { return !m_running || !m_queue.empty(); });

                if (!m_running && m_queue.empty()) break;

                message = m_queue.front();
                m_queue.pop();
            }
            processMessage(message);
        }
    }

    void MessageLayer::sendTo(const std::string &target_name, const Message &message) {
        auto it = m_channels.find(target_name);

        //std::cerr << "Find Channel " << std::endl;
        if (it != m_channels.end()) {
            RawData raw_data = it->second.parser->serialize(message);
            it->second.transport->send(raw_data); //doWrite Transport class (example SerialTransport::doWrite())
        } else {
            std::cerr << "Could not find channel " << target_name << std::endl;
        }
    }

    void MessageLayer::processMessage(const Message &message) {
        auto it = m_logic_handlers.find(message.source);

        if (it != m_logic_handlers.end()) {
            // Передаем сообщение и ссылку на себя (*this), чтобы хендлер мог отвечать.
            it->second->handle(message, *this);
        } else {
            std::cout << "[Core] No logic registered for source: " << message.source
                    << " (Message type: " << message.type << ")" << std::endl;
        }
    }

    void MessageLayer::registerLogic(const std::string &logic_name, ILogicHandlerPtr handler) {
        m_logic_handlers[logic_name] = handler;
    }

    void MessageLayer::sendToLogicLayer(const std::string &target_logic_name, const Message &message) {
        auto it = m_logic_handlers.find(target_logic_name);

        if (it != m_logic_handlers.end()) {
            it->second->handle(message, *this);
        } else {
            std::cerr << "[Core] Target Logic not found: " << target_logic_name << std::endl;
        }
    }
}
