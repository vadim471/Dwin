#ifndef INITPLUS_PROCESS_GROUP_HPP_INCLUDED
#define INITPLUS_PROCESS_GROUP_HPP_INCLUDED

#include <string>
#include <vector>

#include <initp/system/error_code.hpp>

#if defined(WIN32)
#include <windows.h>
#elif defined(POSIX)
#include <unistd.h>
#endif // WIN32|POSIX

namespace initp {
namespace system {

class process_group {
public: // Private classes
    class process {
    public:
        process(void);
        process(process_group*, const std::string&, const std::string&);
        ~process(void);
    public:
        error_code create(const std::string& path, const std::string& argv);
        error_code run(const std::string& path, const std::string& argv);
        void poll(void);
        void terminate(void);
    private:
        process_group* parent_;
        std::string path_;
        std::string argv_;
        bool started_;
        bool restart_;
        #if defined(WIN32)
        STARTUPINFO si_;
        PROCESS_INFORMATION pi_;
        #elif defined(POSIX)
        pid_t pid_;
        #endif // WIN32|POSIX
    };
    friend class process;
public: // Construction
    process_group(void);
    ~process_group(void);
public: // Public methods
    error_code create(const std::string& path, const std::string& argv_first_start, const std::string& argv_on_restart);
    void poll(void);
    void terminate_all(void);
private: // Private fields
    std::vector<process> list_;
    #ifdef WIN32
    HANDLE job_;
    #endif // WIN32
};

}}

#endif // INITPLUS_PROCESS_GROUP_HPP_INCLUDED
