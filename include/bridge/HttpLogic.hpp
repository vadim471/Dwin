//
// Created by vadim.tissen on 10.12.2025.
//
#pragma once
#include <future>
#include <mutex>
#include <queue>
#include "bridge/json.hpp"

#include "ILogicHandler.hpp"
#include "Settings.hpp"

//HttpLogic.hpp
namespace bridge {
    using json = nlohmann::json;

    class HttpLogic : public ILogicHandler {
    public:
        HttpLogic(const Settings& settings);

        void handle(const Message& msg, MessageLayer& core) override;

        static void getDevices(MessageLayer& core);
        static void getParameters(MessageLayer& core, std::string& id);
        static void getProducts(MessageLayer& core);
        static void getDispenserStatus(MessageLayer& core);
        void startPolling(MessageLayer& core) const;
    private:
        void processEvent(const json& jArray, MessageLayer& core);

        static void processDevices(const json& jArray, MessageLayer& core);
        static void processParameters(const json& jArray, MessageLayer& core);
        void processDispenserStatus(const json& jArray, MessageLayer& core);
        void processProducts(const json& jArray);
        void processHandle(const json& jArray, MessageLayer& core);

        // Для начала пролива заказа нужно узнать id команды и ждать, пока ok = true
        static void processTask(const json& jArray, MessageLayer& core);

        static void processTaskId(const json& jArray, MessageLayer& core);

        // Хендлеры обработки event ТРК
        void handleDispenserStatus(const json& jArray, MessageLayer& core) const;
        void hanldeDispenserDisplay(const json& jArray, MessageLayer& core) const;

        static void sendTaskToHttp(MessageLayer& core, int id);

        std::chrono::steady_clock m_lastPollTime;
        bool m_waitingForHttpResponse = false;
        std::mutex m_mutex;
        std::queue<std::promise<Message>> m_pendingRequest;
        std::map<std::string, std::string> m_product_names; //fuel type info
        long long m_lastId = 0;
        Settings m_settings;
        Order m_current_order;
    };

    using HttpLogicPtr = std::shared_ptr<HttpLogic>;
}