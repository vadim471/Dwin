//
// Created by vadim.tissen on 26.02.2026.
//

//
// Created by vadim.tissen on 26.02.2026.
//

#include "bridge/core/Logger.hpp"
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/core/null_deleter.hpp>
#include <iostream>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;

// Инициализируем логгеры и привязываем их к конкретным каналам
SystemLogger uartLogger(keywords::channel = "UART");
SystemLogger httpLogger(keywords::channel = "HTTP");
SystemLogger arkaimLogger(keywords::channel = "ARKAIM");
SystemLogger systemLogger(keywords::channel = "SYSTEM");

void initLogger(const std::string& logDir) {
    // Добавляем общие атрибуты (время, номер строки и т.д.)
    logging::add_common_attributes();

    // Формат вывода: [Timestamp] [Channel] [Severity] Message
    auto format = expr::stream
        << "[" << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") << "]"
        << " [" << expr::attr<std::string>("Channel") << "]"
        << " [" << logging::trivial::severity << "] "
        << expr::smessage;

    // 1. Логирование в консоль
    auto consoleSink = logging::add_console_log(
        std::cout,
        keywords::format = format,
        keywords::auto_flush = true
    );

    // 2. Логирование в файл с ротацией
    // Файлы будут называться: app_2026-04-30_001.log, app_2026-04-30_002.log и т.д.
    auto fileSink = logging::add_file_log(
        keywords::file_name = logDir + "/app_%Y-%m-%d_%N.log",
        keywords::rotation_size = 10 * 1024 * 1024,  // Ротация каждые 10 МБ
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),  // Ротация каждый день в полночь
        keywords::format = format,
        keywords::auto_flush = true,
        keywords::open_mode = std::ios_base::app  // Дописывать в конец файла
    );

    // 3. Отдельный файл для ошибок
    auto errorSink = logging::add_file_log(
        keywords::file_name = logDir + "/errors_%Y-%m-%d_%N.log",
        keywords::rotation_size = 5 * 1024 * 1024,
        keywords::format = format,
        keywords::auto_flush = true,
        keywords::open_mode = std::ios_base::app
    );

    // Фильтр: только ошибки и критические сообщения
    errorSink->set_filter(
        logging::trivial::severity >= logging::trivial::error
    );

    // минимальный уровень логирования (можно изменить на debug, trace и т.д.)
    logging::core::get()->set_filter(
        logging::trivial::severity >= logging::trivial::debug
    );

    LOG_SYSTEM_INFO << "Logger initialized. Log directory: " << logDir;
}
