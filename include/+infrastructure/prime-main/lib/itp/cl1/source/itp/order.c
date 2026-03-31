#include <itp/order.h>
#include <itp/tools.h>
#include <itp/error.h>
#include <itp/debug.h>
#include <itp/memory.h>

#include <initp/tools/decimal.h>

#include <string.h>

void itp_cl1_init_order_info(itp_cl1_order_info_tp object) {
    if (!object) return;
    memset(object, 0x0, sizeof(itp_cl1_order_info_t));
}

void itp_cl1_init_order_data(itp_cl1_order_data_tp object) {
    if (!object) return;
    memset(object, 0x0, sizeof(itp_cl1_order_data_t));
}

void itp_cl1_init_round_info(itp_cl1_round_info_tp object) {
    if (!object) return;
    memset(object, 0x0, sizeof(itp_cl1_round_info_t));
}

void itp_cl1_init_decimal_info(itp_cl1_decimal_info_tp object) {
    if (!object) return;
    memset(object, 0x0, sizeof(itp_cl1_decimal_info_t));
}

void itp_cl1_init_dispenser(itp_cl1_dispenser_tp object, uint8_t address) {
    if (!object) return;
    object->address = address;
    object->next = NULL;
    itp_cl1_init_order_info(&object->order);
}

void itp_cl1_free_dispenser(itp_cl1_dispenser_tp object) {
    if (!object) return;
    itp_free(object);
}

void itp_cl1_init_dispenser_list(itp_cl1_dispenser_list_tp object) {
    if (!object) return;
    object->first = NULL;
}

itp_cl1_dispenser_tp itp_cl1_create_dispenser(uint8_t address) {
    itp_cl1_dispenser_tp object = (itp_cl1_dispenser_tp)itp_malloc(sizeof(itp_cl1_dispenser_t), "itp_cl1_create_dispenser:object");
    if (object) itp_cl1_init_dispenser(object, address);
    return object;
}

itp_cl1_dispenser_tp itp_cl1_add_dispenser(itp_cl1_dispenser_list_tp list, uint8_t address) {
    if (!list || !address) return NULL;
    if (itp_cl1_find_dispenser(list, address)) return NULL;

    itp_cl1_dispenser_tp object = list->first;
    while (object) {
        if (object->next) {
            object = object->next;
        } else {
            object->next = itp_cl1_create_dispenser(address);
            return object->next;
        }
    }

    list->first = itp_cl1_create_dispenser(address);
    return list->first;
}

itp_cl1_dispenser_tp itp_cl1_find_dispenser(itp_cl1_dispenser_list_tp list, uint8_t address) {
    if (!list || !address) return NULL;
    itp_cl1_dispenser_tp object = list->first;
    while (object) {
        if (object->address == address)
            return object;
        object = object->next;
    }
    return NULL;
}

void itp_cl1_dispose_dispenser_list(itp_cl1_dispenser_list_tp list) {
    if (!list) return;
    itp_cl1_dispenser_tp next, object = list->first;
    while (object) {
        next = object->next;
        itp_cl1_free_dispenser(object);
        object = next;
    }
}

uint32_t itp_cl1_round_amount(uint32_t price, uint8_t price_pow, uint32_t volume, uint8_t volume_pow, uint8_t amount_pow, uint8_t amount_precision, uint8_t round) {
    uint64_t a = (uint64_t)price * (uint64_t)volume;
    uint32_t amount = (uint32_t)tools_round_uint64(a, volume_pow + price_pow, amount_precision, round);
    if (amount_pow != amount_precision) {
        return tools_round_uint32(amount, amount_precision, amount_pow, round);
    } else return amount;
}

uint32_t itp_cl1_round_volume(uint32_t price, uint8_t price_pow, uint32_t amount, uint8_t amount_pow, uint8_t volume_pow, uint8_t volume_precision, uint8_t round) {

    // Приводим к максимальной степени перед вычислениями
    if (price_pow > amount_pow) {
        amount = tools_round_uint32(amount, amount_pow, price_pow, 0);
        amount_pow = price_pow;
    } else if (price_pow < amount_pow) {
        price = tools_round_uint32(price, price_pow, amount_pow, 0);
        price_pow = amount_pow;
    }

    // Вычисляем целое значение объёма
    uint64_t a = tools_round_uint64((uint64_t)amount, amount_pow, amount_pow + volume_precision, 0);
    uint64_t p = (uint64_t)price;
    uint32_t v = (uint32_t)(a / p);

    // Вычисляем результат в зависимости от способа округления
    switch (round) {
        case CL1_ROUND_CEIL:
            if (a % p) v++;
            break;
        case CL1_ROUND_NEAREST:
            if (a % p >= p / 2) v++;
            break;
        case CL1_ROUND_FLOOR:
        default:
            break;
    }

    // Округляем, если требуется
    if (volume_pow != volume_precision) {
        return tools_round_uint32(v, volume_precision, volume_pow, round);
    } else return v;
}

