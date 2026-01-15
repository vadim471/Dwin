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



    const std::string DISPENSER_IDLE = "DSP_IDLE";
    const std::string DISPENSER_PISTOL_UP = "DSP_NOZZLE_UP";

    const std::string FUEL_VOLUME_ORDER_TYPE = "OPT_VOLUME";
    const std::string CREATE_ORDER = "create_order";
    const std::string SET_COMMAND = "set_command";

    const std::string DWIN_MESSAGE_TYPE_CHANGE_PAGE = "change_page";
    const std::string DWIN_MESSAGE_TYPE_CHANGE_NUMBER = "change_number";

    const int FUEL_TYPE_LEN = 6;

    const std::string TRK_ID = "trk1";
    const int TRK_FUEL_PRICE = 41;
}
