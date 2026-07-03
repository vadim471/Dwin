//
// Created by vadim.tissen on 25.12.2025.
//

#include <string>
#include "bridge/core/constant.hpp"
#include "../../config/version.hpp.in"

namespace bridge {

    const std::string PRIME_HTTP_LAYER = "HTTP_PRIME";
    const std::string BOS_HTTP_LAYER = "HTTP_BOS";
    const std::string UART_LAYER = "UART";
    const std::string PIPE_LAYER = "PIPE";
    const std::string HTTP_UNAUTHORIZED = "HTTP/1.1 401";
    const std::string CONFIG = "/home/vadim/gas_station_data/config.json";

    const std::string PAY_TRANSACTION = "PAYMENT_TRANSACTION";
    const std::string PAY_TRANSACTION_RESPONSE = "PAY_TRANSACTION_RESPONSE";
    const std::string PAY_CARD_RESOLVED = "PAY_CARD_RESOLVED";
    const std::string PAY_PIN_ENTERED = "PAY_PIN_ENTERED";
    const std::string PAY_CONFIRM = "PAY_CONFIRM";
    const std::string PAY_CANCEL = "PAY_CANCEL";
    const std::string PAY_CANCEL_RESPONSE_SUCCESS = "PAY_CANCEL_RESPONSE_SUCCESS";
    const std::string PAY_CONFIRM_RESPONSE_SUCCESS = "PAY_CONFIRM_RESPONSE_SUCCESS";
    const std::string PAY_PRINT_RECEIPT = "PAY_PRINT_RECEIPT";
    const std::string PAY_GET_BALANCE = "PAY_GET_BALANCE";
    const std::string ARKAIM_REQUEST = "ARKAIM_REQUEST";
    const std::string PRINT_DEBIT_RECEIPT = "PRINT_DEBIT_RECEIPT";
    const std::string PRINT_REFUND_RECEIPT = "PRINT_REFUND_RECEIPT";
    const std::string NO_PINPAD = "No pinpad";
    const std::string INCORECT_PINCODE = "Неверный ПИН";
    const std::string TITLE_BEGINNING_RECEPTION_FUEL = "ТЕКУЩИЕ ПОКАЗАНИЯ УРОВНЕМЕРА";
    const std::string TITLE_ENDING_RECEPTION_FUEL = "ОТЧЕТ ПРИЕМА ТОПЛИВА";
    const std::string TITLE_ENDING_BEFORE_RECEPTION_FUEL = "ЗАМЕР ПЕРЕД ПРИЕМОМ ТОПЛИВА";
    const std::string TITLE_ENDING_AFTER_RECEPTION_FUEL = "ЗАМЕР ПОСЛЕ ПРИЕМА ТОПЛИВА";

    const std::string GET_PRODUCTS = "get_products";
    const std::string GET_IO_PARAMETERS = "get_io_parameters";
    const std::string GET_DEVICES = "get_devices";
    const std::string GET_DISPENSER_STATUS = "get_dispenser_status";
    const std::string GET_EVENTS = "get_events";
    const std::string GET_TASKS = "get_tasks";
    const std::string GET_TANKERS = "get_takers";

    const std::string SET_SALES = "set_sales";
    const std::string SET_METROLOGICAL = "set_metrological";
    const std::string SET_FUEL_RECEIPTION = "set_fuel_reception";

    const std::string BOS_MESSAGE_SET_SALES = "BOS_REPORT_SALES";
    const std::string BOS_MESSAGE_SET_METROLOGICAL = "BOS_REPORT_LEVEL_FUEL";
    const std::string BOS_MESSAGE_SET_RECEIPTION = "BOS_MESSAGE_SET_RECEIPTION";

