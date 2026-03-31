#include <initp/platform/keyboard.h>
#include <initp/platform/console.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

struct termios sys_orig_termios;
sys_on_ctrlc_fp sys_on_ctrlc = NULL;

void sys_enable_async_keyboard(sys_on_ctrlc_fp on_ctrlc) {

    struct termios new_termios;
    sys_on_ctrlc = on_ctrlc;

    // Take two copies - one for now, one for later
    tcgetattr(0, &sys_orig_termios);
    memcpy(&new_termios, &sys_orig_termios, sizeof(new_termios));

    // Register cleanup handler, and set the new terminal mode
    atexit(&sys_disable_async_keyboard);
    //cfmakeraw(&new_termios);
    new_termios.c_lflag &= ~ICANON;
    new_termios.c_lflag &= ~ECHO;
    new_termios.c_lflag &= ~ISIG;
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_termios);
    // Disable output buffer
    sys_disable_console_buffer();
}

void sys_disable_async_keyboard(void) {
    tcsetattr(0, TCSANOW, &sys_orig_termios);
}

int sys_is_keyboard_hit(void) {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

char sys_keyboard_get_char(void) {
    char c;
    if (read(0, &c, sizeof(c)) < 0) {
        return (char)0;
    } else {
        if (c == '\e' && sys_on_ctrlc) {
            sys_on_ctrlc();
            return (char)0;
        }
        return c;
    }
}
