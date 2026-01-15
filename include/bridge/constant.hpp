//
// Created by vadim.tissen on 25.12.2025.
//

#pragma once
#include <string>

namespace bridge {

    // Logic layers
    extern const std::string HTTP_LAYER;
    extern const std::string UART_LAYER;

    // Path to config file
    extern const std::string CONFIG;

    // Type for MESSAGE Struct
    extern const std::string DWIN_MESSAGE_TYPE_CHANGE_PAGE;
    extern const std::string DWIN_MESSAGE_TYPE_CHANGE_NUMBER;
    extern const std::string HTTP_MESSAGE_TYPE_RESPONSE;
    extern const std::string FUEL_VOLUME_ORDER_TYPE;

    extern const int FUEL_TYPE_LEN;

    extern const std::string CREATE_ORDER;

    // TRK const
    extern const std::string TRK_ID;
    extern const int TRK_FUEL_PRICE;


    // HTTP GET
    extern const std::string GET_PRODUCTS;
    extern const std::string GET_IO_PARAMETERS;
    extern const std::string GET_DEVICES;
    extern const std::string GET_DISPENSER_STATUS;
    extern const std::string GET_EVENTS;
    extern const std::string GET_TASKS;

    // HTTP POST
    extern const std::string SET_COMMAND;

    // Prime API const
    extern const std::string DISPENSER_IDLE;
    extern const std::string DISPENSER_PISTOL_UP;

}
