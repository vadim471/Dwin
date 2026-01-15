//
// Created by vadim.tissen on 04.12.2025.
//

#pragma once
#include <future>

#include "ILogicHandler.hpp"
#include "MessageLayer.hpp"
#include "Settings.hpp"

namespace bridge {
    class DwinLogic : public ILogicHandler {
    public:
        // virtual ~DwinLogic() = default;
        DwinLogic(const Settings& settings);

        // ILogicHandler
        void handle(const Message& message, MessageLayer& core) override;

        Message sendAndReceive(const Message& request, MessageLayer& core);
    private:
        std::mutex m_mutex;

        void handleDwinEvent(const Message& message, MessageLayer& core);

        void handleFuelVolume(const Message &message, MessageLayer& core);

        void handleAcceptOrder(MessageLayer& core);

        // Очередь ожидающих запросов.
        // Мы храним std::promise, чтобы передать значение (Message) из одного потока в другой.
        std::queue<std::promise<Message>> m_pendingRequests;

        Settings m_settings;
    };

    using DwinLogicPtr = std::shared_ptr<DwinLogic>;
}
