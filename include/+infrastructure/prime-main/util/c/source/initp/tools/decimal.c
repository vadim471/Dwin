#include <initp/tools/decimal.h>
#include <initp/tools/number.h>

#include <stdio.h>

int16_t tools_round_int16(int16_t number, uint8_t source, uint8_t target, uint8_t round) {
    int16_t result = number;
    if (source > target) {
        int16_t t1 = 1;
        while (source > target) {
            result /= 10;
            t1 *= 10U;
            source--;
        }
        int16_t t2 = number - result * t1;
        if (round == 1) {
            if (t2 > 0) result++;
        } else if (round == 2) {
            if (t2 >= t1 / 2) {
                result++;
            }
        }
    } else if (source < target) {
        while (source < target) {
            result *= 10;
            source++;
        }
    }
    return result;
}

int32_t tools_round_int32(int32_t number, uint8_t source, uint8_t target, uint8_t round) {
    int32_t result = number;
    if (source > target) {
        int32_t t1 = 1;
        while (source > target) {
            result /= 10;
            t1 *= 10U;
            source--;
        }
        int32_t t2 = number - result * t1;
        if (round == 1) {
            if (t2 > 0) result++;
        } else if (round == 2) {
            if (t2 >= t1 / 2) {
                result++;
            }
        }
    } else if (source < target) {
        while (source < target) {
            result *= 10;
            source++;
        }
    }
    return result;
}

int64_t tools_round_int64(int64_t number, uint8_t source, uint8_t target, uint8_t round) {
    int64_t result = number;
    if (source > target) {
        int64_t t1 = 1;
        while (source > target) {
            result /= 10L;
            t1 *= 10U;
            source--;
        }
        int64_t t2 = number - result * t1;
        if (round == 1) {
            if (t2 > 0) result++;
        } else if (round == 2) {
            if (t2 >= t1 / 2) {
                result++;
            }
        }
    } else if (source < target) {
        while (source < target) {
            result *= 10L;
            source++;
        }
    }
    return result;
}

uint16_t tools_round_uint16(uint16_t number, uint8_t source, uint8_t target, uint8_t round) {
    uint16_t result = number;
    if (source > target) {
        uint16_t t1 = 1;
        while (source > target) {
            result /= 10U;
            t1 *= 10U;
            source--;
        }
        uint16_t t2 = number - result * t1;
        if (round == 1) {
            if (t2 > 0) result++;
        } else if (round == 2) {
            if (t2 >= t1 / 2) {
                result++;
            }
        }
    } else if (source < target) {
        while (source < target) {
            result *= 10U;
            source++;
        }
    }
    return result;
}

uint32_t tools_round_uint32(uint32_t number, uint8_t source, uint8_t target, uint8_t round) {
    uint32_t result = number;
    if (source > target) {
        uint32_t t1 = 1;
        while (source > target) {
            result /= 10U;
            t1 *= 10U;
            source--;
        }
        uint32_t t2 = number - result * t1;
        if (round == 1) {
            if (t2 > 0) result++;
        } else if (round == 2) {
            if (t2 >= t1 / 2) {
                result++;
            }
        }
    } else if (source < target) {
        while (source < target) {
            result *= 10U;
            source++;
        }
    }
    return result;
}

uint64_t tools_round_uint64(uint64_t number, uint8_t source, uint8_t target, uint8_t round) {
    uint64_t result = number;
    if (source > target) {
        uint64_t t1 = 1;
        while (source > target) {
            result /= 10UL;
            t1 *= 10U;
            source--;
        }
        uint64_t t2 = number - result * t1;
        if (round == 1) {
            if (t2 > 0) result++;
        } else if (round == 2) {
            if (t2 >= t1 / 2) {
                result++;
            }
        }
    } else if (source < target) {
        while (source < target) {
            result *= 10UL;
            source++;
        }
    }
    return result;
}

uint16_t tools_uint16_addition(uint16_t a, uint8_t ae, uint16_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (ae > be) {
        b = tools_round_uint16(b, be, ae, 0);
        return tools_round_uint16(a + b, ae, e, round);
    } else if (ae < be) {
        a = tools_round_uint16(a, ae, be, 0);
        return tools_round_uint16(a + b, be, e, round);
    } else if (ae != e) {
        return tools_round_uint16(a + b, ae, e, round);
    } else return (a + b);
}

