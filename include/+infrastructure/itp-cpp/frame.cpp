#include <itp/frame.hpp>

int main(int argc, char** argv) {

    itp::frame::uptr frame;

    for (size_t i = 0; i < 1000; ++i) {
        frame.reset(new itp::frame(ITP_CMD_SET_ADDRESS));
        frame->dump();
        frame->from(1);
        frame->to(2);
        frame->write_value<uint8_t>(9);
        frame->dump();
    }

    return 0;
}
