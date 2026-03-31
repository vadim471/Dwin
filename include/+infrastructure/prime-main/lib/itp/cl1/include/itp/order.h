#ifndef ITP_CL1_ORDER_H_INCLUDED
#define ITP_CL1_ORDER_H_INCLUDED

#include <itp/enum.h>
#include <itp/error_code.h>
#include "error_code.h"

#include <stdint.h>

/**
 * \file tools.h
 * \brief Файл функции и структуры округления заказа.
 */

struct itp_cl1_order_info;
typedef struct itp_cl1_order_info itp_cl1_order_info_t;
typedef itp_cl1_order_info_t* itp_cl1_order_info_tp;

struct itp_cl1_order_data;
typedef struct itp_cl1_order_data itp_cl1_order_data_t;
typedef itp_cl1_order_data_t* itp_cl1_order_data_tp;

struct itp_cl1_round_info;
typedef struct itp_cl1_round_info itp_cl1_round_info_t;
typedef itp_cl1_round_info_t* itp_cl1_round_info_tp;

struct itp_cl1_decimal_info;
typedef struct itp_cl1_decimal_info itp_cl1_decimal_info_t;
typedef itp_cl1_decimal_info_t* itp_cl1_decimal_info_tp;

struct itp_cl1_dispenser;
typedef struct itp_cl1_dispenser itp_cl1_dispenser_t;
typedef itp_cl1_dispenser_t* itp_cl1_dispenser_tp;

struct itp_cl1_dispenser_list;
typedef struct itp_cl1_dispenser_list itp_cl1_dispenser_list_t;
typedef itp_cl1_dispenser_list_t* itp_cl1_dispenser_list_tp;

/**
 * \brief Данные для верхнего уровня.
 */
struct itp_cl1_order_info {
    uint32_t price;
    uint8_t price_pow;
    uint32_t amount;
    uint8_t amount_pow;
    uint32_t volume;
    uint8_t volume_pow;
    uint8_t round_to_order;
};

/**
 * \brief Входные данные и данные для ТРК.
 */
struct itp_cl1_order_data {
    uint8_t type;
    uint32_t price;
    uint8_t price_pow;
    uint32_t value;
    uint8_t value_pow;
    uint8_t round_to_order;
};

struct itp_cl1_round_info {
    uint8_t volume_precision;
    uint8_t amount_precision;
    uint8_t volume_round;
    uint8_t amount_round;
    uint8_t amount_order;
    uint8_t round_to_order;
};

struct itp_cl1_decimal_info {
    uint8_t price_decimal;
    uint8_t volume_decimal;
    uint8_t amount_decimal;
};

struct itp_cl1_dispenser {
    uint8_t address;
    itp_cl1_order_info_t order;
    itp_cl1_dispenser_tp next;
};

struct itp_cl1_dispenser_list {
    itp_cl1_dispenser_tp first;
};

void itp_cl1_init_order_info(itp_cl1_order_info_tp);

void itp_cl1_init_order_data(itp_cl1_order_data_tp);

void itp_cl1_init_round_info(itp_cl1_round_info_tp);

void itp_cl1_init_decimal_info(itp_cl1_decimal_info_tp);

void itp_cl1_init_dispenser(itp_cl1_dispenser_tp, uint8_t);

void itp_cl1_free_dispenser(itp_cl1_dispenser_tp);

void itp_cl1_init_dispenser_list(itp_cl1_dispenser_list_tp);

itp_cl1_dispenser_tp itp_cl1_add_dispenser(itp_cl1_dispenser_list_tp, uint8_t);

itp_cl1_dispenser_tp itp_cl1_find_dispenser(itp_cl1_dispenser_list_tp, uint8_t);

void itp_cl1_dispose_dispenser_list(itp_cl1_dispenser_list_tp);

uint16_t itp_cl1_round_order(itp_cl1_order_data_tp, itp_cl1_round_info_tp, itp_cl1_decimal_info_tp, itp_cl1_order_info_tp);

itp_error_code_t itp_cl1_parse_round_option(itp_cl1_round_info_tp, const char*, const char*);

#endif // ITP_CL1_ORDER_H_INCLUDED
