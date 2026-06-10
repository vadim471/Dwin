//
// Created by vadim.tissen on 12.05.2026.
//

#pragma once
#include "ILogicHandler.hpp"
#include "bridge/core/Settings.hpp"
#include "bridge/database/BosRepository.hpp"
#include "bridge/database/Transaction.hpp"

namespace bridge {
    class BosLogic : public ILogicHandler {
    public:
        BosLogic(const Settings& settings, std::shared_ptr<TransactionRepository> transaction_repo);
        void handle(const Message& message, MessageLayer& core) override;

    private:
        // Хендлер отправки заказов на INIT BOS
        void handleBosSales(const Message& message, MessageLayer& core);

        // Хендлер отправки метрологической ведомости INIT BOS
        void handleBosMetrological(const Message& message, MessageLayer& core);

        Settings m_settings;
        std::shared_ptr<BosRepository> m_bos_repo;
        std::shared_ptr<TransactionRepository> m_transaction_repo;
    };
}
