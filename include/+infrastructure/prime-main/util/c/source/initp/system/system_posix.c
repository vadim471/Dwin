#include <initp/system/system.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

void sys_echo(const char* message, const char* device) {
    if (!message) return;
    size_t m_len = strlen(message);
    size_t d_len = (device) ? strlen(device) : 0;
    size_t total = m_len + d_len + 10;
    if (total > 0xFFFF) return;
    char* command = (char*)malloc((total + 1) * sizeof(char));
    if (!command) return;
    if (device) {
        sprintf(command, "echo \"%s\" > %s", message, device);
    } else sprintf(command, "echo \"%s\"", message);
    system(command);
    free(command);
}

void sys_led_on(const char* device) {
    if (!device) return;
    size_t len = strlen(device);
    if (len > 0xFF) return;
    size_t total = len + 40;
    char* command = (char*)malloc((total + 1) * sizeof(char));
    if (!command) return;
    sprintf(command, "echo none > /sys/class/leds/%s/trigger", device);
    system(command);
    sprintf(command, "echo 1 > /sys/class/leds/%s/brightness", device);
    system(command);
    free(command);
}

void sys_led_off(const char* device) {
    if (!device) return;
    size_t len = strlen(device);
    if (len > 0xFF) return;
    size_t total = len + 40;
    char* command = (char*)malloc((total + 1) * sizeof(char));
    if (!command) return;
    sprintf(command, "echo none > /sys/class/leds/%s/trigger", device);
    system(command);
    sprintf(command, "echo 0 > /sys/class/leds/%s/brightness", device);
    system(command);
    free(command);
}

void sys_led_blink(const char* device) {
    if (!device) return;
    size_t len = strlen(device);
    if (len > 0xFF) return;
    size_t total = len + 40;
    char* command = (char*)malloc((total + 1) * sizeof(char));
    if (!command) return;
    sprintf(command, "echo timer > /sys/class/leds/%s/trigger", device);
    system(command);
    free(command);
}

void sys_sync(void) {
    char command[] = "sync";
    system(command);
}

void sys_change_mode(const char* path, const char* mode) {
    if (!path || !mode) return;
    size_t p_len = strlen(path);
    size_t m_len = strlen(mode);
    size_t total = p_len + m_len + 7;
    if (total > 0xFFFF) return;
    char* command = (char*)malloc((total + 1) * sizeof(char));
    if (!command) return;
    sprintf(command, "chmod %s %s", mode, path);
    system(command);
    free(command);
}

void sys_reboot(void) {
    char command[] = "reboot";
    system(command);
}

void sys_execute(const char* command) {
    system(command);
}
