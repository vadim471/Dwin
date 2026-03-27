#ifndef INITPLUS_HTTP_RANGED_FILE_WRITER_HPP_INCLUDED
#define INITPLUS_HTTP_RANGED_FILE_WRITER_HPP_INCLUDED

#include <initp/system/error_code.hpp>

#include <boost/filesystem/fstream.hpp>

#include <cstdint>
#include <cstddef>

namespace initp {
namespace http {
namespace ranged {

class file_writer {

private: // Private types
    typedef file_writer self_type;

public: // Construction
    file_writer(void);
    file_writer(const self_type&) = delete;
    virtual ~file_writer(void) = default;

public: // Private methods
    system::error_code open_file(const std::string&);
    system::error_code write_file(const uint8_t*, size_t);
    void close_file(void);

protected: // Private fields
    boost::filesystem::ofstream file_;
};

}}}

#endif // INITPLUS_HTTP_RANGED_FILE_WRITER_HPP_INCLUDED
