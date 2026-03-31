#include <usd/io.h>
#include <usd/usd.h>
#include <usd/debug.h>
#include <usd/memory.h>

void usd_init_io(usd_io_tp io) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!io) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_init_io> I/O object is null");
        return USD_ERRC_NULL_POINTER;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    usd_init_device_group(&io->devices);
}

void usd_free_io(usd_io_tp io) {

    #ifdef USD_ENABLE_NULLPTR_CHECKS
    if (!sensor) {
        usd_debug_print(USD_ERRC_NULL_POINTER, "usd_free_io> I/O object is null");
        return;
    }
    #endif // USD_ENABLE_NULLPTR_CHECKS

    usd_clear_group(&io->devices);
    usd_free(io);
}
