#include <initp/platform/tools.hpp>

#include <processthreadsapi.h>

namespace initp {
namespace system {
namespace tools {

std::string get_cpu_id(void) {
    return std::string("windows");
}

uint32_t get_pid(const std::string& name) {
    // Not implemented
    return 0;
}

size_t current_thread_id(void) {
    return (size_t)GetCurrentThreadId();
}

}}}
