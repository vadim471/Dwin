#include <initp/platform/named_pipe.h>
#include <initp/system/time.h>
#include <initp/system/debug.h>

#include <stdint.h>
#include <stddef.h>
#include <signal.h>

uint8_t started = 0;

void on_signal(int signal) {
    started = 0;
}

int main(int argc, char** argv) {
    sys_named_pipe_t pipe;
    sys_init_named_pipe(&pipe, "mynamedpipe");
    if (!sys_create_named_pipe(&pipe)) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "main", "Failed to create pipe");
        return 0;
    }
    if (!sys_open_named_pipe(&pipe, 1)) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "main", "Failed to open pipe");
        //return 0;
    }
    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);
    const size_t BUFFER_SIZE = 0x100;
    char data[BUFFER_SIZE];
    size_t bytes;
    for (started = 1; started;) {
        bytes = sys_read_named_pipe(&pipe, data, BUFFER_SIZE);
        if (bytes > 0) {
            sys_debug_print(SYSTEM_LEVEL_TRACE, "main", "Received %d bytes", (int)bytes);
            bytes = sys_write_named_pipe(&pipe, data, bytes);
            sys_debug_print(SYSTEM_LEVEL_TRACE, "main", "Sent %d bytes", (int)bytes);
        }
        sys_sleep_for(100);
    }
    sys_close_named_pipe(&pipe);
    sys_delete_named_pipe(&pipe);
    return 0;
}
