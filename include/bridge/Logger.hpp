//
// Created by vadim.tissen on 26.02.2026.
//
// Logger.hpp
#pragma once
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/trivial.hpp>
#include <string>

// Используем стандартные уровни серьезности из Boost (trace, debug, info, warning, error, fatal)
namespace logging = boost::log;
namespace src = boost::log::sources;

// Определяем тип нашего логгера
using SystemLogger = src::severity_channel_logger_mt<logging::trivial::severity_level, std::string>;

// Объявляем глобальные (или статические) экземпляры логгеров для разных подсистем
extern SystemLogger uartLogger;
extern SystemLogger httpLogger;
extern SystemLogger arkaimLogger;
extern SystemLogger systemLogger;

// Функция инициализации логгера (вызывать в начале main)
void initLogger(const std::string& logDir = "logs");

// Удобные макросы для записи, чтобы не писать длинные конструкции
#define LOG_UART_TRACE BOOST_LOG_SEV(uartLogger, logging::trivial::trace)
#define LOG_UART_DEBUG BOOST_LOG_SEV(uartLogger, logging::trivial::debug)
#define LOG_UART_INFO  BOOST_LOG_SEV(uartLogger, logging::trivial::info)
#define LOG_UART_WARN  BOOST_LOG_SEV(uartLogger, logging::trivial::warning)
#define LOG_UART_ERROR BOOST_LOG_SEV(uartLogger, logging::trivial::error)
#define LOG_UART_FATAL BOOST_LOG_SEV(uartLogger, logging::trivial::fatal)

#define LOG_HTTP_TRACE BOOST_LOG_SEV(httpLogger, logging::trivial::trace)
#define LOG_HTTP_DEBUG BOOST_LOG_SEV(httpLogger, logging::trivial::debug)
#define LOG_HTTP_INFO  BOOST_LOG_SEV(httpLogger, logging::trivial::info)
#define LOG_HTTP_WARN  BOOST_LOG_SEV(httpLogger, logging::trivial::warning)
#define LOG_HTTP_ERROR BOOST_LOG_SEV(httpLogger, logging::trivial::error)
#define LOG_HTTP_FATAL BOOST_LOG_SEV(httpLogger, logging::trivial::fatal)

#define LOG_ARKAIM_TRACE BOOST_LOG_SEV(arkaimLogger, logging::trivial::trace)
#define LOG_ARKAIM_DEBUG BOOST_LOG_SEV(arkaimLogger, logging::trivial::debug)
#define LOG_ARKAIM_INFO  BOOST_LOG_SEV(arkaimLogger, logging::trivial::info)
#define LOG_ARKAIM_WARN  BOOST_LOG_SEV(arkaimLogger, logging::trivial::warning)
#define LOG_ARKAIM_ERROR BOOST_LOG_SEV(arkaimLogger, logging::trivial::error)
#define LOG_ARKAIM_FATAL BOOST_LOG_SEV(arkaimLogger, logging::trivial::fatal)

#define LOG_SYSTEM_TRACE BOOST_LOG_SEV(systemLogger, logging::trivial::trace)
#define LOG_SYSTEM_DEBUG BOOST_LOG_SEV(systemLogger, logging::trivial::debug)
#define LOG_SYSTEM_INFO  BOOST_LOG_SEV(systemLogger, logging::trivial::info)
#define LOG_SYSTEM_WARN  BOOST_LOG_SEV(systemLogger, logging::trivial::warning)
#define LOG_SYSTEM_ERROR BOOST_LOG_SEV(systemLogger, logging::trivial::error)
#define LOG_SYSTEM_FATAL BOOST_LOG_SEV(systemLogger, logging::trivial::fatal)