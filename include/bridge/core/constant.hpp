//
// Created by vadim.tissen on 25.12.2025.
//

#pragma once
#include <string>

namespace bridge {
    // Logic layers
    extern const std::string PRIME_HTTP_LAYER;
    extern const std::string BOS_HTTP_LAYER;
    extern const std::string UART_LAYER;
    extern const std::string PIPE_LAYER;

    //Arkaim const
    extern const std::string PAY_TRANSACTION;
    extern const std::string PAY_TRANSACTION_RESPONSE;
    extern const std::string PAY_CONFIRM;
    extern const std::string PAY_CANCEL;
    extern const std::string PAY_CANCEL_RESPONSE_SUCCESS;
    extern const std::string PAY_CONFIRM_RESPONSE_SUCCESS;
    extern const std::string PAY_CARD_RESOLVED;
    extern const std::string PAY_PIN_ENTERED;
    extern const std::string PAY_PRINT_RECEIPT;
    extern const std::string PAY_GET_BALANCE;
    extern const std::string ARKAIM_REQUEST;
    extern const std::string PRINT_DEBIT_RECEIPT;
    extern const std::string PRINT_REFUND_RECEIPT;
    extern const std::string NO_PINPAD;
    extern const std::string INCORECT_PINCODE;
    extern const std::string TITLE_BEGINNING_RECEPTION_FUEL;
    extern const std::string TITLE_ENDING_RECEPTION_FUEL;
    extern const std::string TITLE_ENDING_BEFORE_RECEPTION_FUEL;
    extern const std::string TITLE_ENDING_AFTER_RECEPTION_FUEL;
    extern const std::string NON_RECOGNIZED_ERROR;
    extern const std::string NOT_ENOUGH_MONEY;

    // Path to config file
    extern const std::string CONFIG;

    // Type for MESSAGE Struct
    extern const std::string DWIN_MESSAGE_TYPE_CHANGE_PAGE;
    extern const std::string DWIN_MESSAGE_TYPE_CHANGE_NUMBER;
    extern const std::string DWIN_MESSAGE_TYPE_WRITE_VP;
    extern const std::string DWIN_MESSAGE_TYPE_PLAY_SOUND;
    extern const std::string DWIN_MESSAGE_TYPE_SET_BRIGHTNESS;
    extern const std::string HTTP_MESSAGE_TYPE_RESPONSE;
    extern const std::string FUEL_VOLUME_ORDER_TYPE;
    extern const std::string CREATE_ORDER;
    extern const std::string PAY_REVERT;

    // Type for MESSAGE Struct BOS part
    extern const std::string BOS_MESSAGE_SET_SALES;
    extern const std::string BOS_MESSAGE_SET_METROLOGICAL;
    extern const std::string BOS_MESSAGE_SET_RECEIPTION;

    //DWIN const
    extern const int FUEL_TYPE_LEN;
    extern const int VP_TEXT_STEP;
    extern const int TRK_ID_LEN;

    extern const std::string UPDATE_CURRENT_ORDER_AMOUNT;
    extern const std::string UPDATE_CURRENT_ORDER_VOLUME;
    extern const std::string UPDATE_CURRENT_FUELLING_VOLUME;
    extern const std::string UPDATE_CURRENT_FUELLING_AMOUNT;
    extern const std::string CLEAR_CURRENT_ORDER_AMOUNT_AND_VOLUME;

    // Const for HTTP Layer message
    extern const std::string HTTP_RESPONSE;

