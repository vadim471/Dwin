#include <cstdio>
#include <cstring>

extern "C" {

#include <itp/debug.h>
#include <itp/memory.h>

}

#ifdef ITP_DEBUG_CUSTOM
void itp_debug_print(uint8_t err, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    uint32_t size = strlen(fmt) + 2;
    char* format = (char*)itp_malloc(size + 1, "itp_debug_print:format");
    sprintf(format, "%s\r\n", fmt);
    vprintf(format, args);
    itp_free(format);
    va_end(args);
}
#endif // ITP_DEBUG_CUSTOM
