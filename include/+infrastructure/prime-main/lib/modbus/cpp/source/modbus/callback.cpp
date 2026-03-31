#include <modbus/callback.hpp>

#include <initp/generic/function_generic.hpp>

extern "C" {

void mbs_free_eparam(size_t address) {
    initp::tools::function<>::free(address);
}

}
