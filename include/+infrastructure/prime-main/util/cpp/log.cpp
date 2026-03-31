#include <iostream>

#include <initp/log/frontend/config.pb.h>
#include <initp/log/frontend/logger.hpp>

using namespace initp;

#define APP_NAME "test"

int main(int argc, char** argv) {
    log::frontend_config config;
    log::frontend_config::logger_config* logger;

    logger = config.add_loggers();
    logger->mutable_std_out()->set_enabled(true);
    logger->mutable_std_out()->set_level(log::LL_INFO);
    logger->mutable_std_out()->set_stderr_level(log::LL_ERROR);

    logger = config.add_loggers();
    logger->mutable_file()->set_enabled(true);
    logger->mutable_file()->set_async(true);
    logger->mutable_file()->set_level(log::LL_INFO);
    logger->mutable_file()->set_path("logs");
    logger->mutable_file()->set_rotation_size(5);
    logger->mutable_file()->set_max_files_size(512);
    logger->mutable_file()->set_max_files_count(512);
    logger->mutable_file()->set_min_free_space(128);

    logger = config.add_loggers();
    logger->mutable_vector()->set_enabled(false);
    logger->mutable_vector()->set_level(log::LL_INFO);
    logger->mutable_vector()->set_host("127.0.0.1");
    logger->mutable_vector()->set_delay(5);
    logger->mutable_vector()->set_max_batch_size(20);
    logger->mutable_vector()->set_max_queue_size(1000);

    log::setup();
    log::trace("main") << "value: " << 2;

    log::setup(APP_NAME, config);
    log::error("main", "err:1", log::LO_FILE) << "value: " << 3;
    log::debug("main") << "value: " << 4;
    return 0;
}
