#include <initp/system/time.h>

#include <sys/time.h>

// Define _BSD_SOURCE for usleep

#if _POSIX_C_SOURCE >= 199309L

#include <time.h>

void sys_sleep_for(sys_time_t milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000LL;
    ts.tv_nsec = (milliseconds % 1000LL) * 1000000LL;
    nanosleep(&ts, NULL);
}

#else // _POSIX_C_SOURCE

#include <unistd.h>

void sys_sleep_for(sys_time_t milliseconds) {
    usleep(milliseconds * 1000LL);
}

#endif // _POSIX_C_SOURCE

sys_time_t sys_clock_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    sys_time_t now = (sys_time_t)(tv.tv_sec * 1000LL + tv.tv_usec / 1000);
    return now;
}
