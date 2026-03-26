#include <initp/system/system.h>
#include <initp/system/debug.h>

void sys_echo(const char* message, const char* device) {
    if (device) {
        sys_debug_print(SYSTEM_LEVEL_INFO, "sys_echo", "[%s] %s", device, message);
    } else {
        sys_debug_print(SYSTEM_LEVEL_INFO, "sys_echo", "%s", message);
    }
}

void sys_led_on(const char* led) {
    sys_debug_print(SYSTEM_LEVEL_INFO, "sys_led_on", "LED %s switch on", led);
}

void sys_led_off(const char* led) {
    sys_debug_print(SYSTEM_LEVEL_INFO, "sys_led_off", "LED %s switch off", led);
}

void sys_led_blink(const char* led) {
    sys_debug_print(SYSTEM_LEVEL_INFO, "sys_led_blink", "LED %s blink", led);
}

void sys_sync(void) {
    sys_debug_print(SYSTEM_LEVEL_INFO, "sys_sync", "Disks synced");
}

void sys_change_mode(const char* path, const char* mode) {
    sys_debug_print(SYSTEM_LEVEL_INFO, "sys_change_mode", "Set file %s mode to %s", path, mode);
}

void sys_reboot(void) {
    sys_debug_print(SYSTEM_LEVEL_INFO, "sys_reboot", "SYSTEM REBOOT");
}

void sys_execute(const char* command) {
    sys_debug_print(SYSTEM_LEVEL_INFO, "sys_debug_print", "SYSTEM: %s", command);
}