uint32_t tools_uint32_addition(uint32_t a, uint8_t ae, uint32_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (ae > be) {
        b = tools_round_uint32(b, be, ae, 0);
        return tools_round_uint32(a + b, ae, e, round);
    } else if (ae < be) {
        a = tools_round_uint32(a, ae, be, 0);
        return tools_round_uint32(a + b, be, e, round);
    } else if (ae != e) {
        return tools_round_uint32(a + b, ae, e, round);
    } else return (a + b);
}

uint64_t tools_uint64_addition(uint64_t a, uint8_t ae, uint64_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (ae > be) {
        b = tools_round_uint64(b, be, ae, 0);
        return tools_round_uint64(a + b, ae, e, round);
    } else if (ae < be) {
        a = tools_round_uint64(a, ae, be, 0);
        return tools_round_uint64(a + b, be, e, round);
    } else if (ae != e) {
        return tools_round_uint64(a + b, ae, e, round);
    } else return (a + b);
}

int16_t tools_int16_addition(int16_t a, uint8_t ae, int16_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (ae > be) {
        b = tools_round_int16(b, be, ae, 0);
        return tools_round_int16(a + b, ae, e, round);
    } else if (ae < be) {
        a = tools_round_int16(a, ae, be, 0);
        return tools_round_int16(a + b, be, e, round);
    } else if (ae != e) {
        return tools_round_int16(a + b, ae, e, round);
    } else return (a + b);
}

int32_t tools_int32_addition(int32_t a, uint8_t ae, int32_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (ae > be) {
        b = tools_round_int32(b, be, ae, 0);
        return tools_round_int32(a + b, ae, e, round);
    } else if (ae < be) {
        a = tools_round_int32(a, ae, be, 0);
        return tools_round_int32(a + b, be, e, round);
    } else if (ae != e) {
        return tools_round_int32(a + b, ae, e, round);
    } else return (a + b);
}

int64_t tools_int64_addition(int64_t a, uint8_t ae, int64_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (ae > be) {
        b = tools_round_int64(b, be, ae, 0);
        return tools_round_int64(a + b, ae, e, round);
    } else if (ae < be) {
        a = tools_round_int64(a, ae, be, 0);
        return tools_round_int64(a + b, be, e, round);
    } else if (ae != e) {
        return tools_round_int64(a + b, ae, e, round);
    } else return (a + b);
}

uint16_t tools_uint16_subtraction(uint16_t a, uint8_t ae, uint16_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (ae > be) {
        b = tools_round_uint16(b, be, ae, 0);
        if (a > b) {
            return tools_round_uint16(a - b, ae, e, round);
        } else return 0U;
    } else if (ae < be) {
        a = tools_round_uint16(a, ae, be, 0);
        if (a > b) {
            return tools_round_uint16(a - b, be, e, round);
        } else return 0U;
    } else if (ae != e) {
        if (a > b) {
            return tools_round_uint16(a - b, ae, e, round);
        } else return 0U;
    } else return (a > b) ? (a - b) : 0U;
}

uint32_t tools_uint32_subtraction(uint32_t a, uint8_t ae, uint32_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (ae > be) {
        b = tools_round_uint32(b, be, ae, 0);
        if (a > b) {
            return tools_round_uint32(a - b, ae, e, round);
        } else return 0U;
    } else if (ae < be) {
        a = tools_round_uint32(a, ae, be, 0);
        if (a > b) {
            return tools_round_uint32(a - b, be, e, round);
        } else return 0U;
    } else if (ae != e) {
        if (a > b) {
            return tools_round_uint32(a - b, ae, e, round);
        } else return 0U;
    } else return (a > b) ? (a - b) : 0U;
}

uint64_t tools_uint64_subtraction(uint64_t a, uint8_t ae, uint64_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (ae > be) {
        b = tools_round_uint64(b, be, ae, 0);
        if (a > b) {
            return tools_round_uint64(a - b, ae, e, round);
        } else return 0U;
    } else if (ae < be) {
        a = tools_round_uint64(a, ae, be, 0);
        if (a > b) {
            return tools_round_uint64(a - b, be, e, round);
        } else return 0U;
    } else if (ae != e) {
        if (a > b) {
            return tools_round_uint64(a - b, ae, e, round);
        } else return 0U;
    } else return (a > b) ? (a - b) : 0U;
}

int16_t tools_int16_subtraction(int16_t a, uint8_t ae, int16_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (ae > be) {
        b = tools_round_int16(b, be, ae, 0);
        return tools_round_int16(a - b, ae, e, round);
    } else if (ae < be) {
        a = tools_round_int16(a, ae, be, 0);
        return tools_round_int16(a - b, be, e, round);
    } else if (ae != e) {
        return tools_round_int16(a - b, ae, e, round);
    } else return (a - b);
}

