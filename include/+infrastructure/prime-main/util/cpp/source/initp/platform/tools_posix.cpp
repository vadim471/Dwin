#include <initp/platform/tools.hpp>
#include <initp/system/debug.hpp>

#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <cstddef>
#include <sstream>

#include <pthread.h>
#include <sys/stat.h>

namespace initp {
namespace system {
namespace tools {

template<typename T>
inline T read_file_value(const char* path) {

    T result = (T)0;
    FILE* f = nullptr;

    f = fopen(path, "r");

    if (f) {

        const size_t BUFFER_SIZE = 0x10;
        char buffer[BUFFER_SIZE];
        fgets(buffer, BUFFER_SIZE, f);
        fclose(f);

        std::stringstream stream;
        stream << std::hex << buffer;
        stream >> result;
    }
    return result;
}

inline std::string read_file_string(const char* path) {

    FILE* f = fopen(path, "r");
    if (!f) return {};

    const size_t BUFFER_SIZE = 0x10;
    char buffer[BUFFER_SIZE];
    fgets(buffer, BUFFER_SIZE, f);
    fclose(f);

    return {buffer};
}

inline bool is_file_exists(const std::string& name) {
    struct stat buffer;
    return stat(name.c_str(), &buffer) == 0;
}

std::string get_cpu_id(void) {
    if (is_file_exists("/etc/prime/rev")) {
        std::stringstream stream;
        stream << read_file_string("/etc/prime/rev");
        stream << "_" << std::hex;
        stream << read_file_value<uint32_t>("/etc/prime/HW_OCOTP_CFG0");
        return stream.str();
    }
    uint32_t id = read_file_value<uint32_t>("/sys/fsl_otp/HW_OCOTP_CFG1");
    uint32_t series = read_file_value<uint32_t>("/sys/fsl_otp/HW_OCOTP_CFG0");
    std::stringstream stream;
    switch (series) {
        case 0x603fd545:
            stream << "fsl0a_";
            break;
        case 0x60428128:
        case 0x63e0c294:
            stream << "fsl0b_";
            break;
        default:
            sys_debug_print(SYSTEM_LEVEL_WARNING, "get_cpu_id", "Unknown series 0x%08x", series);
            if (is_file_exists("/dev/ttymxc3")) {
                stream << "fsl0b_";
            } else if (is_file_exists("/dev/ttymxc4")) {
                stream << "fsl0a_";
            } else {
                sys_debug_print(SYSTEM_LEVEL_FATAL, "get_cpu_id", "Failed to detect device");
                stream << "undef_";
            }
            break;
    }
    stream << std::hex << id;
    return stream.str();
}

uint32_t get_pid(const std::string& name) {
    char buf[512];
    std::string cmd = "pidof -s " + name;
    FILE* cmd_pipe = popen(cmd.c_str(), "r");

    fgets(buf, 512, cmd_pipe);
    pid_t pid = strtoul(buf, NULL, 10);

    pclose(cmd_pipe);
    return (uint32_t)pid;
}

size_t current_thread_id(void) {
    return (size_t)pthread_self();
}

}}}
