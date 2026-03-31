#include <initp/platform/named_pipe.hpp>
#include <initp/tools/function_generic.hpp>

namespace initp {
namespace system {

    // Construction

named_pipe::named_pipe(const char* path):
    opened_(false) {
    sys_init_named_pipe(&this->c_object_, path);
}

named_pipe::named_pipe(const std::string& path):
    opened_(false) {
    sys_init_named_pipe(&this->c_object_, path.c_str());
}

named_pipe::~named_pipe(void) {
    this->close();
    tools::function<on_connect_f>::free(this->c_object_.on_connect_eparam);
    tools::function<on_connect_f>::free(this->c_object_.on_disconnect_eparam);
    sys_delete_named_pipe(&this->c_object_);
}

    // Public methods

bool named_pipe::create(void) {
    if (this->opened_) return true;
    uint8_t result = sys_create_named_pipe(&this->c_object_);
    return result ? true : false;
}

bool named_pipe::open(void) {
    if (this->opened_) return true;
    if (sys_open_named_pipe(&this->c_object_, 1))
        this->opened_ = true;
    return this->opened_;
}

void named_pipe::close(void) {
    if (this->opened_) {
        sys_close_named_pipe(&this->c_object_);
        this->opened_ = false;
    }
}

    // Endpoint methods

size_t named_pipe::read(char* data, size_t size) {
    return sys_read_named_pipe(&this->c_object_, data, size);
}

size_t named_pipe::write(const char* data, size_t length) {
    return sys_write_named_pipe(&this->c_object_, data, length);
}

    // Private methods

void named_pipe::call_on_connect(size_t eparam) {
    //
}

    // Public properties

sys_named_pipe_tp named_pipe::c_object(void) {
    return &this->c_object_;
}

bool named_pipe::opened(void) const {
    return this->opened_;
}

void named_pipe::on_connect(on_connect_f handler) {
    size_t eparam;
    sys_on_named_pipe_connect_fp f;
    tools::function<on_connect_f>::free(this->c_object_.on_connect_eparam);
    std::tie(f, eparam) = tools::function<on_connect_f>::bind(handler);
    if (f) {
        this->c_object_.on_connect = f;
        this->c_object_.on_connect_eparam = eparam;
    }
}

void named_pipe::on_disconnect(on_connect_f handler) {
    size_t eparam;
    sys_on_named_pipe_connect_fp f;
    tools::function<on_connect_f>::free(this->c_object_.on_disconnect_eparam);
    std::tie(f, eparam) = tools::function<on_connect_f>::bind(handler);
    if (f) {
        this->c_object_.on_disconnect = f;
        this->c_object_.on_disconnect_eparam = eparam;
    }
}

}}
