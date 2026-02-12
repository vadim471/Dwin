//
// Created by vadim.tissen on 25.12.2025.
//

#include <string>
#include "bridge/constant.hpp"

namespace bridge {

    const std::string HTTP_LAYER = "HTTP";
    const std::string UART_LAYER = "UART";
    const std::string HTTP_UNAUTHORIZED = "HTTP/1.1 401";
    const std::string CONFIG = "config.json";

    const std::string GET_PRODUCTS = "get_products";
    const std::string GET_IO_PARAMETERS = "get_io_parameters";
    const std::string GET_DEVICES = "get_devices";
    const std::string GET_DISPENSER_STATUS = "get_dispenser_status";
    const std::string GET_EVENTS = "get_events";
    const std::string GET_TASKS = "get_tasks";

    const std::string USER_TOUCH = "USER_TOUCH";
    const std::string HTTP_RESPONSE = "HTTP_RESPONSE";

    const std::string DISPENSER_IDLE = "DSP_IDLE";
    const std::string DISPENSER_NOZZLE_UP = "DSP_NOZZLE_UP";
    const std::string DISPENSER_AUTHORIZED = "DSP_AUTHORIZED";
    const std::string DISPENSER_FUELING = "DSP_FUELING";
    const std::string DISPENSER_COMPLETE = "DSP_COMPLETE";
    const std::string DISPENSER_HALTED = "DSP_HALTED";
    const std::string DISPENSER_OFFLINE = "DSP_OFFLINE";
    const std::string DISPENSER_ERROR = "DSP_ERROR";

    const std::string RATING_PR_92 = "PR_92";
    const std::string RATING_PR_95 = "PR_95";
    const std::string RATING_PR_98 = "PR_98";
    const std::string RATING_PR_100 = "PR_100";
    const std::string RATING_PR_DIESEL = "PR_DIESEL";

    const std::string API_DISPENSER = "API_DISPENSER";

    const std::string FUEL_VOLUME_ORDER_TYPE = "OTP_VOLUME";
    const std::string CREATE_ORDER = "create_order";
    const std::string SET_COMMAND = "set_command";

    const std::string DWIN_MESSAGE_TYPE_CHANGE_PAGE = "change_page";
    const std::string DWIN_MESSAGE_TYPE_CHANGE_NUMBER = "change_number";

    const int FUEL_TYPE_LEN = 6;
    const int TRK_ID_LEN = 6;

    const int TRK_FUEL_PRICE = 4100;
    const int VP_TEXT_STEP = 4;

}