int32_t tools_int32_subtraction(int32_t a, uint8_t ae, int32_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (ae > be) {
        b = tools_round_int32(b, be, ae, 0);
        return tools_round_int32(a - b, ae, e, round);
    } else if (ae < be) {
        a = tools_round_int32(a, ae, be, 0);
        return tools_round_int32(a - b, be, e, round);
    } else if (ae != e) {
        return tools_round_int32(a - b, ae, e, round);
    } else return (a - b);
}

int64_t tools_int64_subtraction(int64_t a, uint8_t ae, int64_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (ae > be) {
        b = tools_round_int64(b, be, ae, 0);
        return tools_round_int64(a - b, ae, e, round);
    } else if (ae < be) {
        a = tools_round_int64(a, ae, be, 0);
        return tools_round_int64(a - b, be, e, round);
    } else if (ae != e) {
        return tools_round_int64(a - b, ae, e, round);
    } else return (a - b);
}

uint16_t tools_uint16_multiplication(uint16_t a, uint8_t ae, uint16_t b, uint8_t be, uint8_t e, uint8_t round) {
    return tools_round_uint16(a * b, ae + be, e, round);
}

uint32_t tools_uint32_multiplication(uint32_t a, uint8_t ae, uint32_t b, uint8_t be, uint8_t e, uint8_t round) {
    return tools_round_uint32(a * b, ae + be, e, round);
}

uint64_t tools_uint64_multiplication(uint64_t a, uint8_t ae, uint64_t b, uint8_t be, uint8_t e, uint8_t round) {
    return tools_round_uint64(a * b, ae + be, e, round);
}

int16_t tools_int16_multiplication(int16_t a, uint8_t ae, int16_t b, uint8_t be, uint8_t e, uint8_t round) {
    return tools_round_int16(a * b, ae + be, e, round);
}

int32_t tools_int32_multiplication(int32_t a, uint8_t ae, int32_t b, uint8_t be, uint8_t e, uint8_t round) {
    return tools_round_int32(a * b, ae + be, e, round);
}

int64_t tools_int64_multiplication(int64_t a, uint8_t ae, int64_t b, uint8_t be, uint8_t e, uint8_t round) {
    return tools_round_int64(a * b, ae + be, e, round);
}

uint16_t tools_uint16_division(uint16_t a, uint8_t ae, uint16_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (!b) return 0;
    if (ae > be) {
        b = tools_round_uint16(b, be, ae, 0);
    } else if (ae < be) {
        a = tools_round_uint16(a, ae, be, 0);
    }
    return tools_double_to_uint16((double)a / (double)b, e, round);
}

uint32_t tools_uint32_division(uint32_t a, uint8_t ae, uint32_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (!b) return 0;
    if (ae > be) {
        b = tools_round_uint32(b, be, ae, 0);
    } else if (ae < be) {
        a = tools_round_uint32(a, ae, be, 0);
    }
    return tools_double_to_uint32((double)a / (double)b, e, round);
}

uint64_t tools_uint64_division(uint64_t a, uint8_t ae, uint64_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (!b) return 0;
    if (ae > be) {
        b = tools_round_uint64(b, be, ae, 0);
    } else if (ae < be) {
        a = tools_round_uint64(a, ae, be, 0);
    }
    return tools_double_to_uint64((double)a / (double)b, e, round);
}

int16_t tools_int16_division(int16_t a, uint8_t ae, int16_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (!b) return 0;
    if (ae > be) {
        b = tools_round_int16(b, be, ae, 0);
    } else if (ae < be) {
        a = tools_round_int16(a, ae, be, 0);
    }
    return tools_double_to_int16((double)a / (double)b, e, round);
}

int32_t tools_int32_division(int32_t a, uint8_t ae, int32_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (!b) return 0;
    if (ae > be) {
        b = tools_round_int32(b, be, ae, 0);
    } else if (ae < be) {
        a = tools_round_int32(a, ae, be, 0);
    }
    return tools_double_to_int32((double)a / (double)b, e, round);
}

int64_t tools_int64_division(int64_t a, uint8_t ae, int64_t b, uint8_t be, uint8_t e, uint8_t round) {
    if (!b) return 0;
    if (ae > be) {
        b = tools_round_int64(b, be, ae, 0);
    } else if (ae < be) {
        a = tools_round_int64(a, ae, be, 0);
    }
    return tools_double_to_int64((double)a / (double)b, e, round);
}
