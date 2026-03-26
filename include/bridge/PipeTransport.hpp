//
// Created by vadim.tissen on 20.11.2025.
//

extern "C" {
#include "initp/platform/named_pipe.h"
#include <initp/system/time.h>
}

#pragma once
#include <thread>

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
        void workerThread();

        std::string m_pipe_path;
        std::atomic<bool> m_running{false};
        std::thread m_worker;
        ReceiveHandler m_receive_handler;
        sys_named_pipe_t m_pipe;

        // Умный указатель на менеджер оплат от разработчиков терминала
        std::unique_ptr<initp::payment::manager> m_manager;
    };
}