    const std::string USER_TOUCH_VOLUME_BUTTON = "VOLUME_BUTTON";
    const std::string USER_TOUCH_PAGINATION_TRK_BUTTON = "PAGINATION_TRK";
    const std::string USER_TOUCH_PAGINATION_FUEL_BUTTON = "PAGINATION_FUEL";
    const std::string USER_TOUCH_PAGINATION_RECEPTION_LEVEL_GAUGE_BUTTON = "PAGINATION_RECEPTION_LEVEL_GAUGE";
    const std::string USER_TOUCH_PAGINATION_LEVEL_GAUGE_BUTTON = "PAGINATION_LEVEL_GAUGE";
    const std::string USER_TOUCH_CHOOSE_MULTI_TRK_BUTTON = "CHOOSE_MULTI_TRK";
    const std::string USER_TOUCH_CHOOSE_TRK_BUTTON = "CHOOSE_TRK";
    const std::string USER_TOUCH_BASIC_TOUCH_BUTTON = "BASIC_TOUCH";
    const std::string USER_TOUCH_BASIC_TOUCH_BEGIN_FUELLING_BUTTON = "BASIC_TOUCH_BEGIN_FUELLING";
    const std::string USER_TOUCH_BASIC_TOUCH_CREATE_NEW_ORDER_BUTTON = "BASIC_TOUCH_CREATE_NEW_ORDER";
    const std::string USER_TOUCH_BASIC_TOUCH_ATTACH_CARD_BACK_BUTTON = "BASIC_TOUCH_ATTACH_CARD_BACK_BUTTON";
    const std::string USER_TOUCH_BASIC_TOUCH_DECLINE_ENTER_SERVICE_CODE_BUTTON = "BASIC_TOUCH_DECLINE_ENTER_SERVICE_CODE_BUTTON";
    const std::string USER_TOUCH_PIN_PAD_ENTER_FUEL_EDITING_BUTTON = "PIN_PAD_ENTER_FUEL_EDITING";
    const std::string USER_TOUCH_PIN_PAD_ENTER_FUEL_VOLUME_BUTTON = "PIN_PAD_ENTER_FUEL_VOLUME";
    const std::string USER_TOUCH_SERVICE_MENU_LEVEL_GAUGE_BUTTON = "SERVICE_MENU_LEVEL_GAUGE";
    const std::string USER_TOUCH_SERVICE_MENU_RECEPTION_LEVEL_GAUGE_BUTTON = "SERVICE_MENU_RECEPTION_LEVEL_GAUGE";
    const std::string USER_TOUCH_SERVICE_MENU_CHANGE_FUEL_PRICE_BUTTON = "SERVICE_MENU_CHANGE_FUEL_PRICE";
    const std::string USER_TOUCH_AMOUNT_TRK_BUTTON = "AMOUNT_TRK";
    const std::string USER_TOUCH_CHOOSE_FUEL_FOR_EDIT_BUTTON = "CHOOSE_FUEL_FOR_EDIT";
    const std::string USER_TOUCH_ACCEPT_SELECTED_TRK_BUTTON = "ACCEPT_SELECTED_TRK";
    const std::string USER_TOUCH_RESET_SELECTED_TRK_BUTTON = "RESET_SELECTED_TRK";
    const std::string USER_TOUCH_CLOSE_ORDER_BUTTON = "CLOSE_ORDER";
    const std::string USER_TOUCH_SET_USED_TRK_BUTTON = "SET_USED_TRK";
    const std::string USER_TOUCH_TAP_ON_USED_TRK_BUTTON = "TAP_ON_USED_TRK";
    const std::string USER_TOUCH_BASIC_TOUCH_CANCEL_TRANSACTION_BUTTON = "CANCEL_TRANSACTION";
    const std::string USER_TOUCH_PIN_PAD_ENTER_PIN_CODE_BUTTON = "PIN_PAD_ENTER_PIN_CODE";
    const std::string USER_TOUCH_PIN_PAD_ENTER_DOCUMENT_NUMBER_BUTTON = "PIN_PAD_ENTER_DOCUMENT_NUMBER";
    const std::string USER_TOUCH_ACCEPT_RECEPTION_FUEL_BUTTON = "ACCEPT_RECEPTION_FUEL";
    const std::string USER_TOUCH_FINISH_RECEPTION_FUEL_BUTTON = "FINISH_RECEPTION_FUEL";
    const std::string USER_TOUCH_CHOOSE_RECEPTION_LEVEL_GAUGE_BUTTON = "CHOOSE_RECEPTION_LEVEL_GAUGE";
    const std::string USER_TOUCH_CANCEL_TRANSACTION_AFTER_CANCEL_ENTER_PIN = "CANCEL_TRANSACTION_AFTER_CANCEL_PIN";

    // const std::string AUDIO_PLAY_WELCOME = "PLAY_WELCOME";
    // const std::string AUDIO_PLAY_FUELLING_END = "PLAY_FUELLING_END";
    // const std::string AUDIO_PLAY_FUELLING_INTERRUPTED = "PLAY_FUELLING_INTERRUPTED";

    const std::string HTTP_RESPONSE = "HTTP_RESPONSE";

    const std::string DISPENSER_IDLE = "DSP_IDLE";
    const std::string DISPENSER_NOZZLE_UP = "DSP_NOZZLE_UP";
    const std::string DISPENSER_AUTHORIZED = "DSP_AUTHORIZED";
    const std::string DISPENSER_FUELLING = "DSP_FUELLING";
    const std::string DISPENSER_COMPLETE = "DSP_COMPLETE";
    const std::string DISPENSER_HALTED = "DSP_HALTED";
    const std::string DISPENSER_OFFLINE = "DSP_OFFLINE";
    const std::string DISPENSER_ERROR = "DSP_ERROR";
    const std::string DISPENSER_LOCKED = "DSP_LOCKED";

