#include <initp/platform/net.hpp>
#include <initp/system/debug.hpp>

#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <cstddef>
#include <sstream>

#include <sys/socket.h>
#include <sys/ioctl.h>
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

}}}
