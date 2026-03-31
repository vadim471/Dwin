#include <initp/http/ranged/file_writer.hpp>

#include <initp/system/debug.hpp>

#include <boost/filesystem.hpp>

#include <iostream>

namespace initp {
namespace http {
namespace ranged {

    // Construction

file_writer::file_writer(void) {}

    // Private methods

system::error_code file_writer::open_file(const std::string& filename) {
    if (this->file_.is_open()) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::file_writer::open_file", "File already opened");
        return system::err::invalid_state;
    }
    try {
        boost::system::error_code ec;
        boost::filesystem::path path(filename);
        boost::filesystem::path dir = path.parent_path();
        if (!boost::filesystem::exists(dir)) {
            if (!boost::filesystem::create_directories(dir, ec)) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::file_writer::open_file", "Failed to create directories");
                return system::err::generic_error;
            }
        }
        this->file_.open(path, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::file_writer::open_file", "Failed to open file");
        return system::err::generic_error;
    }
    return system::err::success;
}

system::error_code file_writer::write_file(const uint8_t* data, size_t size) {
    if (!this->file_.is_open()) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::file_writer::write_file", "File not opened");
        return system::err::invalid_state;
    }
    if (data && size) {
        try {
            this->file_.write((const char*)data, size);
        } catch (std::exception& e) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::file_writer::write_file", "Failed to write file");
            return system::err::generic_error;
        }
    }
    return system::err::success;
}

void file_writer::close_file(void) {
    if (this->file_.is_open()) {
        this->file_.close();
    }
}

}}}
