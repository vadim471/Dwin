#include <initp/helpers/manual_value.h>
#include <initp/helpers/time_filter.h>

#include <stdio.h>

int main(int argc, char** argv) {
    tools_manual_uint8_t uint8;
    tools_manual_init_uint8(&uint8, 0);
    uint8.value = 1;
    printf("%d\n", tools_manual_get_uint8(&uint8));
    uint8.manual = 2;
    printf("%d\n", tools_manual_get_uint8(&uint8));
    tools_time_filter_t filter[2];
    tools_time_filter_init(filter, 2, 1000);
    printf("%s\n", tools_time_filter_check(filter, 0, 1000) ? "true" : "false");
    printf("%s\n", tools_time_filter_check(filter, 0, 1500) ? "true" : "false");
    printf("%s\n", tools_time_filter_check(filter, 0, 2000) ? "true" : "false");
    return 0;
}
