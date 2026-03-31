#include <itp/tools.h>
#include <initp/tools/number.h>
#include <stdio.h>

int main(int argc, char** argv) {

    uint64_t pack = 0;
    itp_cl1_tools_set_bit(&pack, 0, 0);
    itp_cl1_tools_set_bit(&pack, 2, 1);
    printf("has 0: %s\n", itp_cl1_tools_has_bit(&pack, 0) ? "true" : "false");
    printf("has 1: %s\n", itp_cl1_tools_has_bit(&pack, 1) ? "true" : "false");
    printf("has 2: %s\n", itp_cl1_tools_has_bit(&pack, 2) ? "true" : "false");
    printf("get 0: %s\n", itp_cl1_tools_get_bit(&pack, 0) ? "true" : "false");
    printf("get 2: %s\n", itp_cl1_tools_get_bit(&pack, 2) ? "true" : "false");

    /*const uint32_t PRICE = 4110;
    const uint32_t AMOUNT = 10100;
    const uint32_t VOLUME = 2433;

    uint32_t amount, volume;

    amount = AMOUNT;
    printf("\r\n  for amount: %6.2lf\r\n", tools_uint32_to_double(amount, 2));
    volume = itp_cl1_tools_round_volume(PRICE, 2, amount, 2, 3, CL1_ROUND_FLOOR);
    printf("floor volume: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));
    volume = itp_cl1_tools_round_volume(PRICE, 2, amount, 2, 3, CL1_ROUND_CEIL);
    printf(" ceil volume: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));
    volume = itp_cl1_tools_round_volume(PRICE, 2, amount, 2, 3, CL1_ROUND_NEAREST);
    printf(" math volume: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));

    volume = VOLUME;
    printf("\r\n  for volume: %.3lf\r\n", tools_uint32_to_double(volume, 3));
    amount = itp_cl1_tools_round_amount(PRICE, 2, volume, 3, 2, CL1_ROUND_FLOOR);
    printf("floor amount: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));
    amount = itp_cl1_tools_round_amount(PRICE, 2, volume, 3, 2, CL1_ROUND_CEIL);
    printf(" ceil amount: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));
    amount = itp_cl1_tools_round_amount(PRICE, 2, volume, 3, 2, CL1_ROUND_NEAREST);
    printf(" math amount: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));

    amount = AMOUNT;
    printf("\r\nfloor amount: %6.2lf\r\n", tools_uint32_to_double(amount, 2));
    amount = itp_cl1_tools_round_order(PRICE, 2, amount, 2, 2, CL1_ROUND_FLOOR, &volume, 3, CL1_ROUND_FLOOR);
    printf("floor volume: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));
    amount = AMOUNT;
    amount = itp_cl1_tools_round_order(PRICE, 2, amount, 2, 2, CL1_ROUND_FLOOR, &volume, 3, CL1_ROUND_CEIL);
    printf(" ceil volume: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));
    amount = AMOUNT;
    amount = itp_cl1_tools_round_order(PRICE, 2, amount, 2, 2, CL1_ROUND_FLOOR, &volume, 3, CL1_ROUND_NEAREST);
    printf(" math volume: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));

    amount = AMOUNT;
    printf("\r\n ceil amount: %6.2lf\r\n", tools_uint32_to_double(amount, 2));
    amount = itp_cl1_tools_round_order(PRICE, 2, amount, 2, 2, CL1_ROUND_CEIL, &volume, 3, CL1_ROUND_FLOOR);
    printf("floor volume: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));
    amount = AMOUNT;
    amount = itp_cl1_tools_round_order(PRICE, 2, amount, 2, 2, CL1_ROUND_CEIL, &volume, 3, CL1_ROUND_CEIL);
    printf(" ceil volume: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));
    amount = AMOUNT;
    amount = itp_cl1_tools_round_order(PRICE, 2, amount, 2, 2, CL1_ROUND_CEIL, &volume, 3, CL1_ROUND_NEAREST);
    printf(" math volume: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));

    amount = AMOUNT;
    printf("\r\n math amount: %6.2lf\r\n", tools_uint32_to_double(amount, 2));
    amount = itp_cl1_tools_round_order(PRICE, 2, amount, 2, 2, CL1_ROUND_NEAREST, &volume, 3, CL1_ROUND_FLOOR);
    printf("floor volume: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));
    amount = AMOUNT;
    amount = itp_cl1_tools_round_order(PRICE, 2, amount, 2, 2, CL1_ROUND_NEAREST, &volume, 3, CL1_ROUND_CEIL);
    printf(" ceil volume: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));
    amount = AMOUNT;
    amount = itp_cl1_tools_round_order(PRICE, 2, amount, 2, 2, CL1_ROUND_NEAREST, &volume, 3, CL1_ROUND_NEAREST);
    printf(" math volume: %6.2lf, %.3lf\r\n", tools_uint32_to_double(amount, 2), tools_uint32_to_double(volume, 3));*/
    return 0;
}
