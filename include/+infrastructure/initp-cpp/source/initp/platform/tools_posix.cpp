#include <initp/platform/tools.hpp>
#include <initp/system/debug.hpp>

#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <cstddef>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

namespace initp {
namespace system {
namespace tools {

std::string get_host_name(void) {
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    return std::string(hostname);
}

std::string get_user_name(void) {
    char username[LOGIN_NAME_MAX];
    getlogin_r(username, LOGIN_NAME_MAX);
    return std::string(username);
}

std::string get_ip_address(const std::string& name) {

    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd < 0) return std::string("0.0.0.0");

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
        return std::string("0.0.0.0");
    }

    close(fd);

    /* return result */
    return std::string(inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
}

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

inline bool is_file_exists(const std::string& name) {
    struct stat buffer;
    return stat(name.c_str(), &buffer) == 0;
}

std::string get_cpu_id(void) {
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

}}}
