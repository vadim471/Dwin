#include <initp/platform/console.h>

#include <stdio.h>
#include <stdlib.h>

void sys_clear_screen(void) {
    system("clear");
}

void sys_clear_line(void) {
    printf("%c[2K", 27);
}

void sys_set_cursor_position(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}

void sys_disable_console_buffer(void) {
    setvbuf(stdout, (char*)NULL, _IONBF, 0);
}
