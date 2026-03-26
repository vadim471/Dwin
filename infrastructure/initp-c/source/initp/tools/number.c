#include <initp/tools/number.h>
#include <math.h>

int16_t tools_pow10_int16(uint8_t exponent) {
    int16_t result = 1;
    for (uint8_t i = 0; i < exponent; ++i)
        result *= 10;
    return result;
}

int32_t tools_pow10_int32(uint8_t exponent) {
    int32_t result = 1;
    for (uint8_t i = 0; i < exponent; ++i)
        result *= 10;
    return result;
}

int64_t tools_pow10_int64(uint8_t exponent) {
    int64_t result = 1;
    for (uint8_t i = 0; i < exponent; ++i)
        result *= 10;
    return result;
}

uint16_t tools_pow10_uint16(uint8_t exponent) {
    uint16_t result = 1;
    for (uint8_t i = 0; i < exponent; ++i)
        result *= 10;
    return result;
}

uint32_t tools_pow10_uint32(uint8_t exponent) {
    uint32_t result = 1;
    for (uint8_t i = 0; i < exponent; ++i)
        result *= 10;
    return result;
}

uint64_t tools_pow10_uint64(uint8_t exponent) {
    uint64_t result = 1;
    for (uint8_t i = 0; i < exponent; ++i)
        result *= 10;
    return result;
}

double tools_int16_to_double(int16_t number, uint8_t exponent) {
    double result = (double)number / (double)tools_pow10_int16(exponent);
    return result;
}

double tools_int32_to_double(int32_t number, uint8_t exponent) {
    double result = (double)number / (double)tools_pow10_int32(exponent);
    return result;
}

double tools_int64_to_double(int64_t number, uint8_t exponent) {
    int64_t divisor = tools_pow10_int64(exponent);
    double result = (double)(number / divisor);
    number %= divisor;
    result += (double)number / (double)divisor;
    return result;
}

double tools_uint16_to_double(uint16_t number, uint8_t exponent) {
    double result = (double)number / (double)tools_pow10_uint16(exponent);
    return result;
}

double tools_uint32_to_double(uint32_t number, uint8_t exponent) {
    double result = (double)number / (double)tools_pow10_uint32(exponent);
    return result;
}

double tools_uint64_to_double(uint64_t number, uint8_t exponent) {
    uint64_t divisor = tools_pow10_uint64(exponent);
    double result = (double)(number / divisor);
    number %= divisor;
    result += (double)number / (double)divisor;
    return result;
}

int16_t tools_double_to_int16(double value, uint8_t exponent, uint8_t round) {
    int16_t multiplier = tools_pow10_int16(exponent);
    int16_t result = (int16_t)value * multiplier;
    double i;
    value = modf(value, &i) * (double)multiplier;
    result += (int16_t)value;
    if (round == 1) {
        result++;
    } else if (round == 2) {
        value = modf(value, &i);
        if (value >= .499999999999) {
            result++;
        }
    }
    return result;
}

int32_t tools_double_to_int32(double value, uint8_t exponent, uint8_t round) {
    int32_t multiplier = tools_pow10_int32(exponent);
    int32_t result = (int32_t)value * multiplier;
    double i;
    value = modf(value, &i) * (double)multiplier;
    result += (int32_t)value;
    if (round == 1) {
        result++;
    } else if (round == 2) {
        value = modf(value, &i);
        if (value >= .499999999999) {
            result++;
        }
    }
    return result;
}

int64_t tools_double_to_int64(double value, uint8_t exponent, uint8_t round) {
    int64_t multiplier = tools_pow10_int64(exponent);
    int64_t result = (int16_t)value * multiplier;
    double i;
    value = modf(value, &i) * (double)multiplier;
    result += (int64_t)value;
    if (round == 1) {
        result++;
    } else if (round == 2) {
        value = modf(value, &i);
        if (value >= .499999999999) {
            result++;
        }
    }
    return result;
}

uint16_t tools_double_to_uint16(double value, uint8_t exponent, uint8_t round) {
    if (value <= .0 ) return 0;
    uint16_t multiplier = tools_pow10_uint16(exponent);
    uint16_t result = (uint16_t)value * multiplier;
    double i;
    value = modf(value, &i) * (double)multiplier;
    result += (uint16_t)value;
    if (round == 1) {
        result++;
    } else if (round == 2) {
        value = modf(value, &i);
        if (value >= .499999999999) {
            result++;
        }
    }
    return result;
}

uint32_t tools_double_to_uint32(double value, uint8_t exponent, uint8_t round) {
    if (value <= .0 ) return 0;
    uint32_t multiplier = tools_pow10_uint32(exponent);
    uint32_t result = (uint32_t)value * multiplier;
    double i;
    value = modf(value, &i) * (double)multiplier;
    result += (uint32_t)value;
    if (round == 1) {
        result++;
    } else if (round == 2) {
        value = modf(value, &i);
        if (value >= .499999999999) {
            result++;
        }
    }
    return result;
}

