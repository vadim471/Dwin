#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include <initp/log/boost/log.hpp>

#include <sstream>
#include <functional>

namespace initp {
namespace system {
namespace log {

typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> text_sink;

boost::log::sources::severity_logger<boost::log::trivial::severity_level> lg;
boost::shared_ptr<text_sink> file_sink;

void on_open_handler(boost::log::sinks::text_file_backend::stream_type& stream, byte_order_mark_t bom) {
    switch (bom) {
        case BOM_UTF8:
            stream << char(0xEF) << char(0xBB) << char(0xBF);
            break;
        case BOM_UTF16_BE:
            stream << char(0xFE) << char(0xFF);
            break;
        case BOM_UTF16_LE:
            stream << char(0xFF) << char(0xFE);
            break;
        default:
            break;
    }
}

void initialize(
    const std::string& target_folder, const std::string& filename, byte_order_mark_t bom, bool auto_flush,
    uint32_t rotation_size, uint32_t max_files_size, uint32_t max_files_count, uint32_t min_free_space
) {

    if (file_sink) return;

    namespace fs = boost::filesystem;
    fs::path target(target_folder);
    std::string full_name;
    std::stringstream stream;
    stream << u8"%Y-%m-%d--%H-%M-%S--" << filename << u8"-%N.log";
    target.append(stream.str());

    boost::shared_ptr<boost::log::sinks::text_file_backend> report_backend(
        boost::make_shared<boost::log::sinks::text_file_backend>(
            boost::log::keywords::file_name = target.generic_string(),
            boost::log::keywords::rotation_size = rotation_size * 1024 * 1024,
            boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
            boost::log::keywords::open_mode = std::ios_base::app,
            boost::log::keywords::auto_flush = auto_flush
        )
    );
    report_backend->set_file_collector(
        boost::log::sinks::file::make_collector(
            boost::log::keywords::target = target_folder,
            boost::log::keywords::max_size = max_files_size * 1024 * 1024,
            boost::log::keywords::min_free_space = min_free_space * 1024 * 1024,
            boost::log::keywords::max_files = max_files_count
        )
    );
    report_backend->set_open_handler(std::bind(&on_open_handler, std::placeholders::_1, bom));
    report_backend->scan_for_files();

    boost::shared_ptr<text_sink> file_sink = boost::make_shared<text_sink>(report_backend);
    file_sink->set_formatter(
        boost::log::expressions::format("%1% %2% <%3%> %4%")
            % boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f")
            % boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID")
            % boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity")
            % boost::log::expressions::message
    );

    file_sink->set_filter(boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity") >= boost::log::trivial::trace);
    boost::log::core::get()->add_sink(file_sink);
    boost::log::add_common_attributes();
}

void stop(void) {
    boost::log::core::get()->set_logging_enabled(false);
    boost::log::core::get()->flush();
    boost::log::core::get()->remove_all_sinks();
    if (file_sink) file_sink.reset();
}

}}}
