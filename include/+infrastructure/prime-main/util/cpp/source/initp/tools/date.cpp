#include <initp/system/debug.hpp>
#include <initp/tools/date.hpp>

#include <boost/date_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

#include <sstream>
#include <locale>

namespace initp {
namespace tools {
namespace date {

std::string format(system::time_t time, const char* format) {
    std::stringstream date_converter;
    boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
    date_converter.imbue(std::locale(std::locale::classic(), facet));
    facet->format(format);
    boost::posix_time::ptime ptime =
        boost::posix_time::from_time_t(time / 1000) +
        boost::posix_time::milliseconds(time % 1000);
    date_converter << ptime;
    return date_converter.str();
}

system::time_t from_iso_extended_string(const std::string& date) {
    try {
        boost::posix_time::ptime t = boost::posix_time::from_iso_extended_string(date);
        boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
        return (system::time_t)(t - epoch).total_milliseconds();
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "tools::date::from_iso_extended_string", "%s", e.what());
        return 0;
    }
}

int64_t timezone_milliseconds(void) {
    const boost::posix_time::ptime utc = boost::posix_time::second_clock::universal_time();
    const boost::posix_time::ptime local = boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(utc);
    const boost::posix_time::time_duration diff = local - utc;
    return (int64_t)diff.total_milliseconds();
}

}}}