uint16_t itp_cl1_round_order(itp_cl1_order_data_tp data, itp_cl1_round_info_tp round, itp_cl1_decimal_info_tp decimal, itp_cl1_order_info_tp order) {

    // Check parameters
    if (!data || !round || !decimal || !order) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_cl1_round_order> Null pointer found");
        return ITP_ERR_INTERNAL;
    }
    if (data->type != CL1_FUELING_TYPE_AMOUNT &&
        data->type != CL1_FUELING_TYPE_VOLUME &&
        data->type != CL1_FUELING_TYPE_FULL) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_cl1_round_order> Unknown fuelling type %d", (int)data->type);
        return CL1_ERR_DSP_WRONG_FUELLING_TYPE;
    }

    // Check round types
    if (round->amount_round > 2) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_cl1_round_order> Unknown amount round type %d", (int)round->amount_round);
        round->amount_round = 0;
    }
    if (round->volume_round > 2) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_cl1_round_order> Unknown volume round type %d", (int)round->volume_round);
        round->volume_round = 0;
    }
    if (round->amount_order > CL1_AUTHORIZE_VOLUME) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_cl1_round_order> Unknown amount order type %d", (int)round->amount_order);
        round->amount_order = CL1_AUTHORIZE_AS_IS;
    }

    // Temporary variables
    uint32_t price = data->price;
    uint8_t price_pow = data->price_pow;
    uint32_t value = data->value;
    uint8_t value_pow = data->value_pow;

    // Round price
    if (price_pow != decimal->price_decimal) {
        price = tools_round_uint32(price, price_pow, decimal->price_decimal, round->amount_round);
        price_pow = decimal->price_decimal;
    }
    order->round_to_order = data->round_to_order;

    // Round amount order
    if (data->type == CL1_FUELING_TYPE_AMOUNT) {

        // Temporary variables
        uint32_t amount = value;
        uint8_t amount_pow = value_pow;
        uint32_t volume = 0;
        uint8_t volume_pow = decimal->volume_decimal;

        // Set order as is
        if (round->amount_order == CL1_AUTHORIZE_AS_IS) {

            // Round amount
            if (round->amount_precision > 0 &&
                amount_pow != round->amount_precision) {
                amount = tools_round_uint32(amount, amount_pow, round->amount_precision, round->amount_round);
                amount_pow = round->amount_precision;
            }
            if (amount_pow != decimal->amount_decimal) {
                amount = tools_round_uint32(amount, amount_pow, decimal->amount_decimal, round->amount_round);
                amount_pow = decimal->amount_decimal;
            }

            // Round volume
            uint8_t volume_precision = (round->volume_precision > 0) ? round->volume_precision : volume_pow;
            volume = itp_cl1_round_volume(price, price_pow, amount, amount_pow, volume_pow, volume_precision, round->volume_round);

            // User order
            if (order->round_to_order) {
                order->price = data->price;
                order->price_pow = data->price_pow;
                order->amount = data->value;
                order->amount_pow = data->value_pow;
                order->volume = volume;
                order->volume_pow = volume_pow;
            } else {
                order->price = price;
                order->price_pow = price_pow;
                order->amount = amount;
                order->amount_pow = amount_pow;
                order->volume = volume;
                order->volume_pow = volume_pow;
            }

            // Dispenser order
            if (decimal->price_decimal != data->price_pow) {
                data->price = price;
                data->price_pow = price_pow;
            }
            data->value = amount;
            data->value_pow = amount_pow;
        } else {

            // Round volume
            uint8_t volume_precision = (round->volume_precision > 0) ? round->volume_precision : volume_pow;
            volume = itp_cl1_round_volume(price, price_pow, amount, amount_pow, volume_pow, volume_precision, round->volume_round);

            // Round amount
            amount_pow = decimal->amount_decimal;
            uint8_t amount_precision = (round->amount_precision > 0) ? round->amount_precision : amount_pow;
            amount = itp_cl1_round_amount(price, price_pow, volume, volume_pow, amount_pow, amount_precision, round->amount_round);

            // User order
            if (order->round_to_order) {
                order->price = data->price;
                order->price_pow = data->price_pow;
                order->amount = data->value;
                order->amount_pow = data->value_pow;
                order->volume = volume;
                order->volume_pow = volume_pow;
            } else {
                order->price = price;
                order->price_pow = price_pow;
                order->amount = amount;
                order->amount_pow = amount_pow;
                order->volume = volume;
                order->volume_pow = volume_pow;
            }

            // Dispenser order
            if (decimal->price_decimal != data->price_pow) {
                data->price = price;
                data->price_pow = price_pow;
            }
            if (round->amount_order == CL1_AUTHORIZE_ROUND) {
                data->value = amount;
                data->value_pow = amount_pow;
            } else {
                data->type = CL1_FUELING_TYPE_VOLUME;
                data->value = volume;
                data->value_pow = volume_pow;
            }
        }
    } else {

        // Temporary variables
        uint32_t amount = 0;
        uint8_t amount_pow = decimal->amount_decimal;
        uint32_t volume = value;
        uint8_t volume_pow = value_pow;

        // Round volume
        if (round->volume_precision > 0 &&
            volume_pow != round->volume_precision) {
            volume = tools_round_uint32(volume, volume_pow, round->volume_precision, round->volume_round);
            volume_pow = round->volume_precision;
        }
        if (volume_pow != decimal->volume_decimal) {
            volume = tools_round_uint32(volume, volume_pow, decimal->volume_decimal, round->volume_round);
            volume_pow = decimal->volume_decimal;
        }

        // Round amount
        uint8_t amount_precision = (round->amount_precision > 0) ? round->amount_precision : amount_pow;
        amount = itp_cl1_round_amount(price, price_pow, volume, volume_pow, amount_pow, amount_precision, round->amount_round);

        // User order
        if (order->round_to_order) {
            order->price = data->price;
            order->price_pow = data->price_pow;
            order->amount = amount;
            order->amount_pow = amount_pow;
            order->volume = data->value;
            order->volume_pow = data->value_pow;
        } else {
            order->price = price;
            order->price_pow = price_pow;
            order->amount = amount;
            order->amount_pow = amount_pow;
            order->volume = volume;
            order->volume_pow = volume_pow;
        }

        // Dispenser order
        if (decimal->price_decimal != data->price_pow) {
            data->price = price;
            data->price_pow = price_pow;
        }
        data->value = volume;
        data->value_pow = volume_pow;
    }
    return ITP_ERR_NONE;
}

