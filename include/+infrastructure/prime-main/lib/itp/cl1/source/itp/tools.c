#include <itp/tools.h>
#include <itp/enum.h>
#include <initp/tools/decimal.h>

#include <string.h>

uint8_t itp_cl1_tools_string_to_uint8(const char* string) {
    if (!string) return 0;
    uint16_t result = 0;
    for (uint8_t i = 0; i < 3; ++i) {
        if (!string[i]) break;
        if (string[i] < '0' || string[i] > '9')
            return 0;
        result *= 10;
        result += string[i] - '0';
    }
    if (result <= 0xFF) {
        return (uint8_t)result;
    } else return 0;
}

uint32_t itp_cl1_tools_string_to_uint32(const char* string) {
    if (!string) return 0;
    uint64_t result = 0;
    for (uint8_t i = 0; i < 10; ++i) {
        if (!string[i]) break;
        if (string[i] < '0' || string[i] > '9')
            return 0;
        result *= 10;
        result += string[i] - '0';
    }
    if (result <= 0xFFFFFFFF) {
        return (uint32_t)result;
    } else return 0;
}

uint8_t itp_cl1_tools_string_to_bool(const char* string) {
    if (!string) return 0;
    int length = strlen(string);
    if (length < 1 || length > 5) return 0;
    if (strcmp(string, "true") == 0 ||
        strcmp(string, "1") == 0) {
        return 1;
    } else return 0;
}

uint8_t itp_cl1_tools_has_bit(int64_t* pack, uint8_t index) {
    return (*pack >> (index * 2)) & 0x1;
}

uint8_t itp_cl1_tools_get_bit(int64_t* pack, uint8_t index) {
    return (*pack >> (index * 2 + 1)) & 0x1;
}

void itp_cl1_tools_set_bit(int64_t* pack, uint8_t index, uint8_t value) {
    int64_t ONE_BIT = 0x1;
    *pack |= (ONE_BIT << (index * 2));
    if (value) {
        *pack |= (ONE_BIT << (index * 2 + 1));
    } else {
        *pack &= ~(ONE_BIT << (index * 2 + 1));
    }
}