    // Const for Display Touch to HttpLayer
    extern const std::string USER_TOUCH_VOLUME_BUTTON;
    extern const std::string USER_TOUCH_PAGINATION_TRK_BUTTON;
    extern const std::string USER_TOUCH_PAGINATION_FUEL_BUTTON;
    extern const std::string USER_TOUCH_PAGINATION_RECEPTION_LEVEL_GAUGE_BUTTON;
    extern const std::string USER_TOUCH_PAGINATION_LEVEL_GAUGE_BUTTON;
    extern const std::string USER_TOUCH_CHOOSE_MULTI_TRK_BUTTON;
    extern const std::string USER_TOUCH_CHOOSE_TRK_BUTTON;
    extern const std::string USER_TOUCH_BASIC_TOUCH_BUTTON;
    extern const std::string USER_TOUCH_BASIC_TOUCH_BEGIN_FUELLING_BUTTON;
    extern const std::string USER_TOUCH_BASIC_TOUCH_CREATE_NEW_ORDER_BUTTON;
    extern const std::string USER_TOUCH_BASIC_TOUCH_ATTACH_CARD_BACK_BUTTON;
    extern const std::string USER_TOUCH_BASIC_TOUCH_DECLINE_ENTER_SERVICE_CODE_BUTTON;
    extern const std::string USER_TOUCH_PIN_PAD_ENTER_FUEL_EDITING_BUTTON;
    extern const std::string USER_TOUCH_PIN_PAD_ENTER_FUEL_VOLUME_BUTTON;
    extern const std::string USER_TOUCH_SERVICE_MENU_LEVEL_GAUGE_BUTTON;
    extern const std::string USER_TOUCH_SERVICE_MENU_RECEPTION_LEVEL_GAUGE_BUTTON;
    extern const std::string USER_TOUCH_SERVICE_MENU_CHANGE_FUEL_PRICE_BUTTON;
    extern const std::string USER_TOUCH_AMOUNT_TRK_BUTTON;
    extern const std::string USER_TOUCH_CHOOSE_FUEL_FOR_EDIT_BUTTON;
    extern const std::string USER_TOUCH_ACCEPT_SELECTED_TRK_BUTTON;
    extern const std::string USER_TOUCH_RESET_SELECTED_TRK_BUTTON;
    extern const std::string USER_TOUCH_CLOSE_ORDER_BUTTON;
    extern const std::string USER_TOUCH_SET_USED_TRK_BUTTON;
    extern const std::string USER_TOUCH_TAP_ON_USED_TRK_BUTTON;
    extern const std::string USER_TOUCH_BASIC_TOUCH_CANCEL_TRANSACTION_BUTTON;
    extern const std::string USER_TOUCH_PIN_PAD_ENTER_PIN_CODE_BUTTON;
    extern const std::string USER_TOUCH_PIN_PAD_ENTER_DOCUMENT_NUMBER_BUTTON;
    extern const std::string USER_TOUCH_ACCEPT_RECEPTION_FUEL_BUTTON;
    extern const std::string USER_TOUCH_FINISH_RECEPTION_FUEL_BUTTON;
    extern const std::string USER_TOUCH_CHOOSE_RECEPTION_LEVEL_GAUGE_BUTTON;
    extern const std::string USER_TOUCH_CANCEL_TRANSACTION_AFTER_CANCEL_ENTER_PIN;

    // HTTP GET
    extern const std::string GET_PRODUCTS;
    extern const std::string GET_IO_PARAMETERS;
    extern const std::string GET_DEVICES;
    extern const std::string GET_DISPENSER_STATUS;
    extern const std::string GET_EVENTS;
    extern const std::string GET_TASKS;
    extern const std::string GET_TANKERS;

    // BOS POST
    extern const std::string SET_SALES;
    extern const std::string SET_METROLOGICAL;
    extern const std::string SET_FUEL_RECEIPTION;

    // HTTP POST
    extern const std::string SET_COMMAND;

    // Prime API const
    extern const std::string DISPENSER_IDLE;
    extern const std::string DISPENSER_NOZZLE_UP;
    extern const std::string DISPENSER_AUTHORIZED;
    extern const std::string DISPENSER_FUELLING;
    extern const std::string DISPENSER_COMPLETE;
    extern const std::string DISPENSER_HALTED;
    extern const std::string DISPENSER_OFFLINE;
    extern const std::string DISPENSER_ERROR;
    extern const std::string DISPENSER_LOCKED;

    extern const std::string UPPER_VOLUME;
    extern const std::string LOWER_VOLUME;
    extern const std::string TOTAL_VOLUME;
    extern const std::string UPPER_LEVEL;
    extern const std::string LOWER_LEVEL;
    extern const std::string FILLING;
    extern const std::string DENSITY;
    extern const std::string WEIGHT;

    extern const std::string RATING_PR_92;
    extern const std::string RATING_PR_95;
    extern const std::string RATING_PR_98;
    extern const std::string RATING_PR_100;
    extern const std::string RATING_PR_DIESEL;

    extern const std::string API_DISPENSER;
    extern const std::string API_IO;

    extern const std::string ORDER_IDLE;
    extern const std::string ORDER_READY;
    extern const std::string ORDER_AUTHORIZED;
    extern const std::string ORDER_IN_PROGRESS;
    extern const std::string ORDER_DELIVERED;
    extern const std::string ORDER_INTERRUPTED;
    extern const std::string ORDER_FAILED;

    extern const std::string LOW_VOLUME;

    // Application version
    extern const std::string APP_VERSION;

    // Error codes
    extern const int POCO_ERROR;

    extern const std::string ON_GET_BALANCE_RESPONSE;
    extern const std::string GET_BALANCE_TYPE;
    extern const std::string GET_PAYMENT_TYPE;

    extern const std::string ARKAIM_INITIALIZED;
}