itp_error_code_t itp_cl1_parse_round_option(itp_cl1_round_info_tp object, const char* key, const char* value) {

    if (!object || !key || !value) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_cl1_parse_round_option> Null pointer found");
        return ITP_ERRC_NULL_POINTER;
    }

    if (strcmp(key, "volume_precision") == 0) {
        object->volume_precision = itp_cl1_tools_string_to_uint8(value);
    } else if (strcmp(key, "amount_precision") == 0) {
        object->amount_precision = itp_cl1_tools_string_to_uint8(value);
    } else if (strcmp(key, "amount_order") == 0) {
        if (strcmp(value, "as_is") == 0) {
            object->amount_order = CL1_AUTHORIZE_AS_IS;
        } else if (strcmp(value, "round") == 0) {
            object->amount_order = CL1_AUTHORIZE_ROUND;
        } else if (strcmp(value, "volume") == 0) {
            object->amount_order = CL1_AUTHORIZE_VOLUME;
        } else {
            itp_debug_print(ITP_ERRC_BAD_ARGUMENT, "itp_cl1_parse_round_option> Bad %s value %s", key, value);
            return ITP_ERRC_BAD_ARGUMENT;
        }
    } else if (strcmp(key, "volume_round") == 0) {
        if (strcmp(value, "floor") == 0) {
            object->volume_round = CL1_ROUND_FLOOR;
        } else if (strcmp(value, "ceil") == 0) {
            object->volume_round = CL1_ROUND_CEIL;
        } else if (strcmp(value, "math") == 0 ||
                   strcmp(value, "nearest") == 0) {
            object->volume_round = CL1_ROUND_NEAREST;
        } else {
            itp_debug_print(ITP_ERRC_BAD_ARGUMENT, "itp_cl1_parse_round_option> Bad %s value %s", key, value);
            return ITP_ERRC_BAD_ARGUMENT;
        }
    } else if (strcmp(key, "amount_round") == 0) {
        if (strcmp(value, "floor") == 0) {
            object->amount_round = CL1_ROUND_FLOOR;
        } else if (strcmp(value, "ceil") == 0) {
            object->amount_round = CL1_ROUND_CEIL;
        } else if (strcmp(value, "math") == 0 ||
                   strcmp(value, "nearest") == 0) {
            object->amount_round = CL1_ROUND_NEAREST;
        } else {
            itp_debug_print(ITP_ERRC_BAD_ARGUMENT, "itp_cl1_parse_round_option> Bad %s value %s", key, value);
            return ITP_ERRC_BAD_ARGUMENT;
        }
    } else if (strcmp(key, "round_to_order") == 0) {
        object->round_to_order = itp_cl1_tools_string_to_bool(value);
    } else {
        return ITP_ERRC_NOT_IMPLEMENTED;
    }
    return ITP_ERRC_NONE;
}
