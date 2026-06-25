//
// Created by vadim.tissen on 12.05.2026.
//

#pragma once
#include "ILogicHandler.hpp"
#include "bridge/core/Settings.hpp"
#include "bridge/database/BosRepository.hpp"
#include "bridge/database/FuelReceptionRepository.hpp"
#include "bridge/database/LevelGaugeRepository.hpp"
#include "bridge/database/TransactionRepository.hpp"

namespace bridge {
    class BosLogic : public ILogicHandler {
    public:
        BosLogic(const Settings& settings, std::shared_ptr<TransactionRepository> transaction_repo,
            std::shared_ptr<LevelGaugeRepository> level_gauge_repo,
            std::shared_ptr<FuelReceptionRepository> fuel_reception_repo);
        void handle(const Message& message, MessageLayer& core) override;

    private:
        // Хендлер отправки заказов на INIT BOS
        void handleBosSales(const Message& message, MessageLayer& core);

        // Хендлер отправки метрологической ведомости INIT BOS
        void handleBosMetrological(const Message& message, MessageLayer& core);

        // Хендлер сохранения пополнения резервуаров.
        void handleBosReceipt(const Message& message, MessageLayer& core);

        // Хендлер обработки ответов от 1С сервера. Нужен для сверки, принял ли удаленный сервер наш запрос => пометить в БД, как отправленный.
        void handleHttpResponse(const Message& message, MessageLayer& core);

        Settings m_settings;
        std::shared_ptr<BosRepository> m_bos_repo;
        std::shared_ptr<TransactionRepository> m_transaction_repo;
        std::shared_ptr<LevelGaugeRepository> m_level_gauge_repo;
        std::shared_ptr<FuelReceptionRepository> m_fuel_reception_repo;
    };
}