uint64_t tools_double_to_uint64(double value, uint8_t exponent, uint8_t round) {
    if (value <= .0 ) return 0;
    uint64_t multiplier = tools_pow10_uint64(exponent);
    uint64_t result = (uint64_t)value * multiplier;
    double i;
    value = modf(value, &i) * (double)multiplier;
    result += (uint64_t)value;
    if (round == 1) {
        result++;
    } else if (round == 2) {
        value = modf(value, &i);
        if (value >= .499999999999) {
            result++;
        }
    }
    return result;
}

int16_t tools_ipart_of_int16(int16_t number, uint8_t exponent) {
    int16_t result = number / tools_pow10_int16(exponent);
    return result;
}

int32_t tools_ipart_of_int32(int32_t number, uint8_t exponent) {
    int32_t result = number / tools_pow10_int32(exponent);
    return result;
}

int64_t tools_ipart_of_int64(int64_t number, uint8_t exponent) {
    int64_t result = number / tools_pow10_int64(exponent);
    return result;
}

uint16_t tools_ipart_of_uint16(uint16_t number, uint8_t exponent) {
    uint16_t result = number / tools_pow10_uint16(exponent);
    return result;
}

uint32_t tools_ipart_of_uint32(uint32_t number, uint8_t exponent) {
    uint32_t result = number / tools_pow10_uint32(exponent);
    return result;
}

uint64_t tools_ipart_of_uint64(uint64_t number, uint8_t exponent) {
    uint64_t result = number / tools_pow10_uint64(exponent);
    return result;
}

int16_t tools_fract_of_int16(int16_t number, uint8_t exponent) {
    if (number < 0) number = -number;
    int16_t divisor = tools_pow10_int16(exponent);
    int16_t result = number % divisor;
    return result;
}

int32_t tools_fract_of_int32(int32_t number, uint8_t exponent) {
    if (number < 0) number = -number;
    int32_t divisor = tools_pow10_int32(exponent);
    int32_t result = number % divisor;
    return result;
}

int64_t tools_fract_of_int64(int64_t number, uint8_t exponent) {
    if (number < 0) number = -number;
    int64_t divisor = tools_pow10_int64(exponent);
    int64_t result = number % divisor;
    return result;
}

uint16_t tools_fract_of_uint16(uint16_t number, uint8_t exponent) {
    uint16_t divisor = tools_pow10_uint16(exponent);
    uint16_t result = number % divisor;
    return result;
}

uint32_t tools_fract_of_uint32(uint32_t number, uint8_t exponent) {
    uint32_t divisor = tools_pow10_uint32(exponent);
    uint32_t result = number % divisor;
    return result;
}

uint64_t tools_fract_of_uint64(uint64_t number, uint8_t exponent) {
    uint64_t divisor = tools_pow10_uint64(exponent);
    uint64_t result = number % divisor;
    return result;
}

int16_t tools_ifract_to_int16(int16_t ipart, uint16_t fract, uint8_t exponent) {
    int16_t multiplier = tools_pow10_int16(exponent);
    int16_t result = ipart * multiplier;
    if (ipart > 0) {
        result += (int32_t)fract;
    } else result -= (int32_t)fract;
    return result;
}

int32_t tools_ifract_to_int32(int32_t ipart, uint16_t fract, uint8_t exponent) {
    int32_t multiplier = tools_pow10_int32(exponent);
    int32_t result = ipart * multiplier;
    if (ipart > 0) {
        result += (int32_t)fract;
    } else result -= (int32_t)fract;
    return result;
}

int64_t tools_ifract_to_int64(int64_t ipart, uint32_t fract, uint8_t exponent) {
    int64_t multiplier = tools_pow10_int64(exponent);
    int64_t result = ipart * multiplier;
    if (ipart > 0) {
        result += (int64_t)fract;
    } else result -= (int64_t)fract;
    return result;
}

uint16_t tools_ifract_to_uint16(uint16_t ipart, uint16_t fract, uint8_t exponent) {
    uint16_t multiplier = tools_pow10_uint16(exponent);
    uint16_t result = ipart * multiplier + fract;
    return result;
}

uint32_t tools_ifract_to_uint32(uint32_t ipart, uint16_t fract, uint8_t exponent) {
    uint32_t multiplier = tools_pow10_uint32(exponent);
    uint32_t result = ipart * multiplier + (uint32_t)fract;
    return result;
}

uint64_t tools_ifract_to_uint64(uint64_t ipart, uint32_t fract, uint8_t exponent) {
    uint64_t multiplier = tools_pow10_uint64(exponent);
    uint64_t result = ipart * multiplier + (uint64_t)fract;
    return result;
}
