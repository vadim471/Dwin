#include <cstdio>
#include <cstdarg>

extern "C" {

// Debug print function for ITP protocol
void itp_debug_print(unsigned char err, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    char message[512];
    vsnprintf(message, sizeof(message), fmt, args);
    
    if (err != 0) { // ITP_ERRC_TRACE = 0
        printf("[itp:%d] %s\n", (int)err, message);
    } else {
        printf("[itp] %s\n", message);
    }
    
    va_end(args);
}

// Debug print function for system
void sys_debug_print(char level, const char* what, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    char message[512];
    vsnprintf(message, sizeof(message), fmt, args);
    
    printf("[sys:%c][%s] %s\n", level, what, message);
    
    va_end(args);
}

} // extern "C"
