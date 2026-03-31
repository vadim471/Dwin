#include <initp/system/time.h>

#include <windows.h>

sys_time_t sys_clock_ms(void) {
    return (sys_time_t)GetTickCount();
}

void sys_sleep_for(sys_time_t milliseconds) {
    Sleep((DWORD)milliseconds);
}
