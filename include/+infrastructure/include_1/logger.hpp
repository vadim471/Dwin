#pragma once

#include <itp/logger.hpp>

extern "C"
{
#include <initp/system/debug.h>
#include <itp/debug.h>
#include <itp/error_code.h>
}

#include <itp/tools.hpp>
#include <boost/format.hpp>
#include <helpers/LoggerHelper.hpp>

void log_trace(const std::string &target, const std::string &message);
void log_trace(const std::string &target, const boost::basic_format<char>& formattedMessage);
void log_debug(const std::string &target, const std::string &message);
void log_debug(const std::string &target, const boost::basic_format<char>& formattedMessage);
void log_info(const std::string &target, const std::string &message);
void log_info(const std::string &target, const boost::basic_format<char>& formattedMessage);
void log_warn(const std::string &target, const std::string &message);
void log_warn(const std::string &target, const boost::basic_format<char>& formattedMessage);
void log_error(const std::string &target, const std::string &message);
void log_error(const std::string &target, const boost::basic_format<char>& formattedMessage);
void log_fatal(const std::string &target, const std::string &message);
void log_fatal(const std::string &target, const boost::basic_format<char>& formattedMessage);

