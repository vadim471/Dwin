#include <initp/platform/console.h>

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void sys_clear_screen(void) {
    system("cls");
}

void sys_clear_line(void) {
    printf("%c[2K", 27);
}

void sys_set_cursor_position(int x, int y) {
    COORD Coord;
    Coord.X = (SHORT)x;
    Coord.Y = (SHORT)y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Coord);
}

void sys_disable_console_buffer(void) {}
