#include <initp/log/frontend/logger.hpp>
#include <initp/log/frontend/log.hpp>
#include <initp/log/frontend/tools.hpp>
#include <initp/log/frontend/config.pb.h>
#include <initp/platform/tools.hpp>
#include <initp/generic/singleton.hpp>

#include <iostream>
#include <memory>
#include <cstddef>
#include <set>

namespace initp {
namespace log {

namespace {

struct logger_t {
    static constexpr size_t PRIORITY = 10;
    std::vector<std::unique_ptr<log_base>> loggers;
    logger_t(void) {
        log_base::initialize();
    }
};

static thread_local bool pending_helper = false;

}

    // Construction

log_helper::log_helper(level::value level, int mask)
    : skip_message_(pending_helper)
    , level_(level)
    , output_mask_(mask)
{
    pending_helper = true;
}

log_helper::log_helper(log_helper&& helper)
    : std::stringstream(std::forward<log_helper>(helper))
    , skip_message_(helper.skip_message_)
    , level_(helper.level_)
    , output_mask_(helper.output_mask_)
    , what_(std::move(helper.what_))
    , error_(std::move(helper.error_))
{}

log_helper::~log_helper() {
    // Uncomment to forbid recursive writes from single thread
    //if (this->skip_message_) return;
    try {
        for (const auto& logger : tools::singleton_with_priority<logger_t, logger_t::PRIORITY>()->loggers) {
            if (!(this->output_mask_ & logger->type())) continue;
            logger->write_line(this->level_, this->what_.str(), this->error_.str(), this->str());
        }
    } catch (...) {}
    pending_helper = false;
}

    // Public properties

std::stringstream& log_helper::what(void) {
    return this->what_;
}

std::stringstream& log_helper::error(void) {
    return this->error_;
}

    // Setup functions

void setup(void) {
    logger_t& instance = *tools::singleton_with_priority<logger_t, logger_t::PRIORITY>();
    if (!instance.loggers.empty()) {
        instance.loggers.clear();
    }
    // use stdout as default logger
    std_out_config config;
    config.set_level(log_level::LL_ALL);
    config.set_stderr_level(log_level::LL_ERROR);
    instance.loggers.emplace_back(log_base::create(config));
}

void setup(const char* app, const frontend_config& config) {
    logger_t& instance = *tools::singleton_with_priority<logger_t, logger_t::PRIORITY>();
    if (!instance.loggers.empty()) {
        instance.loggers.clear();
    }
    for (auto logger : config.loggers()) {
        if (logger.has_std_out() && logger.std_out().enabled()) {
            instance.loggers.emplace_back(log_base::create(logger.std_out()));
        } else if (logger.has_file() && logger.file().enabled()) {
            instance.loggers.emplace_back(log_base::create(app, logger.file()));
        } else if (logger.has_http() && logger.http().enabled()) {
            instance.loggers.emplace_back(log_base::create(app, logger.http()));
        } else if (logger.has_vector() && logger.vector().enabled()) {
            instance.loggers.emplace_back(log_base::create(app, logger.vector()));
        }
    }
}

std::vector<std::unique_ptr<log_base>>& get_loggers(void) {
    return tools::singleton_with_priority<logger_t, logger_t::PRIORITY>()->loggers;
}

}}
