#include <itp/callback.hpp>

#include <initp/tools/function_generic.hpp>

extern "C" {

void itp_free_user_handler(void* ptr, size_t address) {
    if (address) initp::tools::function<>::free(address);
}

}
