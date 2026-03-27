#include <initp/helpers/time_filter.h>

#include <string.h>

void tools_time_filter_init(tools_time_filter_t* array, size_t size, uint32_t delay) {
    for (size_t i = 0; i < size; ++i) {
        array[i].time = 0;
        array[i].delay = delay;
    }
}

uint8_t tools_time_filter_check(tools_time_filter_t* array, size_t index, uint64_t time) {
    if (array[index].time + array[index].delay <= time) {
        array[index].time = time;
        return 1;
    }
    return 0;
}
