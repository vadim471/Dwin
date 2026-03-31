#ifndef INITPLUS_LOG_BOOST_LOG_HPP_INCLUDED
#define INITPLUS_LOG_BOOST_LOG_HPP_INCLUDED

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/system/error_code.hpp>

#include <string>

namespace initp {
namespace system {
namespace log {

enum byte_order_mark_t {
    BOM_NONE = 0,
    BOM_UTF8,
    BOM_UTF16_BE,
    BOM_UTF16_LE
};

extern boost::log::sources::severity_logger<boost::log::trivial::severity_level> lg;

void initialize(const std::string& target, const std::string& filename, byte_order_mark_t bom, bool = true, uint32_t = 5, uint32_t = 256, uint32_t = 512, uint32_t = 128);

void stop(void);

}}}

#define LOG_TRACE(what) BOOST_LOG_SEV(initp::system::log::lg, boost::log::trivial::trace) << "<" << what << ">: "
#define LOG_DEBUG(what) BOOST_LOG_SEV(initp::system::log::lg, boost::log::trivial::debug) << "<" << what << ">: "
#define LOG_INFO(what) BOOST_LOG_SEV(initp::system::log::lg, boost::log::trivial::info) << "<" << what << ">: "
#define LOG_WARNING(what) BOOST_LOG_SEV(initp::system::log::lg, boost::log::trivial::warning) << "<" << what << ">: "
#define LOG_ERROR(what) BOOST_LOG_SEV(initp::system::log::lg, boost::log::trivial::error) << "<" << what << ">: "
#define LOG_FATAL(what) BOOST_LOG_SEV(initp::system::log::lg, boost::log::trivial::fatal) << "<" << what << ">: "

#define ELOG_TRACE(err, what) BOOST_LOG_SEV(initp::system::log::lg, boost::log::trivial::trace) << "<" << what << "> <" << err << ">: "
#define ELOG_DEBUG(err, what) BOOST_LOG_SEV(initp::system::log::lg, boost::log::trivial::debug) << "<" << what << "> <" << err << ">: "
#define ELOG_INFO(err, what) BOOST_LOG_SEV(initp::system::log::lg, boost::log::trivial::info) << "<" << what << "> <" << err << ">: "
#define ELOG_WARNING(err, what) BOOST_LOG_SEV(initp::system::log::lg, boost::log::trivial::warning) << "<" << what << "> <" << err << ">: "
#define ELOG_ERROR(err, what) BOOST_LOG_SEV(initp::system::log::lg, boost::log::trivial::error) << "<" << what << "> <" << err << ">: "
#define ELOG_FATAL(err, what) BOOST_LOG_SEV(initp::system::log::lg, boost::log::trivial::fatal) << "<" << what << "> <" << err << ">: "

#endif // INITPLUS_LOG_BOOST_LOG_HPP_INCLUDED
