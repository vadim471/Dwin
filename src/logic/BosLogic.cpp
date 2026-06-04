//
// Created by vadim.tissen on 12.05.2026.
//


#include "bridge/logic/BosLogic.hpp"

#include "bridge/core/MessageLayer.hpp"

namespace bridge {
    BosLogic::BosLogic(const Settings& settings) : m_settings(settings) {};

    void BosLogic::handle(const Message &message, MessageLayer &core) {
        if (message.type == BOS_MESSAGE_SET_SALES) {
            handleBosSales(message, core);
            return;
        }
        if (message.type == BOS_MESSAGE_SET_METROLOGICAL) {
            handleBosMetrological(message, core);
            return;
        }
    }

    void BosLogic::handleBosSales(const Message &message, MessageLayer &core) {
        Message request;

        request.source = BOS_HTTP_LAYER;
        request.type = SET_SALES;
        request.payload = message.payload;

        core.sendTo(BOS_HTTP_LAYER, request);
    }

    void BosLogic::handleBosMetrological(const Message &message, MessageLayer &core) {
        Message request;

        request.source = BOS_HTTP_LAYER;
        request.type = SET_METROLOGICAL;
        request.payload = message.payload;

        core.sendTo(BOS_HTTP_LAYER, request);
    }
    //
    // Message refund_msg;
    // refund_msg.source = PRIME_HTTP_LAYER;
    // refund_msg.type = PRINT_REFUND_RECEIPT;
    // refund_msg.resource_id = order.id;
    // refund_msg.payload = serializeReceiptData(receipt);
    //
    // core.sendToLogicLayer(PIPE_LAYER, refund_msg);
}
