#include <initp/platform/keyboard.h>

#include <windows.h>

DWORD fdwMode;
sys_on_ctrlc_fp sys_on_ctrlc;

void sys_enable_async_keyboard(sys_on_ctrlc_fp on_ctrlc) {
    sys_on_ctrlc = on_ctrlc;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdin, &fdwMode);
    SetConsoleMode(hStdin, ENABLE_WINDOW_INPUT);
}

void sys_disable_async_keyboard(void) {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(hStdin, fdwMode);
}

int sys_is_keyboard_hit(void) {
    DWORD cNumRead;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetNumberOfConsoleInputEvents(hStdin, &cNumRead);
    return (int)cNumRead;
}

char sys_keyboard_get_char(void) {
    DWORD cNumRead;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetNumberOfConsoleInputEvents(hStdin, &cNumRead);
    if (cNumRead > 0) {
        INPUT_RECORD input_record;
        ReadConsoleInput(hStdin, &input_record, 1, &cNumRead);
        if (cNumRead > 0) {
            if (input_record.EventType == KEY_EVENT) {
                if (input_record.Event.KeyEvent.bKeyDown) {
                    if ((input_record.Event.KeyEvent.wVirtualKeyCode == 0x43) &&
                        (input_record.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED)) {
                        if (sys_on_ctrlc) sys_on_ctrlc();
                    } else return (char)input_record.Event.KeyEvent.uChar.AsciiChar;
                }
            }
        }
    }
    return (char)0;
}
