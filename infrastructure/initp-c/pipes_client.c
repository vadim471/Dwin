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
    if (!sys_open_named_pipe(&pipe, 1)) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "main", "Failed to open pipe");
        //return 0;
    }
    signal(SIGTERM, &on_signal);
    const size_t BUFFER_SIZE = 0x100;
    char data[BUFFER_SIZE];
    size_t bytes;
    sys_time_t time, last = sys_clock_ms();
    for (started = 1; started;) {
        bytes = sys_read_named_pipe(&pipe, data, BUFFER_SIZE);
        if (bytes > 0) {
            sys_debug_print(SYSTEM_LEVEL_TRACE, "main", "Received %d bytes", (int)bytes);
        }
        time = sys_clock_ms();
        if (last + 2000 < time) {
            last = time;
            sprintf(data, "Hello");
            bytes = sys_write_named_pipe(&pipe, data, 5);
            sys_debug_print(SYSTEM_LEVEL_TRACE, "main", "Sent %d bytes", (int)bytes);
        }
        sys_sleep_for(3);
    }
    sys_close_named_pipe(&pipe);
    return 0;
}
