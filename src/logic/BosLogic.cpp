//
// Created by vadim.tissen on 12.05.2026.
//


#include "bridge/logic/BosLogic.hpp"

#include "bridge/bos/FuelLevelData.hpp"
#include "bridge/bos/FuelReceiptionData.hpp"
#include "bridge/bos/SalesData.hpp"
#include "bridge/database/TransactionRepository.hpp"
#include "bridge/core/MessageLayer.hpp"
#include "bridge/core/utility.hpp"
#include "bridge/database/FuelReceptionRepository.hpp"
#include "bridge/database/LevelGaugeRepository.hpp"

namespace bridge {
    BosLogic::BosLogic(const Settings &settings, std::shared_ptr<TransactionRepository> transaction_repo,
        std::shared_ptr<LevelGaugeRepository> level_gauge_repo, std::shared_ptr<FuelReceptionRepository> fuel_reception_repository) :
    m_settings(settings),
    m_transaction_repo(transaction_repo),
    m_level_gauge_repo(level_gauge_repo),
    m_fuel_reception_repo(fuel_reception_repository)
    {
    };

    void BosLogic::handle(const Message &message, MessageLayer &core) {
        if (message.type == BOS_MESSAGE_SET_SALES) {
            handleBosSales(message, core);
            return;
        }
        if (message.type == BOS_MESSAGE_SET_METROLOGICAL) {
            handleBosMetrological(message, core);
            return;
        }
        if (message.type == BOS_MESSAGE_SET_RECEIPTION) {
            handleBosReceipt(message, core);
            return;
        }

        if (message.type == HTTP_RESPONSE) {
            handleHttpResponse(message, core);
        }
    }

    void BosLogic::handleBosReceipt(const Message &message, MessageLayer &core) {
        try {
            std::string json_str(message.payload.begin(), message.payload.end());
            auto j = nlohmann::json::parse(json_str);
            FuelReceptionRecord f_data;

            parseFuelReceiptFromJson(j, f_data);

            int64_t db_id = m_fuel_reception_repo->insert(f_data);

            Message request;

            request.source = BOS_HTTP_LAYER;
            request.type = SET_FUEL_RECEIPTION;
            request.resource_id = std::to_string(db_id);
            request.payload = message.payload;

            core.sendTo(BOS_HTTP_LAYER, request);
        } catch (const nlohmann::json::exception &e) {
            std::cerr << "[BosLogic] JSON parse error in handleBosSales: " << e.what() << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "[BosLogic] Database error: " << e.what() << std::endl;
        }
    }

    void BosLogic::handleBosSales(const Message &message, MessageLayer &core) {
        try {
            std::string json_str(message.payload.begin(), message.payload.end());
            auto j = nlohmann::json::parse(json_str);
            TransactionData t_data;

            parseTransactionFromJson(j, t_data);

            int64_t db_id = m_transaction_repo->insert(t_data);

            Message request;

            request.source = BOS_HTTP_LAYER;
            request.type = SET_SALES;
            request.resource_id = std::to_string(db_id);
            request.payload = message.payload;

            core.sendTo(BOS_HTTP_LAYER, request);
        } catch (const nlohmann::json::exception &e) {
            std::cerr << "[BosLogic] JSON parse error in handleBosSales: " << e.what() << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "[BosLogic] Database error: " << e.what() << std::endl;
        }
    }

    void BosLogic::handleHttpResponse(const Message &response, MessageLayer &core) {
        if (response.status_code >= 200 && response.status_code < 300) {
            try {
                if (!response.resource_id.empty()) {
                    int64_t db_id = std::stoll(response.resource_id);

                    m_transaction_repo->markAsSent(db_id);

                    std::cout << "[BosLogic] Transaction " << db_id << " successfully synced to 1C." << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "[BosLogic] Error parsing resource_id: " << e.what() << std::endl;
            }
        } else {
            std::cerr << "[BosLogic] Failed to sync. 1C returned status: " << response.status_code << std::endl;
        }
    }

    void BosLogic::handleBosMetrological(const Message &message, MessageLayer &core) {
        try {
            std::string json_str(message.payload.begin(), message.payload.end());
            auto j = nlohmann::json::parse(json_str);
            LevelGaugeData lg_data;
            parseLevelGaugeRecordFromJson(j, lg_data);

            int64_t db_id = m_level_gauge_repo->insert(lg_data);

            Message request;

            request.source = BOS_HTTP_LAYER;
            request.type = SET_METROLOGICAL;
            request.payload = message.payload;

            core.sendTo(BOS_HTTP_LAYER, request);
        } catch (const nlohmann::json::exception &e) {
            std::cerr << "[BosLogic] JSON parse error in handleBosMetrological: " << e.what() << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "[BosLogic] Database error: " << e.what() << std::endl;
        }
    }
}
