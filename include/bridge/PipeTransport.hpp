//
// Created by vadim.tissen on 20.11.2025.
//


#pragma once
#include "ITransport.hpp"

namespace bridge {
    class PipeTransport : public ITransport {
    public:
        PipeTransport();
        ~PipeTransport();

        // ITransport
        void stop() override;
        void start() override;
        void setReceiveHandler(ReceiveHandler handler) override;
        void send(const RawData& pkt) override;

    private:
    };
}
