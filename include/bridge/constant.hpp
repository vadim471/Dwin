//
// Created by vadim.tissen on 25.12.2025.
//

#pragma once
#include <map>
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
    extern const std::string CREATE_ORDER;

    //DWIN const
    extern const int FUEL_TYPE_LEN;
    extern const int VP_TEXT_STEP;
    extern const int TRK_ID_LEN;

    // Const for HTTP Layer message
    extern const std::string USER_TOUCH;
    extern const std::string HTTP_RESPONSE;

    // TRK const
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
    extern const std::string DISPENSER_NOZZLE_UP;
    extern const std::string DISPENSER_AUTHORIZED;
    extern const std::string DISPENSER_FUELING;
    extern const std::string DISPENSER_COMPLETE;
    extern const std::string DISPENSER_HALTED;
    extern const std::string DISPENSER_OFFLINE;
    extern const std::string DISPENSER_ERROR;

    extern const std::string RATING_PR_92;
    extern const std::string RATING_PR_95;
    extern const std::string RATING_PR_98;
    extern const std::string RATING_PR_100;
    extern const std::string RATING_PR_DIESEL;

    extern const std::string API_DISPENSER;

}
