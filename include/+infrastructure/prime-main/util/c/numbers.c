#include <initp/tools/number.h>
#include <initp/tools/decimal.h>
#include <stdio.h>
#include <inttypes.h>

int main(int argc, char** argv) {
    printf("10 pow 0: %d\r\n", tools_pow10_int16(0));
    printf("10 pow 1: %d\r\n", tools_pow10_int32(1));
    printf("10 pow 3: %d\r\n", tools_pow10_uint32(3));
    printf("14234001 by 2: %.2lf\r\n", tools_int32_to_double(14234001, 2));
    printf("-14234001 by 2: %.2lf\r\n", tools_int32_to_double(-14234001, 2));
    printf("14234001 by 3: %.3lf\r\n", tools_uint64_to_double(14234001, 3));
    printf("111111.222 by 3: %d\r\n", tools_double_to_uint32(111111.222d, 3, 0));
    printf("-111111.222 by 3: %d\r\n", tools_double_to_int32(-111111.222d, 3, 0));
    printf("3333111111.22222 by 5: %" PRIu64 "\r\n", tools_double_to_uint64(3333111111.22222d, 5, 0));
    printf(
        "1111222 3 -> %d %03d / %d %02d\r\n",
        tools_ipart_of_uint32(1111222, 3),
        tools_fract_of_uint32(1111222, 3),
        tools_ipart_of_uint32(1111222, 3),
        tools_fract_of_uint32(1111222, 3)
    );
    printf(
        "111122 2 -> %d %03d / %d %02d\r\n",
        tools_ipart_of_uint32(111122, 2),
        tools_fract_of_uint32(111122, 2),
        tools_ipart_of_uint32(111122, 2),
        tools_fract_of_uint32(111122, 2)
    );
    printf(
        "1111022 3 -> %d %03d / %d %02d\r\n",
        tools_ipart_of_uint32(1111022, 3),
        tools_fract_of_uint32(1111022, 3),
        tools_ipart_of_uint32(1111022, 3),
        tools_fract_of_uint32(1111022, 3)
    );
    printf(
        "111102 2 -> %d %03d / %d %02d\r\n",
        tools_ipart_of_uint32(111102, 2),
        tools_fract_of_uint32(111102, 2),
        tools_ipart_of_uint32(111102, 2),
        tools_fract_of_uint32(111102, 2)
    );
    printf(
        "* -> %d\r\n",
        tools_ifract_to_uint32(
            tools_ipart_of_uint32(111102, 2),
            tools_fract_of_uint32(111102, 2),
            3
        )
    );
    printf(
        "-111102 2 -> %d %03d / %d %02d\r\n",
        tools_ipart_of_int32(-111102, 2),
        tools_fract_of_int32(-111102, 2),
        tools_ipart_of_int32(-111102, 2),
        tools_fract_of_int32(-111102, 2)
    );
    printf(
        "* -> %d\r\n",
        tools_ifract_to_int32(
            tools_ipart_of_int32(-111102, 2),
            tools_fract_of_int32(-111102, 2),
            3
        )
    );
    printf(
        "floor: %.2lf -> %.1lf\r\n",
        tools_uint32_to_double(1149, 2),
        tools_uint32_to_double(tools_round_uint32(1149, 2, 1, 0), 1)
    );
    printf(
        "floor: %.2lf -> %.1lf\r\n",
        tools_uint32_to_double(1140, 2),
        tools_uint32_to_double(tools_round_uint32(1140, 2, 1, 0), 1)
    );
    printf(
        " ceil: %.2lf -> %.1lf\r\n",
        tools_uint32_to_double(1149, 2),
        tools_uint32_to_double(tools_round_uint32(1149, 2, 1, 1), 1)
    );
    printf(
        " ceil: %.2lf -> %.1lf\r\n",
        tools_uint32_to_double(1140, 2),
        tools_uint32_to_double(tools_round_uint32(1140, 2, 1, 1), 1)
    );
    printf(
        " ceil: %.2lf -> %.1lf\r\n",
        tools_uint32_to_double(1141, 2),
        tools_uint32_to_double(tools_round_uint32(1141, 2, 1, 1), 1)
    );
    printf(
        " math: %.2lf -> %.1lf\r\n",
        tools_uint32_to_double(1144, 2),
        tools_uint32_to_double(tools_round_uint32(1144, 2, 1, 2), 1)
    );
    printf(
        " math: %.2lf -> %.1lf\r\n",
        tools_uint32_to_double(1145, 2),
        tools_uint32_to_double(tools_round_uint32(1145, 2, 1, 2), 1)
    );
    printf(
        " math: %.2lf -> %.1lf\r\n",
        tools_uint32_to_double(1140, 2),
        tools_uint32_to_double(tools_round_uint32(1140, 2, 1, 2), 1)
    );
    return 0;
}