    const std::string UPPER_VOLUME = "IO_UPPER_VOLUME";
    const std::string LOWER_VOLUME = "IO_LOWER_VOLUME";
    const std::string TOTAL_VOLUME = "IO_TOTAL_VOLUME";
    const std::string UPPER_LEVEL = "IO_UPPER_LEVEL";
    const std::string LOWER_LEVEL = "IO_LOWER_LEVEL";
    const std::string FILLING = "IO_FILLING";
    const std::string DENSITY = "IO_DENSITY";
    const std::string WEIGHT = "IO_WEIGHT";

    const std::string RATING_PR_92 = "PR_92";
    const std::string RATING_PR_95 = "PR_95";
    const std::string RATING_PR_98 = "PR_98";
    const std::string RATING_PR_100 = "PR_100";
    const std::string RATING_PR_DIESEL = "PR_DIESEL";

    const std::string API_DISPENSER = "API_DISPENSER";
    const std::string API_IO = "API_IO";

    const std::string ORDER_IDLE = "ORD_IDLE";
    const std::string ORDER_READY = "ORD_READY";
    const std::string ORDER_AUTHORIZED = "ORD_AUTHORIZED";
    const std::string ORDER_IN_PROGRESS = "ORD_IN_PROGRESS";
    const std::string ORDER_DELIVERED = "ORD_DELIVERED";
    const std::string ORDER_INTERRUPTED = "ORD_INTERRUPTED";
    const std::string ORDER_FAILED = "ORD_FAILED";

    const std::string LOW_VOLUME = "Product level too low";

    const std::string FUEL_VOLUME_ORDER_TYPE = "OTP_VOLUME";
    const std::string CREATE_ORDER = "create_order";
    const std::string SET_COMMAND = "set_command";

    const std::string DWIN_MESSAGE_TYPE_CHANGE_PAGE = "change_page";
    const std::string DWIN_MESSAGE_TYPE_CHANGE_NUMBER = "change_number";
    const std::string DWIN_MESSAGE_TYPE_WRITE_VP = "write_vp";
    const std::string DWIN_MESSAGE_TYPE_PLAY_SOUND = "PLAY_SOUND";
    const std::string DWIN_MESSAGE_TYPE_SET_BRIGHTNESS = "SET_BRIGHTNESS";

    const std::string APP_VERSION = APP_VERSION_STRING;

    const int FUEL_TYPE_LEN = 6;
    const int TRK_ID_LEN = 6;

    const int VP_TEXT_STEP = 4;

    const int POCO_ERROR = 599;

    const std::string UPDATE_CURRENT_ORDER_AMOUNT = "UPDATE_AMOUNT_DISPLAY";
    const std::string UPDATE_CURRENT_ORDER_VOLUME = "UPDATE_VOLUME_DISPLAY";
    const std::string UPDATE_CURRENT_FUELLING_VOLUME = "UPDATE_FUELLING_VOLUME_DISPLAY";
    const std::string UPDATE_CURRENT_FUELLING_AMOUNT = "UPDATE_FUELLING_AMOUNT_DISPLAY";
    const std::string CLEAR_CURRENT_ORDER_AMOUNT_AND_VOLUME = "CLEAR_AMOUNT_AND_VOLUME_DISPLAY";
    const std::string ON_GET_BALANCE_RESPONSE = "GET_BALANCE_RESPONSE";

    const std::string GET_BALANCE_TYPE = "GET_BALANCE";
    const std::string GET_PAYMENT_TYPE = "GET_PAYMENT";

    const std::string HIDDEN_CARD_BALANCE_OPTIMA_PC = "Баланс по кошелькам скрыт!";

    const std::string ARKAIM_INITIALIZED = "174";

    const std::string PAY_REVERT = "PAY_REVERT";
    const std::string NON_RECOGNIZED_ERROR = "NON_RECOGNIZED_ERROR";
    const std::string NOT_ENOUGH_MONEY = "Недостаточно средств";
    const std::string ON_CARD_DETECTED = "CARD_DETECTED";
    const std::string CLEAR_CARD_DATA_REQUEST = "CLEAR_CARD_DATA_REQUEST";

    const std::string ERROR_SERVER_PROCESSING_TYPE = "PROCESSING_ERROR";

    const int ERROR_STATUS_CODE = 400;
}
