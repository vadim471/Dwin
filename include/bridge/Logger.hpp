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

// Удобные макросы для записи, чтобы не писать длинные конструкции
#define LOG_UART_ERROR BOOST_LOG_SEV(uartLogger, logging::trivial::error)
#define LOG_UART_INFO  BOOST_LOG_SEV(uartLogger, logging::trivial::info)

#define LOG_HTTP_ERROR BOOST_LOG_SEV(httpLogger, logging::trivial::error)
#define LOG_HTTP_DEBUG BOOST_LOG_SEV(httpLogger, logging::trivial::debug)